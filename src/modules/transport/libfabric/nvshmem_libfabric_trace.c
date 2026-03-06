/*
 * Copyright (c) 2026, Amazon.com, Inc. or its affiliates. All rights reserved.
 *
 * LTTng tracepoint provider compilation unit.
 * This file must be compiled exactly once per shared library.
 */

#ifdef NVSHMEM_LIBFABRIC_TRACE

#define LTTNG_UST_TRACEPOINT_CREATE_PROBES
#define LTTNG_UST_TRACEPOINT_DEFINE
#include "nvshmem_libfabric_trace.h"

#endif
