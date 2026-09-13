# T408 S2 x86 RTL arithmetic and fill restoration

## Decision

S2 retires the 33-line C provider for D13--D15 from
`adapter-mvdm-host-out/win32/source/opennt_support_rtl.c`.  The selected
worker now links the two original-source x86 routines from
`opennt-host/base/ntos/rtl/x86` through a dedicated MSVC `ml.exe` rule and
`original-opennt-rtl-x86.lib`.

The source files are mechanical selected bodies of OpenNT's
`largeint.asm` and `movemem.asm`: only the two reached large-integer entries
and `RtlFillMemoryUlong` are exported.  This is not a new C reimplementation.
The build slice replaces OpenNT's general include/calling-convention macro
layer with direct, identical x86 stdcall symbol declarations; the arithmetic,
register/stack layout and fill loop are unchanged.

## Retired and retained material

| Item | Result |
| --- | --- |
| D13 `RtlExtendedLargeIntegerDivide` | Original x86 shift/subtract division restored, including `RtlRaiseStatus(STATUS_INTEGER_DIVIDE_BY_ZERO)`. The prior signed C division returned zero on divisor zero. |
| D14 `RtlExtendedIntegerMultiply` | Original signed multiply body restored. |
| D15 `RtlFillMemoryUlong` | Original aligned `rep stosd` body restored; low two length bits are ignored as documented by OpenNT. |
| `copy_fnc.c` forward fill | Retained: S1 proved the selected branch is the original in-source loop. It is not duplicate autonomous code. |
| Modern binding | `ml.exe` invocation and the existing `ntdll` import for `RtlRaiseStatus` are the only new build boundary. |

## Verification

| Check | Result |
| --- | --- |
| Fresh x86 graph | `build/M0-T408/S2/r001-rtl-x86-source`, generated with Node `v22.22.1`; includes `ASM-X86` objects and `original-opennt-rtl-x86.lib`. |
| Worker link | `original-softpc-process.exe` linked successfully after all 440 graph steps. |
| Focused fixture | `rtl-x86-fixture.exe` passed: ordinary 64/32 quotient/remainder, high-bit unsigned dividend (`0x8000000000000000 / 3`), signed negative multiplication, four-byte fill with a six-byte request leaving its two-byte tail unchanged, and zero-divisor `STATUS_INTEGER_DIVIDE_BY_ZERO`. |
| Ordinary CLI regression | Staged `O:\ntvdm64\ntvdm32.exe O:\ntvdm64\MEM.EXE` exited `0`; logs are `O:\ntvdm64\logs\m0-t408-s2-r001-mem.{stdout,stderr}.log`. |
| Published image | `O:\ntvdm64\ntvdm32.exe`, 3,235,328 bytes, SHA-256 `7cf5b4364d06b86675805c7c74e267d8c3c28e21f7574e45de855c5c772049e4`. |

## Review and receiver

Source review confirms the high-bit division, divide-zero and short-fill
contracts intentionally differ from the removed C bodies.  Link review
confirms that the worker reaches `original-opennt-rtl-x86.lib`, rather than
the removed adapter definitions.  S2 is complete; D11 environment recovery
is the next isolated owner cohort (S3).  D12 and D16/D26 remain unchanged.
