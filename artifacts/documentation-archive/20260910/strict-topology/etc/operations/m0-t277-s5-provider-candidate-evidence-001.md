# M0 T277 S5 P1 — SoftPC provider-candidate evidence

This delivery records lexical provider candidates for every one of the 76
unique symbols called by the 197 `softpc.new/host/src` work items.  It is an
input to the manual source-form family decision, not a definition resolver:
C/K&R declarations, macro/function-table forms and ordinary calls can match
the same spelling.

Every candidate is therefore explicitly labeled as lexical-only.  Candidate
path classes distinguish original host-control forms, historical SoftPC base
executors that Bochs replaces, MVDM/DOS providers, debugger and monitor/product
shell candidates, DPMI forms, and declaration carriers.  S5 may assign a final
family owner only after checking the original body and its relevant caller
return path.

Reproduce with:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Export-T277S5ProviderCandidates.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T277S5ProviderCandidates.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```
