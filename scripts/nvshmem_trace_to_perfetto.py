#!/usr/bin/env python3
"""
Convert NVSHMEM libfabric LTTng traces to Perfetto JSON format.

Usage:
    # Capture traces
    lttng create nvshmem-trace
    lttng enable-channel --userspace ch0
    lttng enable-event --userspace --channel ch0 'nvshmem_libfabric:*'
    lttng start
    <run workload>
    lttng stop
    lttng destroy

    # Convert to Perfetto JSON
    babeltrace2 ~/lttng-traces/nvshmem-trace -o ctf-metadata | head  # verify
    python3 nvshmem_trace_to_perfetto.py ~/lttng-traces/nvshmem-trace -o trace.json

    # Open in https://ui.perfetto.dev
"""

import argparse
import json
import subprocess
import sys
import re
from collections import defaultdict


def parse_babeltrace_line(line):
    """Parse a babeltrace2 text output line into structured data."""
    # Format: [HH:MM:SS.NNNNNNNNN] (+delta) hostname provider:event: { cpu_id = N }, { field = val, ... }
    m = re.match(
        r'\[(\d+:\d+:\d+\.\d+)\]\s+\(\+[\d.]+\)\s+(\S+)\s+(\S+):\s+\{[^}]*\},\s+\{(.*)\}',
        line.strip()
    )
    if not m:
        return None

    timestamp_str, hostname, event_name, fields_str = m.groups()

    # Parse timestamp to nanoseconds
    parts = timestamp_str.split(':')
    h, mi = int(parts[0]), int(parts[1])
    sec_parts = parts[2].split('.')
    s = int(sec_parts[0])
    ns = int(sec_parts[1])
    ts_ns = ((h * 3600 + mi * 60 + s) * 1_000_000_000) + ns

    # Parse fields
    fields = {}
    for pair in fields_str.split(','):
        pair = pair.strip()
        if '=' in pair:
            k, v = pair.split('=', 1)
            k = k.strip()
            v = v.strip()
            try:
                fields[k] = int(v)
            except ValueError:
                fields[k] = v

    return {
        'ts_ns': ts_ns,
        'hostname': hostname,
        'event': event_name,
        'fields': fields,
    }


