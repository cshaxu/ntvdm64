# M0 T276 S25: `wow32` carrier final integration disposition

## Decision

All 105 selected WOW32 declaration carriers are **binding-only**. They retain
the original public/private shape for later same-shaped `adapter-wow` and
`adapter-win32` binding, but do not authorize pointer or HANDLE identity casts.

Eight condition-unselected source files and eleven build/resource inputs are
**not-host-runtime**. They remain byte-exact provenance evidence. The remaining
77 active original source bodies are deliberately left for the eight-subdomain
provider decision, not inferred from these carrier classifications.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T276S25Wow32CarrierFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S25Wow32CarrierFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```
