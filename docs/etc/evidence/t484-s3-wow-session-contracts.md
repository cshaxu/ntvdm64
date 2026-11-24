# T484 S3 P197: native session contracts and retired reply research

## Disposition

Keep the session binding fixture: it exercises actual current session/runtime
sources with test-owned lifecycle operations, including rejected retirement,
missing operations, TLS/counter preservation and retriable disposal. The inner
runtime fixture separately verifies independent linkage and cross-thread
callout/reacquisition. Existing large lifecycle tests do not replace these
specific failure contracts. Both C bodies remain unchanged. Move their source
and compiler profiles to the existing CMD/CMake WOW host-contract module;
remove the PowerShell compiler command composition. No copied product code.

Remove verify-wow-reply-contract.ps1 and its sole wow_reply_contract_fixture.c.
This was a completed pinned external-original source-order study, documented
in m0-t422-s2-user-client-lifecycle-ledger.md E93. It compiles no current product
implementation and substitutes spies for all scheduling/callback operations.
There is no current caller outside that wrapper. Preserve the historical
12-case evidence and Git history, not a new compiler producer for a sealed
investigation. Existing actual Win16 message and early-reply payload tests
remain unchanged. Their coverage is not claimed equivalent to every historical
source-spy case, nor newly executed by this batch.

## Verification

Owned background26292 runs build/t484-s3-wow-session-contracts/run.ps1.
Both actual compiled contracts pass: WOW_USER_SESSION_BINDING errors=0 child=1
lifecycle=1 and WOW_USER_RUNTIME errors=0 peer=1 callout_reacquire=1.
The run also requires cached full host build and all11 raw/22 published hashes.
Final result and governance are checked before the containing commit/push.
No production, mirror or original guest change; unchanged package hashes mean
no redundant publication overwrite. No new Full or guest acceptance claimed.

## Tracking

Ten of13 supplemental compiler candidates are disposed; three remain:
Win1.01 visual helper, OEM guest and DPMI fatal. Frozen census316/355 and21
supplemental preparation/staging chains are unchanged. S3/S4/S5 remain open.
