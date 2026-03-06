/*
 * Copyright (c) 2026, Amazon.com, Inc. or its affiliates. All rights reserved.
 *
 * NVSHMEM libfabric transport LTTng tracepoint definitions.
 *
 * Usage: Build libfabric with --enable-lttng, build NVSHMEM with
 * -DNVSHMEM_LIBFABRIC_TRACE=ON, then use lttng to capture traces
 * and convert to Perfetto JSON for visualization.
 */

#undef LTTNG_UST_TRACEPOINT_PROVIDER
#define LTTNG_UST_TRACEPOINT_PROVIDER nvshmem_libfabric

#undef LTTNG_UST_TRACEPOINT_INCLUDE
#define LTTNG_UST_TRACEPOINT_INCLUDE "./nvshmem_libfabric_trace.h"

#if !defined(_NVSHMEM_LIBFABRIC_TRACE_H) || defined(LTTNG_UST_TRACEPOINT_HEADER_MULTI_READ)
#define _NVSHMEM_LIBFABRIC_TRACE_H

#include <lttng/tracepoint.h>
#include <stdint.h>

/*
 * rma_impl — fired on every RMA put/get operation
 * Increment outstanding counter here.
 */
LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    rma_impl,
    LTTNG_UST_TP_ARGS(
        int, pe,
        int, domain_index,
        uint64_t, op_size,
        int, verb_desc,
        uint64_t, remote_addr
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, pe, pe)
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(uint64_t, op_size, op_size)
        lttng_ust_field_integer(int, verb_desc, verb_desc)
        lttng_ust_field_integer(uint64_t, remote_addr, remote_addr)
    )
)

/*
 * gdrcopy_amo_ack — fired when sending AMO ack via write-with-imm
 * Increment outstanding counter.
 */
LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    gdrcopy_amo_ack,
    LTTNG_UST_TP_ARGS(
        int, pe,
        int, domain_index,
        uint32_t, sequence_count
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, pe, pe)
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(uint32_t, sequence_count, sequence_count)
    )
)

/*
 * perform_gdrcopy_amo — start and end of GDRCopy AMO processing
 * Duration event. Increment outstanding on start.
 */
LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    perform_gdrcopy_amo_start,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        int, op_type
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(int, op_type, op_type)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    perform_gdrcopy_amo_end,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        int, status
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(int, status, status)
    )
)

/*
 * gdr_amo — fired when posting a GDR AMO operation
 * Increment outstanding counter.
 */
LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    gdr_amo,
    LTTNG_UST_TP_ARGS(
        int, pe,
        int, domain_index,
        int, op_type
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, pe, pe)
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(int, op_type, op_type)
    )
)

/*
 * gdr_signal — fired when posting a signal operation
 * Increment outstanding counter.
 */
LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    gdr_signal,
    LTTNG_UST_TP_ARGS(
        int, pe,
        int, domain_index,
        uint32_t, sequence_count,
        uint16_t, num_writes
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, pe, pe)
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(uint32_t, sequence_count, sequence_count)
        lttng_ust_field_integer(uint16_t, num_writes, num_writes)
    )
)

/*
 * single_ep_progress — start and end of progress loop iteration
 * Duration event.
 */
LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    progress_start,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        int, qp_index
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(int, qp_index, qp_index)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    progress_end,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        int, status
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(int, status, status)
    )
)

/*
 * Completion events — fired inside gdr_process_completion
 * Decrement outstanding counter on send/write completions.
 */
LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    completion_send,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        uint64_t, wr_id
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(uint64_t, wr_id, wr_id)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    completion_write,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        uint64_t, wr_id
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(uint64_t, wr_id, wr_id)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    completion_signal,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        uint64_t, addr,
        uint32_t, seq_num
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(uint64_t, addr, addr)
        lttng_ust_field_integer(uint32_t, seq_num, seq_num)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    completion_amo,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        uint64_t, wr_id
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(uint64_t, wr_id, wr_id)
    )
)

/*
 * Outstanding counter — tracks inflight operations
 */
LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    outstanding,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        int64_t, count
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(int64_t, count, count)
    )
)


/*
 * put_signal_ack_completion — receiver gets ack for standalone put
 */
LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    put_signal_ack_completion,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        uint64_t, addr
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(uint64_t, addr, addr)
    )
)

/*
 * gdr_process_ack — duration of ack processing loop
 */
LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    gdr_process_ack_start,
    LTTNG_UST_TP_ARGS(
        int, domain_index
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    gdr_process_ack_end,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        int, status
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(int, status, status)
    )
)

/*
 * gdr_process_amo — duration of AMO processing loop
 */
LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    gdr_process_amo_start,
    LTTNG_UST_TP_ARGS(
        int, domain_index
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    gdr_process_amo_end,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        int, status
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(int, status, status)
    )
)

/*
 * signal_delivered — signal actually delivered to GPU after ordering
 */
LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    signal_delivered,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        int, pe,
        uint32_t, seq_num
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(int, pe, pe)
        lttng_ust_field_integer(uint32_t, seq_num, seq_num)
    )
)

/*
 * remote_data_arrival — FI_REMOTE_CQ_DATA received
 */
LTTNG_UST_TRACEPOINT_EVENT(
    nvshmem_libfabric,
    remote_data_arrival,
    LTTNG_UST_TP_ARGS(
        int, domain_index,
        int, imm_header
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int, domain_index, domain_index)
        lttng_ust_field_integer(int, imm_header, imm_header)
    )
)

#endif /* _NVSHMEM_LIBFABRIC_TRACE_H */

#include <lttng/tracepoint-event.h>
