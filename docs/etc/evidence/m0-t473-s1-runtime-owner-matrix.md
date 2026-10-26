# M0 T473 S1 — runtime-owner matrix

## Interpretation

`mvdm-union-node-r26` proves current source selection into product link edges;
it does not prove initialization, dispatch, failure or teardown. This matrix
therefore retains the earlier conservative classification: **no original host
package becomes “complete” solely because T473 reconfirmed its link input**.
Each selected package remains partial until its current-input runtime witnesses
cover its own lifecycle. The cited records are proof entrypoints, not a claim
that an old run can substitute for a run after a changed provider.

## Selected production owners

| Product owner | Current link surface | Existing source/lifecycle evidence entrypoint | Current classification and next dependency-ordered proof |
| --- | --- | --- | --- |
| CCPU386 | `original-ccpu386.lib` → `ntvdm.exe` | [T420 S1](m0-t420-s1-ccpu386-package-audit.md) | Partial; state/vector binding, C-VID publication and worker teardown precede instruction-family claims. |
| SoftPC host roots | `original-softpc-host-roots.lib` → `ntvdm.exe` | [T420 S32](m0-t420-s32-host-capability-closure.md) | Partial; worker entry/Console lifecycle is a foundation for every following package. |
| Support | `original-softpc-support.lib` → `ntvdm.exe` | [T420 S27](m0-t420-s27-support-capability-closure.md) | Partial; original initialization order must be witnessed with CCPU/C-VID and normal worker exit. |
| BIOS | `original-softpc-bios.lib` → `ntvdm.exe` | [T420 S26](m0-t420-s26-bios-capability-baseline.md) | Partial; reset, RTC and BIOS failure behavior remain package-local evidence. |
| Keymouse | `original-softpc-keymouse.lib` → `ntvdm.exe` | [T420 S25](m0-t420-s25-keymouse-capability-baseline.md) | Partial; controller/PIC delivery, callback and teardown must be paired with the current frontend route. |
| System | `original-softpc-system.lib` → `ntvdm.exe` | [T420 S24](m0-t420-s24-system-capability-baseline.md) | Partial; timer/PIC/idle behavior must be reconciled with the current host event boundary. |
| Disks | `original-softpc-disks.lib` → `ntvdm.exe` | [T420 S28](m0-t420-s28-disks-capability-closure.md) | Partial; original no-attached-media normal and rejection behavior are the explicit product profile. |
| Video | `original-softpc-video.lib` → `ntvdm.exe` | [T420 S23](m0-t420-s23-video-capability-baseline.md) | Partial; source layout and presentation publication need one current lifecycle witness. |
| C-VID | `original-softpc-cvidc.lib` → `ntvdm.exe` | [T420 S22](m0-t420-s22-cvidc-capability-baseline.md) | Partial; generated vector initialization, reset and CCPU/video consumers are one closure group. |
| Comms | `original-softpc-comms.lib` → `ntvdm.exe` | [T420 S29](m0-t420-s29-comms-capability-closure.md) | Partial; source owner is retained; actual serial/parallel medium is an explicit external boundary. |
| DOS/EMS | `original-softpc-dos.lib` → `ntvdm.exe` | [T420 S10](m0-t420-s10-dos-package-recovery.md) | Partial; selected LIM profile and failure semantics need a current guest path. |
| Base trace | `original-softpc-base-trace.lib` → `ntvdm.exe` | [runtime package audit](mvdm-runtime-package-completion-audit-20260916.md) | Partial; trace is not evidence of a second policy or debugger provider. |
| DEM | `original-mvdm-dem.lib` → `ntvdm.exe` | [T420 S33](m0-t420-s33-dem-capability-closure.md) | Partial; ordinary file/error/caller teardown must retain bounded guest-memory ownership. |
| COMMAND | `original-mvdm-command.lib` → `ntvdm.exe` | [T420 S14](m0-t420-s14-command-package-recovery.md) | Partial; direct, nested, redirection and native-child return routes remain current regression gates. |
| XMS | `original-mvdm-xms.lib` → `ntvdm.exe` | [T420 S35](m0-t420-s35-xms-capability-progress.md) | Partial; known immutable guest defect remains a non-pass, not an excuse to replace original XMS behavior. |
| DPMI32 | `original-mvdm-dpmi32.lib` → `ntvdm.exe` | [T420 S38](m0-t420-s38-dpmi32-capability-review.md) | Partial; CPU40 descriptor/selector and DOSX activation/return are current package gates. |
| SubAlloc | `original-mvdm-host-suballoc.lib` → `ntvdm.exe` | [T420 S36](m0-t420-s36-suballoc-capability-review.md) | Partial; allocation/reallocation/free ownership must be exercised through XMS/DPMI consumers. |
| OEMUNI | `original-mvdm-host-oemuni.lib` → `ntvdm.exe` | [T420 S37](m0-t420-s37-oemuni-capability-review.md) | Partial; non-ASCII and buffer/failure consumers remain required. |
| VDMREDIR | direct original object DLL → `VDMREDIR.dll` | [T420 S20](m0-t420-s20-vdmredir-package-recovery.md) | Partial; DLL load, guest staging, named-pipe failure and `VrTerminate*` teardown stay separate from the worker link. |
| WOW32 | direct original object DLL → `wow32.dll` | [T472 S4](m0-t472-s4-wow-hard-error-reconciliation.md) | Partial; original WOW dispatch plus task/lifecycle/provider load must be revalidated as a complete package. |

## Non-product host boundaries

| Owner | Current role | Required decision |
| --- | --- | --- |
| `dbg/dbg.c` | Debugger fixture source only; it has no production product-link entry. | [T420 S43](m0-t420-s43-debugger-capability-ledger.md) is the source/caller evidence. A future S must either admit a complete original debugger package or preserve fixture-only status explicitly. |
| `vdd` | Bindings/samples and device boundaries, not one selected complete VDD package. | Keep each device behind its concrete external-medium admission; no sample becomes a product provider by compilation. |
| `dpmi`, `wow16` guest packages | Immutable guest/load activation inputs. | Do not link them into host code; acceptance must observe their actual activation from the selected worker. |

## Finite follow-through

S2 has no blanket source import: the path ledger found no unclassified missing
host source. Its only source-shaped work is to reconcile the selected
CPU40/adapter deltas against their documented original owner and either remove
an actual duplicate or retain one bounded carrier. S3 then begins with the
host/CCPU/system/C-VID dependency foundation, followed by DOS/DEM/COMMAND,
memory/DPMI, redirector, and WOW32; it must admit one complete owner package
per S packet. This order preserves the current product profile and makes each
runtime claim independently falsifiable.
