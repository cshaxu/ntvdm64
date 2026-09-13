# T408 S1 RTL recovery map and frozen baseline

## Scope and frozen baseline

This is the read-only S1 disposition for D11--D16 and D26.  It freezes
`cc4e9c165` as the pre-restoration source baseline and the selected x86 graph
at `build/governance-x86-target-check/build.ninja`.  It does not import code,
change product behaviour, or treat a source file that is absent from that
graph as a runtime provider.

The comparison source is the pinned local OpenNT tree named by the source
policy.  The six requested original owners are present there:

| Row | Original source owner | Lines | Current selected provider and direct reached consumers | S disposition |
| --- | --- | ---: | --- | --- |
| D11 | `base/ntos/rtl/environ.c` | 863 | `adapter-mvdm-host-out/win32/source/opennt_support_rtl.c`: `RtlCreateEnvironment`, `RtlDestroyEnvironment`, `RtlSetEnvironmentVariable`; reached by `base/win32/client/vdm.c`, `dos/command/cmdenv.c`, and the Base VDM environment binding | S3: retain the original environment algorithm in `opennt-host/base/ntos/rtl/environ.c`; bind only its `Zw*VirtualMemory`/PEB allocation and lock mechanics. |
| D12 | `base/ntos/rtl/error.c` and its status tables | 187 | the reduced `RtlNtStatusToDosError` in `opennt_support_rtl.c`; reached broadly by selected OpenNT/MVDM callers | S4: recover table-driven conversion and `LastStatusValue`; do not retain the four-case mapping as product behaviour. |
| D13 | `base/ntos/rtl/x86/largeint.asm:RtlExtendedLargeIntegerDivide` | 855 | C substitute in `opennt_support_rtl.c`; reached by `softpc.new/host/src/nt_timer.c` | S2: import the exact x86 owner or a source-proven buildable equivalent wrapper; preserve original zero-divisor raise path rather than returning zero. |
| D14 | `base/ntos/rtl/x86/largeint.asm:RtlExtendedIntegerMultiply` | 855 | C substitute in `opennt_support_rtl.c`; reached by `nt_timer.c` | S2: same assembly-owner cohort; verify the original low-64-bit arithmetic contract. |
| D15 | `base/ntos/rtl/x86/movemem.asm:RtlFillMemoryUlong` | 779 | C substitute in `opennt_support_rtl.c`; reached by `copy_fnc.c` and `wow32/wkmem.c`. `copy_fnc.c` itself selects its original in-source forward fill loop, except for `memset4`. | S2: restore the RTL fill owner, retain the proven original local loop, and prove byte count/tail semantics before deleting any loop. |
| D16 | `windows/core/ntuser/rtl/chartran.c:MBToWCSEx` | 605 | `wow_public_user_facade.c` and duplicate `wow32_public_api_adapters.c` exist, but neither appears in the frozen selected x86 graph | S5: first remove/reconcile inactive duplicate material; then restore the original USER conversion algorithm with a finite allocator/NLS binding. No runtime replacement claim is made in S1. |
| D26 | `ds/netapi/netlib/copystr.c:NetpCopyWStrToStr` | 417 | the byte-identical original `opennt-host/netapi/netlib/copystr.c` is selected in `original-opennt-netlib.lib`; `mvdm_redirector_guest_copy.c` is separately selected and reached by `vdmredir/vrnetapi.c` | S5: preserve the original host-buffer conversion owner; keep only a bounded guest-write publication binding. The unbounded original destination-pointer contract cannot cross the guest lease boundary directly. |

## Source and build facts

- The original `environ.c` relies on `ZwAllocateVirtualMemory`,
  `ZwQueryVirtualMemory`, `ZwFreeVirtualMemory`, the PEB lock and current PEB.
  These are a finite modern binding problem, not permission to preserve the
  adapter's independent mutable-MULTI_SZ algorithm.
- `error.c` relies on `RtlpRunTable`/`RtlpStatusTable` and records
  `LastStatusValue` in the TEB.  The current four-result function neither
  supplies the table nor that state transition.
- `largeint.asm` implements the two reached entries and invokes
  `RtlRaiseStatus` on divide-by-zero.  No MASM/llvm-ml assembler was available
  on the frozen command path; S2 must establish a reproducible x86 assembly
  build gate before replacing the C provider.
- The build selects `copy_fnc.c` and `opennt_support_rtl.c`.  Its forward fill
  uses the original in-source loop (`#if 1`); only the `memset4` call currently
  reaches `RtlFillMemoryUlong`.  Thus D15's measured local mirror delta
  (`+21/-7` in the original-owner audit) is not a blanket deletion candidate.
- `NetpCopyWStrToStr` writes into an unbounded host `LPSTR` after
  `RtlUnicodeStringToOemString`; `mvdm_redirector_copy_wide_to_guest` obtains
  an exact OEM byte count then performs one guest lease write.  The conversion
  algorithm is recoverable; the lease publication is irreducible adapter work.

## Four-rung disposition and measured footprint

| Cohort | Original body available | Directly composable today | Autonomous selected code eligible for retirement | Necessary retained binding |
| --- | --- | --- | --- | --- |
| D13--D15 | Yes | Not yet: x86 assembler gate is unproved | 33 lines in `opennt_support_rtl.c` for the three functions; D15 local loop is excluded pending contract proof | x86 object build/link only |
| D11 | Yes | Not yet: virtual-memory/PEB binding is absent | 137-line local environment implementation (lines 152--288) after original body and binding pass | `Zw*` VM, current PEB and lock boundary |
| D12 | Yes | Not yet: required table unit/TEB carrier must be selected | 1 reduced mapping function (line 382), not a valid equivalent | TEB `LastStatusValue` carrier |
| D16 | Yes | No selected consumer yet | 0 selected runtime lines; two inactive duplicate facades are retirement/reconciliation candidates | USER allocation/NLS boundary only if a consumer is selected |
| D26 | Already selected, byte-identical | Yes for host conversion | 0 conversion lines; do not delete the 84-line guest-copy file as a whole | bounded guest-memory write and capacity/terminator check |

The 33/137 counts are source ranges, not a promised net line reduction.  S2
is therefore the source-proven first implementation cohort: D13--D15 only.
It must add a reproducible x86 assembly gate, test division/multiplication/fill
edge contracts and test `nt_timer`, `copy_fnc` and `wkmem` consumers before
any later cohort opens.  S3--S5 retain the proposal's stated order.

## Independent review

Review against the frozen build confirms that the map distinguishes selected
providers from inactive duplicate files, and distinguishes original host
conversion from required guest-memory publication.  No source body is claimed
restored and no behavior is claimed accepted at S1.