def convert_to_perfetto(trace_path, output_path, max_events=None):
    """Convert LTTng trace to Perfetto JSON."""
    cmd = ['babeltrace', trace_path]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    events = []
    first_ts = None
    outstanding = defaultdict(int)  # per (hostname, domain_index)
    progress_starts = {}  # (hostname, domain_index) -> ts
    amo_starts = {}  # (hostname, domain_index) -> ts
    count = 0

    for line in proc.stdout:
        if 'nvshmem_libfabric' not in line:
            continue

        parsed = parse_babeltrace_line(line)
        if not parsed:
            continue

        if first_ts is None:
            first_ts = parsed['ts_ns']

        ts_us = (parsed['ts_ns'] - first_ts) / 1000.0  # microseconds for Perfetto
        hostname = parsed['hostname']
        event = parsed['event']
        fields = parsed['fields']
        domain = fields.get('domain_index', 0)
        tid = f"{hostname}-d{domain}"

        # Duration events: progress_start/end
        if event == 'nvshmem_libfabric:progress_start':
            progress_starts[(hostname, domain)] = ts_us
        elif event == 'nvshmem_libfabric:progress_end':
            start = progress_starts.pop((hostname, domain), None)
            if start is not None:
                events.append({
                    'name': 'progress',
                    'cat': 'progress',
                    'ph': 'X',  # complete event
                    'ts': start,
                    'dur': ts_us - start,
                    'pid': hostname,
                    'tid': tid,
                    'args': {'status': fields.get('status', 0)},
                })

        # Duration events: perform_gdrcopy_amo start/end
        elif event == 'nvshmem_libfabric:perform_gdrcopy_amo_start':
            amo_starts[(hostname, domain)] = ts_us
        elif event == 'nvshmem_libfabric:perform_gdrcopy_amo_end':
            start = amo_starts.pop((hostname, domain), None)
            if start is not None:
                events.append({
                    'name': 'perform_gdrcopy_amo',
                    'cat': 'amo',
                    'ph': 'X',
                    'ts': start,
                    'dur': ts_us - start,
                    'pid': hostname,
                    'tid': tid,
                    'args': {'status': fields.get('status', 0)},
                })

        # Instant events: post operations
        elif event == 'nvshmem_libfabric:rma_impl':
            events.append({
                'name': 'rma_impl',
                'cat': 'post',
                'ph': 'i',
                'ts': ts_us,
                'pid': hostname,
                'tid': tid,
                's': 't',
                'args': {
                    'pe': fields.get('pe', 0),
                    'op_size': fields.get('op_size', 0),
                    'verb': fields.get('verb_desc', 0),
                },
            })

        elif event == 'nvshmem_libfabric:gdrcopy_amo_ack':
            events.append({
                'name': 'gdrcopy_amo_ack',
                'cat': 'post',
                'ph': 'i',
                'ts': ts_us,
                'pid': hostname,
                'tid': tid,
                's': 't',
                'args': {'pe': fields.get('pe', 0), 'seq': fields.get('sequence_count', 0)},
            })

        elif event == 'nvshmem_libfabric:gdr_amo':
            events.append({
                'name': 'gdr_amo',
                'cat': 'post',
                'ph': 'i',
                'ts': ts_us,
                'pid': hostname,
                'tid': tid,
                's': 't',
                'args': {'pe': fields.get('pe', 0), 'op': fields.get('op_type', 0)},
            })

        elif event == 'nvshmem_libfabric:gdr_signal':
            events.append({
                'name': 'gdr_signal',
                'cat': 'post',
                'ph': 'i',
                'ts': ts_us,
                'pid': hostname,
                'tid': tid,
                's': 't',
                'args': {
                    'pe': fields.get('pe', 0),
                    'seq': fields.get('sequence_count', 0),
                    'num_writes': fields.get('num_writes', 0),
                },
            })

        # Instant events: completions
        elif event == 'nvshmem_libfabric:completion_send':
            events.append({
                'name': 'send_completion',
                'cat': 'completion',
                'ph': 'i',
                'ts': ts_us,
                'pid': hostname,
                'tid': tid,
                's': 't',
            })

        elif event == 'nvshmem_libfabric:completion_write':
            events.append({
                'name': 'write_completion',
                'cat': 'completion',
                'ph': 'i',
                'ts': ts_us,
                'pid': hostname,
                'tid': tid,
                's': 't',
            })

        elif event == 'nvshmem_libfabric:completion_signal':
            events.append({
                'name': 'signal_completion',
                'cat': 'completion',
                'ph': 'i',
                'ts': ts_us,
                'pid': hostname,
                'tid': tid,
                's': 't',
                'args': {'seq': fields.get('seq_num', 0)},
            })

        elif event == 'nvshmem_libfabric:completion_amo':
            events.append({
                'name': 'amo_completion',
                'cat': 'completion',
                'ph': 'i',
                'ts': ts_us,
                'pid': hostname,
                'tid': tid,
                's': 't',
            })

        # Counter events: outstanding operations
        elif event == 'nvshmem_libfabric:outstanding':
            cnt = fields.get('count', 0)
            events.append({
                'name': 'outstanding_ops',
                'cat': 'counter',
                'ph': 'C',
                'ts': ts_us,
                'pid': hostname,
                'tid': tid,
                'args': {'outstanding': cnt},
            })

        count += 1
        if max_events and count >= max_events:
            break

    proc.wait()

    trace = {'traceEvents': events}
    with open(output_path, 'w') as f:
        json.dump(trace, f)

    print(f"Wrote {len(events)} Perfetto events from {count} LTTng events to {output_path}")


def main():
    parser = argparse.ArgumentParser(description='Convert NVSHMEM LTTng traces to Perfetto JSON')
    parser.add_argument('trace_path', help='Path to LTTng trace directory')
    parser.add_argument('-o', '--output', default='nvshmem_trace.json', help='Output JSON file')
    parser.add_argument('-n', '--max-events', type=int, help='Max events to process')
    args = parser.parse_args()

    convert_to_perfetto(args.trace_path, args.output, args.max_events)


if __name__ == '__main__':
    main()
