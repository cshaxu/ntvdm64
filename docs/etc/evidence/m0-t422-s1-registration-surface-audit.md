# T422 S1 WOW32 registration-surface audit

## Question

Does the local `UserRegisterWowHandlers` provider preserve the complete
original 21-input/20-output ABI surface, and which output is explicitly known
to be incomplete rather than silently null?

## Procedure and result

`verify-wow32-registration-surface.ps1` reads the original
`opennt-abi/.../wowuserp.h` structures, verifies all 21 assignments in original
`wow32.c::W32Init`, and verifies all 20 provider output assignments. It also
checks that the provider preserves the whole input record and declares the
pinned `0x84000000` USER layout.

The source audit passes with 21 input and 20 output slots. The sole direct
explicit-failure output is `pfnGetFullUserHandle`, assigned to
`unsupported_full_handle` and returning `ERROR_CALL_NOT_IMPLEMENTED`. It is
not a null pointer or a success substitute.

The current-source S1 recheck also verified the six selected original owner
bodies and four guest-client layout spans against their pinned OpenNT source
identities.  This proves the selected source slices remain selected and
unchanged outside their registered boundary adaptations; it does not convert
the slot assignment into guest callback reachability.

## Disposition

The ABI shape is complete, but this is not a runtime success claim. Inputs
remain owned by their original WOW32 functions; their lifecycle owners are S2
(version/dialog/callback), S4 (resources) S5 (clipboard) and S6 (memory).
Outputs retain the per-slot S2/S4/S5 allocation in the coverage ledger.

S2 must replace the one explicit full-handle failure only by completing the
same guest desktop/WND/HANDLEENTRY lifecycle documented in the client-view
audit. It must not replace the slot with a host HWND or arbitrary pointer.
All other non-null outputs still require their owner S's actual guest,
reentry and teardown evidence; this audit deliberately proves ABI coverage,
not behavior.

## Reproduction

```powershell
& .\tests\observation\verify-wow32-registration-surface.ps1 `
  -RepositoryRoot (Get-Location).Path
```
