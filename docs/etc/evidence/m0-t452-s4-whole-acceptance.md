# M0 T452 S4 — W03 whole acceptance

## Result

S4 closes W03's finite module, allocation and alias denominator without a new
product-source change. The former r062 WinOldAp observation predates the
already-owned `wkman.c` wait binding; it is not a reason to add a second
scheduler, WinOldAp registry, or process-lifetime policy.

`src/wow32-dll/include/wow_native_process.h` is forced only into selected
original `wkman.c`. It maps that definition's existing
`MsgWaitForMultipleObjects` to `wow_user_msg_wait`, which preserves the
original taskman's `xxxSleepTask` / `xxxDirectedYield` execution handoff.
Original `WK32LoadModule32` command construction, WinOldAp records,
representative association, child wait and handle close stay with their
original owners. No guest or mirror body changed in S4.

## Current evidence

| W03 contract | Actual evidence | Result |
| --- | --- | --- |
| Original `wole2.c` 32-slot aliases | `build/t452-s3/alias-lifetime-r1/exit.txt` is `0`; fixture marker is `WOW_MODULE_ALIAS_BOUNDS_AND_LIFETIME_OK`. | Bounded output, oldest-slot replacement and explicit retirement pass. A signaled thread is not mistakenly treated as an automatically invalid alias. |
| WinOldAp wait/reentry | Historical real r064/r065 in [real chains](m0-t445-s6-real-chains.md#r063r064-wait-adapter-results) exercised all x86/x64 and policy combinations after this narrow binding. | Six real Win16/native/Win16 return chains pass; unrelated USER/guest behavior is not claimed. |
| Current `wow_user_msg_wait` owner | Current production `wow32`/worker objects run through `verify-wow-task-lifecycle.ps1`, producing `WOW_FIXTURE_OK case=task-lifecycle`. | Ready, timeout, invalid-handle, nested/native and execution-owner restoration cases pass. |
| Original external allocate/free | Refreshed formal CCPU40 graph and `original-external-memory-test.exe` exit `0`, proving grow/shrink, EMS/SAS, DIB and add/remove sharing. | The graph refresh removes a stale removed-style source reference; it is cache metadata only. |

The lifecycle fixture's first two attempts used legacy PowerShell or incorrect
nested Ninja roots. Those were runner configuration errors before product
testing, not product outcomes. The succeeding run used the prescribed modern
`pwsh`, `wow32` provider root and worker root.

## Handoff

W03 creates no additional module, memory, alias, WinOldAp, scheduler or
process authority. T452 is a zero-product-diff audit completion after the
already published S2 package. W04 consumes these existing owners only when an
actual caller requires one; it owns file/OEM conversion, directory state,
environment inheritance, rename/delete rollback and non-ASCII consumers.
