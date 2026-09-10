# T198 S15 MSVC x64 Generic UD2 Observation 001

## Question

Does the existing selector-blind generic `#UD` bridge immediately stop the
neutral `UD2` finite fixture under the S13 MSVC x64 core/mantle closure?

## Observation

The fresh x64 `UD2` recipe links the intended local objects:
`exception_mantle_ud.obj`, `generic_ud_bridge.obj` and the finite runner.
The link map resolves both `BX_CPU_C::exception` from the replacement
exception object and `bx_ntvdm_mantle_generic_ud_bridge_v1` from the mantle
bridge. The generated fixture records entry bytes `0f 0b`, CPU5 profile and
x64 PE32+ output; no adapter, BOP, OpenNT, MinGW or product input occurs.

The process nevertheless exits zero through
`BX_NTVDM_FINITE_RUN_COMPLETED_BUDGET`: its run log reaches
`prefetch: EIP [00010000] > CS.limit [0000ffff]`. The finite runner returns
that zero status only if its 64-tick watchdog callback fires. A bridge STOP
instead sets `kill_bochs_request`, leaves the watchdog state false and would
return `BX_NTVDM_FINITE_RUN_UNEXPECTED_LOOP_RETURN`, not zero.

Therefore this run does **not** prove a generic-UD bridge invocation. It
proves the opposite narrow fact: the present fixture did not observe its
expected immediate STOP before the watchdog. The decode table still maps
`0f 0b` to `UD2A`, whose handler calls `exception(BX_UD_EXCEPTION, 0)`; the
failure is not yet attributed to decoding, entry placement, prefetch state,
or bridge validation.

The S15 r2 retry adds the already-native `invalidate_prefetch_q()` after the
project-owned real-mode CS:RIP entry setter. The same watchdog observation
remains. The repair is retained because a direct control-transfer state change
must invalidate fetch state, but it is not credited as the UD2 root cause.

S15 r3 corrects the finite fixture's former false-positive condition. Its
private mantle test control now records an accepted generic STOP, and the
generated UD2 program accepts only `COMPLETED_UD_STOP`; watchdog completion is
an explicit failure. The fresh MSVC x64 run exits `1`, while retaining the
same prefetch-limit log. This conclusively rejects the former zero result as
proof of bridge traversal.

S15 r4 restores the native page-write-stamp decrement for every successful
chunk in the generic ordinary-RAM copier. That is required cache coherency for
any injected executable byte sequence, but r4 still exits 1 under the
stop-observed acceptance rule. Thus neither stale prefetch state nor omitted
page-write stamps is sufficient to explain the present UD2 non-observation.

S15 r5 adds a private finite-fixture read-back of the first two entry bytes
after the generic ordinary-RAM write. The run again exits `1`, not the distinct
entry-byte-mismatch status, proving that `0f 0b` is present at the requested
ordinary RAM location before CPU-loop entry. The remaining boundary is thus
the initial native fetch/decode/exception observation chain.

## Next Diagnostic Bound

The next step may add only default-off mechanical observation sufficient to
distinguish copied entry bytes, fault vector and bridge disposition. It may
not add BOP recognition, adapter/provider linkage, DOS/OpenNT behavior, or
new machine/device semantics.

## Closure

S15 r6 corrects the fixture lifecycle: disabling fixture mode no longer clears
the already-observed STOP. The fresh MSVC x64 run exits zero under the strict
stop-observed criterion, proving neutral `UD2` reaches the generic copied
`#UD` bridge and its typed STOP outcome without adapter, BOP, provider,
OpenNT, MinGW or product input.
