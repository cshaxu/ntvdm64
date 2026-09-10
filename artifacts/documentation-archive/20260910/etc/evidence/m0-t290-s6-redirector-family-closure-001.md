# M0 T290 S6 — Redirector whole-family closure

## Question

Does the original `vdmredir` owner package now have a complete, source-first
local disposition for every `57:00..31` dispatch entry and its reached
COMMAND/DEM consumers, without quietly enabling a BOP route or recreating a
private NT4 network/IRQ product shell?

## Inputs

- The original 50-entry dispatch ledger and S3--S5 focused evidence.
- [The final S6 disposition matrix](../operations/m0-t290-s6-redirector-family-disposition-matrix.tsv).
- Exact selected OpenNT-4.5 `vdmredir` source for mirror review.
- Existing `adapter-mvdm-host-out/{redir,win32,softpc}` and `session` seams.

## Procedure

1. Reconciled every dispatch entry and reached brokered helper into the S6
   matrix. Each row is either an executed original body or an explicit
   source-proven unavailable result with its missing owner contract.
2. Compared each of the 25 selected `mvdm-host/vdmredir` paths to the
   selected canonical source. Twenty-four are byte-identical. `vrmslot.c` is
   the only changed file and its only retained behavioral difference is the
   existing `MVDM-HOST-DIV-021` session-identity boundary.
3. Ran the formal package runner below outside the sandbox:

   ```powershell
   powershell.exe -ExecutionPolicy Bypass -File tools\build\Invoke-T290S6RedirectorFamilyMatrix.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
   ```

## Observation

- The runner passed 11 local graphs on both x86 and x64: pointer-scope,
  host-resource identity, original lifecycle/helper, all synchronous
  named-pipe forms, original COMMAND local redirection, original mailslots,
  local NetAPI results and the disabled ingress observation.
- The pointer-scope fixture initially failed because it predated the current
  mandatory predeclared-lease contract. It now declares exactly the two
  write spans it actually consumes; no production source or provider result
  changed.
- The bounded integration observation supplies an opaque `C4 C4 57 1C`
  exception record directly to the actual `adapter-mvdm-host-in` bridge while
  no route is bound. It returns decline and leaves the copied outcome record
  unchanged on both widths. Thus no selector recognition or Redirector
  ingress has been enabled.

## Interpretation

T290 is locally source/package complete. It does **not** claim that a guest
can execute a Redirector BOP or that LAN, NetBIOS, DLC, VDD or physical IRQ
support exists. The unavailable rows retain the original providers and name
the future protocol, monitor/IRQ or bounded-marshaling prerequisite rather
than introducing a synthetic result.

The single mirror divergence is the required native-identity-to-session
surrogate boundary; its source purpose, reason, implementation and files are
already registered in `src/mvdm-host/README.md`. No other provider body or
adapter becomes a second Redirector implementation.

## Follow-up

The next ordered package is the original WOW owner group. A later product
composition task decides whether a fully closed Redirector package can be
admitted through `adapter-mvdm-host-in`; until then all `57:xx` routes remain
disabled.
