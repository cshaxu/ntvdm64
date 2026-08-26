# M0 T276 S25: `softpc.new/host/src` control source disposition

## Decision

The final source-level outcome for the 38 remaining SoftPC control bodies is:

- 25 original monitor/BOP/session/host-capability providers are
  **adapter-backed**. Their algorithm, ordering and failure routes remain in
  the mirror; adapters provide only same-shaped machine, public Win32, BOP,
  VDD and session boundaries.
- Six useful control algorithms are **overlay-required** because their source
  body combines original logic with direct CPU/RAM/page/section transport.
  The overlay replaces only that transport with checked mapping-manager and
  typed `adapter-bochs` operations.
- Seven alternate CPU, profile, virtual-FDC or debugger/product-shell bodies
  are **not-host-runtime** to preserve the single Bochs executor rule.

This is a final expected-integration decision, not enablement. No original
source body becomes part of the active build merely by receiving a disposition.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T276S25SoftpcControlSourceFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S25SoftpcControlSourceFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```
