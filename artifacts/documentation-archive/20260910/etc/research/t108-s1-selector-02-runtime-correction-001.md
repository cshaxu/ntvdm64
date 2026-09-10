# T108 S1 selector-02 runtime correction 001

## Immutable observation

The single allowed 10M/30-second deferred observation is
`artifacts/analysis/t108-s1-redir-unavailable-runtime-001`. It used the
unchanged T107 binary SHA-256
`FB748E860670ECA325620D2158238E172044E870DC338874C6737197F2869AF7` and
profile SHA-256
`3606D8D0570F4D7C42B0DA181EE81A31F18D92E2377396CFE2AFF65BA771D982`.
The watchdog ended it after 30 seconds: 444933 BOP observations and four
commits.

At `bochs.log` line 429, selector `02` is observed at `073B:03FF`; line 430
records a typed accepted resume from `03FF` to `0402`. The trace then repeats
the same selector alongside `prefetch: EIP [00010000] > CS.limit [0000ffff]`
until the watchdog. Thus the common ingress route is reachable, but the trace
alone does not prove that its register result is the intended one.

## Source correction

The original source, `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c`,
defines `MS_bop_2` at line 282 as the XMS BOP: it calls `XMSDispatch` and
advances IP by one BOP-service byte. `bop.h` defines `BOP_XMS` as `52h` and
`BOP_REDIR` as `57h`; the BIOS table binds those selectors to `MS_bop_2` and
`MS_bop_7` respectively. The VdmRedir load state and
`ERROR_INVALID_FUNCTION` terminal branch belong to `MS_bop_7`, starting at
line 382, with the error result at lines 420--463.

T107 therefore made a source-identity mistake by associating selector `02`
with the VdmRedir failure branch. Its unit, build and T108 reachability
evidence prove only that the erroneous adapter route compiled and ran; they
do not prove XMS behavior or redirector support. This document corrects the
record without rewriting the immutable T107 evidence.

## Required next boundary

T109 must remove the selector-02 VdmRedir disposition and mechanically audit
the selector-to-original-dispatcher identities before any new runtime trace.
Selector `02` must remain unclaimed until its actual machine owner is
established; it may not be treated as XMS. T109 may not replace XMS or
VdmRedir with an adapter implementation.
