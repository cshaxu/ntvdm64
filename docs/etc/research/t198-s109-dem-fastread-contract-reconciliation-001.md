# M0 T198 S109: DEMFASTREAD Contract Reconciliation

## Result

S106's `50:42/43` no-op family copied the visible historical dispatcher stub,
but it was not the correct selected CLI composition for `50:42`.  It leaves
AX untouched; the reached event carries `AX=4E53h`.  OpenNT `handle.asm` then
uses AX as the read count and advances the SFT position.  The previously
admitted source-derived `50:42` provider instead copies declared bytes and
returns the actual count in AX.  Its removal explains why S107 diverges before
the already-proven BOP `5F` stream initialization.

## Source Contract

| Step | Original source fact | Required CLI composition meaning |
| --- | --- | --- |
| Fast branch selection | `handle.asm:564-579` checks non-pipe state and `MIPS_BIT_MASK`; clear bit selects fast. `vint.h` defines it as `0x400`, and the x86 initialization clears it. | The observed x86 profile correctly reaches `50:42`; changing that bit or forcing `50:16` is rejected. |
| BOP form | `dossvc.h:87-90` encodes `C4 C4 50 <service>`; `42h` is `SVC_DEMFASTREAD`. | Resume follows the four copied bytes through the common ingress bridge. |
| Caller inputs | `handle.asm:450-462` supplies AX:BP handle, BX:SI position, CX count, DS:DX destination and ZF from `sf_nt_seek`. | Existing source-derived provider already consumes this copied ABI and uses only the declared read-only namespace. |
| Caller continuation | `handle.asm:463-474` takes CF-clear success, adds AX to SFT position, swaps AX/CX and returns AX as actual bytes. | A successful selected provider must set AX to the actual byte count. Preserving stale AX is not a valid load result. |
| Historical table | `demdisp.c:165-166,230-238` maps 42/43 to `demNotYetImplemented`, which clears CF and changes no GPR. | This records a missing historical x86 worker. It is the default unavailable behavior, not proof that a CLI-selected readable file should report success without data. |

## Runtime Reconciliation

S104 observes the live x86 event with `AX=4E53h`, `BP=4005h`, `CX=20h`, ZF
set and CF clear.  S106 resumes it unchanged.  Consequently the unchanged
guest caller treats `4E53h` as a successful read length and mutates its file
state accordingly.  S107 then records no BOP `5F` and reaches the legacy
fullscreen INT10 target.

By contrast, T153/T154 and T198 S99 define and test the existing
profile-gated source-derived provider: it supplies a checked bulk write to
DS:DX, returns AX equal to the byte count, clears CF, and leaves FASTWRITE
unprovided.  S89/S90's BOP `5F` stream handoff is the relevant downstream
control path that the S106 change displaced.

## Disposition

S110 may restore the already-admitted provider route for `50:42` and remove
the no-op intercept.  `50:43` must remain non-captured/unavailable; S110 may
not implement host writes.  No Bochs, mantle, ROM, VGA, device or CLI
capability change is required.

## Evidence

- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/handle.asm:450-579`
- `src/opennt/base/mvdm/inc/{dossvc.h,vint.h}`
- `src/opennt/base/mvdm/dos/dem/demdisp.c:165-166,230-238`
- `docs/etc/research/t152-s1-original-fastread-execution-path-audit-001.md`
- `docs/etc/research/t153-s1-x86-fastio-provider-contract-001.md`
- `docs/etc/research/t198-s99-fastread-state-correction-001.md`
- `docs/etc/research/t198-s104-complete-fastread-native-observation-001.md`
- `docs/etc/research/t198-s107-fastio-noop-native-observation-001.md`
