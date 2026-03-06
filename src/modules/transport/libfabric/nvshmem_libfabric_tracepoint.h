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

/* Sender posts — inc outstanding */
#define NVSHMEM_TRACE_SENDER_POST_RMA(pe, di, sz, vd, ra) do {     lttng_ust_tracepoint(nvshmem_libfabric, sender_post_rma, pe, di, sz, vd, ra);     nvshmem_trace_inc(di); } while (0)

#define NVSHMEM_TRACE_SENDER_POST_SIGNAL(pe, di, seq, nw) do {     lttng_ust_tracepoint(nvshmem_libfabric, sender_post_signal, pe, di, seq, nw);     nvshmem_trace_inc(di); } while (0)

#define NVSHMEM_TRACE_SENDER_POST_AMO(pe, di, op) do {     lttng_ust_tracepoint(nvshmem_libfabric, sender_post_amo, pe, di, op);     nvshmem_trace_inc(di); } while (0)

/* Sender local completions — dec outstanding */
#define NVSHMEM_TRACE_SENDER_COMPLETION(di, wr, type) do {     lttng_ust_tracepoint(nvshmem_libfabric, sender_completion, di, wr, type);     nvshmem_trace_dec(di); } while (0)

/* Sender remote completions (acks/responses arriving back) */
#define NVSHMEM_TRACE_SENDER_COMPLETION_ACK(di, addr, hdr)     lttng_ust_tracepoint(nvshmem_libfabric, sender_completion_ack, di, addr, hdr)

#define NVSHMEM_TRACE_SENDER_COMPLETION_AMO_RESPONSE(di, wr)     lttng_ust_tracepoint(nvshmem_libfabric, sender_completion_amo_response, di, wr)

/* Receiver remote completions (data arriving from sender) */
#define NVSHMEM_TRACE_RECEIVER_COMPLETION_REMOTE(di, addr, type)     lttng_ust_tracepoint(nvshmem_libfabric, receiver_completion_remote, di, addr, type)

/* Receiver GDRCopy duration */
#define NVSHMEM_TRACE_RECEIVER_GDRCOPY_START(di, op, is_sig)     lttng_ust_tracepoint(nvshmem_libfabric, receiver_gdrcopy_start, di, op, is_sig)

#define NVSHMEM_TRACE_RECEIVER_GDRCOPY_END(di, st)     lttng_ust_tracepoint(nvshmem_libfabric, receiver_gdrcopy_end, di, st)

/* Receiver ack/response posts — inc outstanding */
#define NVSHMEM_TRACE_RECEIVER_POST_ACK(pe, di, seq, hdr) do {     lttng_ust_tracepoint(nvshmem_libfabric, receiver_post_ack, pe, di, seq, hdr);     nvshmem_trace_inc(di); } while (0)

/* Receiver local completions — dec outstanding */
#define NVSHMEM_TRACE_RECEIVER_COMPLETION(di, wr, type) do {     lttng_ust_tracepoint(nvshmem_libfabric, receiver_completion, di, wr, type);     nvshmem_trace_dec(di); } while (0)

/* Progress duration (disabled in libfabric.cpp) */
#define NVSHMEM_TRACE_PROGRESS_START(di, qp)     lttng_ust_tracepoint(nvshmem_libfabric, progress_start, di, qp)

#define NVSHMEM_TRACE_PROGRESS_END(di, st)     lttng_ust_tracepoint(nvshmem_libfabric, progress_end, di, st)

/* Unused — defined but never called */
#define NVSHMEM_TRACE_SIGNAL_DELIVERED(di, pe, seq)     lttng_ust_tracepoint(nvshmem_libfabric, signal_delivered, di, pe, seq)

#define NVSHMEM_TRACE_GDR_PROCESS_ACK_START(di)     lttng_ust_tracepoint(nvshmem_libfabric, gdr_process_ack_start, di)

#define NVSHMEM_TRACE_GDR_PROCESS_ACK_END(di, st)     lttng_ust_tracepoint(nvshmem_libfabric, gdr_process_ack_end, di, st)

#define NVSHMEM_TRACE_GDR_PROCESS_AMO_START(di)     lttng_ust_tracepoint(nvshmem_libfabric, gdr_process_amo_start, di)

#define NVSHMEM_TRACE_GDR_PROCESS_AMO_END(di, st)     lttng_ust_tracepoint(nvshmem_libfabric, gdr_process_amo_end, di, st)

#else
#define NVSHMEM_TRACE_NOINLINE

#define NVSHMEM_TRACE_SENDER_POST_RMA(pe, di, sz, vd, ra)
#define NVSHMEM_TRACE_SENDER_POST_SIGNAL(pe, di, seq, nw)
#define NVSHMEM_TRACE_SENDER_POST_AMO(pe, di, op)
#define NVSHMEM_TRACE_SENDER_COMPLETION(di, wr, type)
#define NVSHMEM_TRACE_SENDER_COMPLETION_ACK(di, addr, hdr)
#define NVSHMEM_TRACE_SENDER_COMPLETION_AMO_RESPONSE(di, wr)
#define NVSHMEM_TRACE_RECEIVER_COMPLETION_REMOTE(di, addr, type)
#define NVSHMEM_TRACE_RECEIVER_GDRCOPY_START(di, op, is_sig)
#define NVSHMEM_TRACE_RECEIVER_GDRCOPY_END(di, st)
#define NVSHMEM_TRACE_RECEIVER_POST_ACK(pe, di, seq, hdr)
#define NVSHMEM_TRACE_RECEIVER_COMPLETION(di, wr, type)
#define NVSHMEM_TRACE_PROGRESS_START(di, qp)
#define NVSHMEM_TRACE_PROGRESS_END(di, st)
#define NVSHMEM_TRACE_SIGNAL_DELIVERED(di, pe, seq)
#define NVSHMEM_TRACE_GDR_PROCESS_ACK_START(di)
#define NVSHMEM_TRACE_GDR_PROCESS_ACK_END(di, st)
#define NVSHMEM_TRACE_GDR_PROCESS_AMO_START(di)
#define NVSHMEM_TRACE_GDR_PROCESS_AMO_END(di, st)

#endif
#endif
