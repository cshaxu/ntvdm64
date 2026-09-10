# M0 T276 S25: `softpc.new` tool and product-entry source disposition

## Decision

The twelve original debugger/disassembler source bodies are **tool-only**.
They are not embedded in the final host runtime. The two `obj.vdm` source
bodies are historical standalone NTVDM product-entry scaffolding and are
**not-host-runtime** because `app` owns the final executable composition.

This decision deliberately does not classify the remaining `base/support` or
`host/src` machine/control sources. They require their existing source-span
owner evidence and cannot be inferred from their old directory names.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T276S25SoftpcToolSourceFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S25SoftpcToolSourceFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```
