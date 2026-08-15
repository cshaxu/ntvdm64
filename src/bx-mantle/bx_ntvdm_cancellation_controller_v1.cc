#include "bx_ntvdm_cancellation_controller_v1.h"

#include <intrin.h>

/* The minimal Bochs configuration macroizes `inline`, so the C++ standard
 * atomic headers are not admissible here. These MSVC x64 interlocked
 * intrinsics supply the same process-local atomic state without a Win32
 * header, handle or callback. */
static volatile long bx_ntvdm_cancellation_active = 0;
static volatile long bx_ntvdm_cancellation_reason = 0;
#if defined(BX_NTVDM_CANCELLATION_TESTING)
static volatile long bx_ntvdm_cancellation_test_polls = 0;
#endif

static uint32_t bx_ntvdm_cancellation_load(volatile long *value)
{
  return (uint32_t) _InterlockedCompareExchange(value, 0, 0);
}

static void bx_ntvdm_cancellation_store(volatile long *value, uint32_t input)
{
  _InterlockedExchange(value, (long) input);
}

static int bx_ntvdm_cancellation_compare_exchange(volatile long *value,
  uint32_t expected, uint32_t replacement)
{
  return (uint32_t) _InterlockedCompareExchange(value, (long) replacement,
    (long) expected) == expected;
}

static int bx_ntvdm_cancellation_reason_valid(uint32_t reason)
{
  return reason == BX_NTVDM_CANCELLATION_V1_USER_REQUEST;
}

extern "C" int bx_ntvdm_cancellation_controller_v1_activate(void)
{
  uint32_t expected = 0u;
  bx_ntvdm_cancellation_store(&bx_ntvdm_cancellation_reason,
    BX_NTVDM_CANCELLATION_V1_NONE);
#if defined(BX_NTVDM_CANCELLATION_TESTING)
  bx_ntvdm_cancellation_store(&bx_ntvdm_cancellation_test_polls, 0u);
#endif
  return bx_ntvdm_cancellation_compare_exchange(&bx_ntvdm_cancellation_active,
    expected, 1u);
}

extern "C" void bx_ntvdm_cancellation_controller_v1_deactivate(void)
{
  bx_ntvdm_cancellation_store(&bx_ntvdm_cancellation_active, 0u);
  bx_ntvdm_cancellation_store(&bx_ntvdm_cancellation_reason,
    BX_NTVDM_CANCELLATION_V1_NONE);
}

extern "C" int bx_ntvdm_cancellation_controller_v1_request(uint32_t reason)
{
  uint32_t expected = BX_NTVDM_CANCELLATION_V1_NONE;
  if (!bx_ntvdm_cancellation_reason_valid(reason) ||
      bx_ntvdm_cancellation_load(&bx_ntvdm_cancellation_active) == 0u)
    return 0;
  if (!bx_ntvdm_cancellation_compare_exchange(&bx_ntvdm_cancellation_reason,
      expected, reason))
    return 0;
  if (bx_ntvdm_cancellation_load(&bx_ntvdm_cancellation_active) != 0u)
    return 1;
  expected = reason;
  bx_ntvdm_cancellation_compare_exchange(&bx_ntvdm_cancellation_reason,
    expected, BX_NTVDM_CANCELLATION_V1_NONE);
  return 0;
}

extern "C" uint32_t bx_ntvdm_cancellation_controller_v1_requested_reason(void)
{
  if (bx_ntvdm_cancellation_load(&bx_ntvdm_cancellation_active) == 0u)
    return BX_NTVDM_CANCELLATION_V1_NONE;
  return bx_ntvdm_cancellation_load(&bx_ntvdm_cancellation_reason);
}

#if defined(BX_NTVDM_CANCELLATION_TESTING)
extern "C" void bx_ntvdm_cancellation_controller_v1_test_poll_mark(void)
{
  _InterlockedIncrement(&bx_ntvdm_cancellation_test_polls);
}

extern "C" uint32_t bx_ntvdm_cancellation_controller_v1_test_poll_count(void)
{
  return bx_ntvdm_cancellation_load(&bx_ntvdm_cancellation_test_polls);
}
#endif
