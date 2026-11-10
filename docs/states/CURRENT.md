# Project Status

## Current Work

**No active S packet. T481 S13 is closed; S14 is next for admission.**
S12 was closed by owner decision after the reported close dialog did not recur
in bounded probes; its root cause remains unlocated. See
[S12 evidence](../etc/operations/m0-t481-multitask-completion-plan.md#s12-preliminary-source-observations).

## S13 closure

S13 completed its setup selection, four actual Win3.1 mode launches and full
coherent package delivery. Original Setup was not rerun interactively: its
unchanged command and failure-exit contract were checked with a test shim.
This is a stated verification boundary, not evidence of a fresh installation.
See [S13 evidence](../etc/evidence/m0-t481-s13-win31-setup.md). Remaining
integrated Win95 acceptance belongs to S14.

S14 enhances `tools/win95-launch` with the same DOSONLY=0 / DOSONLY=1
selection and matching launch configuration. For DOSONLY=0, install the
released `insignia.386` and `ntvdm64.386` in the installed Win95 `PATCH`
directory and configure the existing Win95 installation for multitask
payloads. Validate both choices, including launch, guest DOS execution and
the intended host-payload behavior, while preserving original guest binaries.

Known baseline: the Win95 dual-editor test uses guest USER synthetic mouse
events and therefore does not prove the owner's physical mouse route. The
published O:/WINNT/edit.exe and C:/Windows/System32/edit.exe have identical
SHA-256 `5F54A0FA8CBF5423AEF8BE8502173A59183CF3ADBC913212B1066EF8FFF3D9C0`.
An independent real hidden-console probe observed Edit input mode 0x288:
native MOUSE_EVENT records were ignored while the selected NTVWM VT mouse
conversion opened its File menu. This does not establish end-to-end delivery
through Win95, focus, geometry or the physical RDP route.

[S10 evidence](../etc/evidence/m0-t481-s10-edit-mouse.md) records the input-chain investigation.

S13 disposable workspace: `build/t481-s13`, owner S13; budget 8 hours,
20 GiB, 30-minute no-progress review. Stop and join owned processes, record
results, and clean task-specific O:/tmp copies before retirement. The owner
diagnostic package remains available for the close investigation.

## Current Technical Baseline

The published 11-image host package and installed W31 addon pair are
hash-verified. Win95 WIN.CMD points to the formal package. Original guest
binaries remain immutable. See [S9 evidence](../etc/evidence/m0-t481-s9-softpc.md)
for the 16 Full gates, skips, Win95 observer exit boundary and historical
mirror-guard test limitation.

## Recent M0 Closures

- [T480](../history/m0-t480-closure.md): adjacent audit closure.
- [T479](../history/m0-t479-closure.md): guest multitask transferred to T481.

## Recent Governance

See the [execution rules](../rules/EXECUTION.md) for the current governance.
