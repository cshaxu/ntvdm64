# Package-recovery program proposals

This is the detailed proposal set for the current package-first program. Each
`P` section is a package proposal, not a serial audit gate. The active
`M0 T303` Bochs/SoftPC recovery is represented here for program continuity but
is not a Queue candidate; its live S scope belongs only to Status. Every
admitted packet still requires its own bounded S brief in Status.

## 2026-08-27 owner-directed recovery order

The former standalone candidates for whole-program one/two-degree expansion,
source-absent classification, perimeter reconciliation, generic support/ABI,
generic Win32, generic OpenNT-host and generic Ninja closure are no longer
queue blockers. They remain shared ledgers and per-package S1 admission work.
An owner package may inspect only its immediate original callers, files,
declarations and external boundary; it updates those common ledgers, then
recovers its original source body and tests its local contract. It may not use
the absence of an unrelated global classification to author a substitute.

This avoids an audit-only critical path while retaining the existing physical
identity, provenance, source-first and mirror/overlay rules. The completed P01
source-index task seeds package admissions but does not need to grow into an
unbounded whole-program BFS before functional recovery starts. Each recovery
package performs only its bounded immediate-boundary S1 review, then recovers
and locally verifies the selected original owner slice.
The decision record is [the owner-directed package-first replan]
(../evidence/m0-t301-owner-directed-package-first-queue-replan-001.md).

The superseded standalone gate candidates remain retained evidence, including
their ledgers, scripts and already-complete results.  They are not deleted or
declared unnecessary: their checks become the immediate S1 admission evidence
of the first owner package that reaches the relevant interface.

## P01 — MVDM first-degree physical interface audit (completed predecessor)

Historical source-index evidence. It emitted physical declaration/signature
binding seeds but selected no package/provider route. It is closed and is not
a candidate or implementation gate.

## P02 — Base VDM command protocol (completed M0 T302)

**Predecessor:** P01. **S1 admission:** audit only the immediate BaseClient,
BaseSrv VDM slice and its private/public boundary. **Delivery:** recover the
source-shaped `VDMINFO` request, copied capture, sizing/retry, wait/wake and
completion protocol from `opennt-host`; migrate the app one-command seam to a
bounded backend rather than a parallel protocol owner. **Exit:** local
one-session protocol tests plus explicit deferred CSRSS/multi-process/WOW
broker paths; no CSRSS reconstruction. The admitted local source-shaped
client/server slice is complete; retained CSRSS product-shell branches are
separate future owners.

## P03 — Bochs/SoftPC mechanical contract closure (active M0 T303; not a Queue candidate)

**Predecessor:** P02. **S1 admission:** inspect only reached original SoftPC/
CCPU forms used by the first vertical slice. **Delivery:** make
`adapter-bochs` and the SoftPC family provide bounded original-shaped mechanics
and mapping leases while Bochs remains the sole executor. **Exit:** x86/x64
contract tests and an explicit unsupported-machine ledger.

## P04 — MVDM ingress and fixed-width event bridge

**Predecessor:** P03. **S1 admission:** inspect only reached original machine
event forms. **Delivery:** complete selector-blind copied machine
event ingress to original MVDM dispatch. **Exit:** typed frame/stop matrix
without BOP semantics in Bochs.

## P05 — DEM/COMMAND vertical slice and child lifecycle

**Predecessor:** P02–P04. **S1 admission:** inspect only DEM/COMMAND's reached
Base VDM, machine and local child boundaries. **Delivery:** compose the original DEM/COMMAND
mirrors with the Base VDM protocol, closed local child lifecycle and machine
interfaces. **Exit:** local package matrix and explicit unavailable historical
product-shell branches; no new app-owned command semantics.

## P06 — Minimal DOS guest execution and controlled return

**Predecessor:** P05. **S1 admission:** verify only reached guest load and
return contracts. **Delivery:** load and run one declared DOS program
through the completed vertical slice. **Exit:** controlled result-return
evidence; a trace verifies this package and does not select repairs.

## P07 — Minimal vertical-slice integration closure

**Predecessor:** P06. **Delivery:** x86/x64 positive/negative matrix and
reversion sweep. **Exit:** no unowned bypass in the completed first slice.

## P08 — XMS owner package

**Predecessor:** P03, P04, P07. **S1 admission:** inspect only reached XMS
mapping, A20, UMB and INT15 boundaries. **Delivery:** original XMS allocator, A20,
UMB and INT15 semantics through mapping and machine contracts. **Exit:**
package tests and explicit hardware prerequisites.

## P09 — DPMI/DPMI32 owner package

**Predecessor:** P03, P04, P08. **S1 admission:** inspect only reached
protected-context, monitor and mapping boundaries. **Delivery:** original protected-context,
LDT, exception and memory paths. **Exit:** monitor and x86/x64 package matrix.

## P10 — Redirector owner package

**Predecessor:** P02–P05. **S1 admission:** inspect only reached local file,
pipe and host capability boundaries. **Delivery:** original VDMREDIR local file and pipe
paths, with explicit network/device unavailable paths. **Exit:** owner-family
regression without NetAPI/CSRSS shell reconstruction.

## P11 — WOW32/Win16 owner package

**Predecessor:** P02–P04, P09. **S1 admission:** inspect only reached callback,
task, monitor and host-UI boundaries. **Delivery:** original callback, task and
command contracts through bounded monitor and host-UI boundaries. **Exit:**
source package matrix; no speculative WOW broker.

## P12 — VDD owner package

**Predecessor:** P03–P04. **S1 admission:** inspect only reached VDD service,
machine and public-host boundaries. **Delivery:** original VDD service/interface paths
where public capability exists; preserve unavailable kernel branches. **Exit:**
family closure ledger and tests.

## P13 — Debugger/BDE/FAX owner packages

**Predecessor:** P02–P04, P12. **S1 admission:** inspect only reached debug/UI
and machine boundaries. **Delivery:** package-specific debug/UI
boundaries and explicit unsupported product-shell paths. **Exit:** no generic
adapter or unregistered mirror difference.

## P14 — Service-package cross-family closure

**Predecessor:** P08–P13. **Delivery:** reconcile shared interfaces, mappings,
completion/lifecycle and package regressions. **Exit:** one owner per edge and
native integration evidence for each completed family.

## P15 — Broker process and cross-process coordination

**Predecessor:** P02, P11, P14. **Delivery:** public-IPC restoration of needed
BaseSrv-style coordination. **Exit:** registration, notification, lease and
cleanup tests; no CSRSS recreation.

## P16 — In-process multi-session reentrancy audit

**Predecessor:** P14–P15. **Delivery:** classify each reached MVDM global and
worker state before any multi-context enablement. **Exit:** explicit supported
profile or evidence-backed deferral.

## P17 — Host capability expansion

**Predecessor:** P14. **Delivery:** package-owned drive, filesystem, console,
registry and device capabilities through public Win32. **Exit:** source-shaped
semantics and permission/failure matrix.

## P18 — Guest DOS/WOW16 artifact matrix

**Predecessor:** P07, P14. **Delivery:** validate source-built and original
guest images as load-only inputs. **Exit:** artifact classification and
reproducible manifest.

## P19 — Product release closure

**Predecessor:** P15–P18. **Delivery:** x86/x64 package, integration,
stability and release manifests. **Exit:** unpack-and-run product evidence and
remaining explicit exclusions.
