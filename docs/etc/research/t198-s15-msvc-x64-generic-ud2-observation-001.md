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

## Next Diagnostic Bound

The next step may add only default-off mechanical observation sufficient to
distinguish copied entry bytes, fault vector and bridge disposition. It may
not add BOP recognition, adapter/provider linkage, DOS/OpenNT behavior, or
new machine/device semantics.
