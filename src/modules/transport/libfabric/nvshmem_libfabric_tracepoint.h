#ifndef _NVSHMEM_LIBFABRIC_TRACEPOINT_H
#define _NVSHMEM_LIBFABRIC_TRACEPOINT_H

#ifdef NVSHMEM_LIBFABRIC_TRACE
#define NVSHMEM_TRACE_NOINLINE __attribute__((noinline))

#include "nvshmem_libfabric_trace.h"
#include <atomic>

static std::atomic<int64_t> nvshmem_trace_outstanding[32];

static inline void nvshmem_trace_inc(int di) {
    int64_t v = nvshmem_trace_outstanding[di].fetch_add(1) + 1;
    lttng_ust_tracepoint(nvshmem_libfabric, outstanding, di, v);
}
static inline void nvshmem_trace_dec(int di) {
    int64_t v = nvshmem_trace_outstanding[di].fetch_sub(1) - 1;
    lttng_ust_tracepoint(nvshmem_libfabric, outstanding, di, v);
}

/* Network posts — inc outstanding */
#define NVSHMEM_TRACE_RMA_IMPL(pe, di, sz, vd, ra) do {     lttng_ust_tracepoint(nvshmem_libfabric, rma_impl, pe, di, sz, vd, ra);     nvshmem_trace_inc(di); } while (0)

#define NVSHMEM_TRACE_GDR_SIGNAL(pe, di, seq, nw) do {     lttng_ust_tracepoint(nvshmem_libfabric, gdr_signal, pe, di, seq, nw);     nvshmem_trace_inc(di); } while (0)

#define NVSHMEM_TRACE_GDRCOPY_AMO_ACK(pe, di, seq) do {     lttng_ust_tracepoint(nvshmem_libfabric, gdrcopy_amo_ack, pe, di, seq);     nvshmem_trace_inc(di); } while (0)

#define NVSHMEM_TRACE_GDR_AMO(pe, di, op) do {     lttng_ust_tracepoint(nvshmem_libfabric, gdr_amo, pe, di, op);     nvshmem_trace_inc(di); } while (0)

/* Local AMO ops — trace only */
#define NVSHMEM_TRACE_PERFORM_GDRCOPY_AMO_START(di, op)     lttng_ust_tracepoint(nvshmem_libfabric, perform_gdrcopy_amo_start, di, op)

#define NVSHMEM_TRACE_PERFORM_GDRCOPY_AMO_END(di, st)     lttng_ust_tracepoint(nvshmem_libfabric, perform_gdrcopy_amo_end, di, st)

/* Network completions — dec outstanding */
#define NVSHMEM_TRACE_COMPLETION_SEND(di, wr) do {     lttng_ust_tracepoint(nvshmem_libfabric, completion_send, di, wr);     nvshmem_trace_dec(di); } while (0)

#define NVSHMEM_TRACE_COMPLETION_WRITE(di, wr) do {     lttng_ust_tracepoint(nvshmem_libfabric, completion_write, di, wr);     nvshmem_trace_dec(di); } while (0)

/* Other completions — trace only */
#define NVSHMEM_TRACE_COMPLETION_SIGNAL(di, addr, seq)     lttng_ust_tracepoint(nvshmem_libfabric, completion_signal, di, addr, seq)

#define NVSHMEM_TRACE_COMPLETION_AMO(di, wr)     lttng_ust_tracepoint(nvshmem_libfabric, completion_amo, di, wr)

/* Progress duration */
#define NVSHMEM_TRACE_PROGRESS_START(di, qp)     lttng_ust_tracepoint(nvshmem_libfabric, progress_start, di, qp)

#define NVSHMEM_TRACE_PROGRESS_END(di, st)     lttng_ust_tracepoint(nvshmem_libfabric, progress_end, di, st)

/* NEW: Receiver-side events */
#define NVSHMEM_TRACE_PUT_SIGNAL_ACK_COMPLETION(di, addr)     lttng_ust_tracepoint(nvshmem_libfabric, put_signal_ack_completion, di, addr)

#define NVSHMEM_TRACE_GDR_PROCESS_ACK_START(di)     lttng_ust_tracepoint(nvshmem_libfabric, gdr_process_ack_start, di)

#define NVSHMEM_TRACE_GDR_PROCESS_ACK_END(di, st)     lttng_ust_tracepoint(nvshmem_libfabric, gdr_process_ack_end, di, st)

#define NVSHMEM_TRACE_GDR_PROCESS_AMO_START(di)     lttng_ust_tracepoint(nvshmem_libfabric, gdr_process_amo_start, di)

#define NVSHMEM_TRACE_GDR_PROCESS_AMO_END(di, st)     lttng_ust_tracepoint(nvshmem_libfabric, gdr_process_amo_end, di, st)

#define NVSHMEM_TRACE_SIGNAL_DELIVERED(di, pe, seq)     lttng_ust_tracepoint(nvshmem_libfabric, signal_delivered, di, pe, seq)

#define NVSHMEM_TRACE_REMOTE_DATA_ARRIVAL(di, hdr)     lttng_ust_tracepoint(nvshmem_libfabric, remote_data_arrival, di, hdr)

#else
#define NVSHMEM_TRACE_NOINLINE

#define NVSHMEM_TRACE_RMA_IMPL(pe, di, sz, vd, ra)
#define NVSHMEM_TRACE_GDRCOPY_AMO_ACK(pe, di, seq)
#define NVSHMEM_TRACE_PERFORM_GDRCOPY_AMO_START(di, op)
#define NVSHMEM_TRACE_PERFORM_GDRCOPY_AMO_END(di, st)
#define NVSHMEM_TRACE_GDR_AMO(pe, di, op)
#define NVSHMEM_TRACE_GDR_SIGNAL(pe, di, seq, nw)
#define NVSHMEM_TRACE_PROGRESS_START(di, qp)
#define NVSHMEM_TRACE_PROGRESS_END(di, st)
#define NVSHMEM_TRACE_COMPLETION_SEND(di, wr)
#define NVSHMEM_TRACE_COMPLETION_WRITE(di, wr)
#define NVSHMEM_TRACE_COMPLETION_SIGNAL(di, addr, seq)
#define NVSHMEM_TRACE_COMPLETION_AMO(di, wr)
#define NVSHMEM_TRACE_PUT_SIGNAL_ACK_COMPLETION(di, addr)
#define NVSHMEM_TRACE_GDR_PROCESS_ACK_START(di)
#define NVSHMEM_TRACE_GDR_PROCESS_ACK_END(di, st)
#define NVSHMEM_TRACE_GDR_PROCESS_AMO_START(di)
#define NVSHMEM_TRACE_GDR_PROCESS_AMO_END(di, st)
#define NVSHMEM_TRACE_SIGNAL_DELIVERED(di, pe, seq)
#define NVSHMEM_TRACE_REMOTE_DATA_ARRIVAL(di, hdr)

#endif
#endif
