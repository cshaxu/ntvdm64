# T465 S4 — selected classic DDE acceptance

## Question and scope

Does the selected original classic `WM_DDE_*` bridge preserve its own
conversation, conversion, callback and alias-lifetime contract without
introducing a DDEML provider, a scheduler, a guest change or a second cleanup
owner?

The answer is yes for the finite classic transport admitted by T465.  This is
not a claim for DDEML, an arbitrary third-party DDE server, cross-worker DDE,
item-atom management or automatic reclamation of application-owned data after
peer/module loss.

## Aggregate execution evidence

All runs used the sealed `build/t464-s2/package-r5` package, immutable guest
inputs, the checked-in x86 compiler driver and the configured noninteractive
modern-PowerShell supervisor.

| Case | Fresh evidence root | Result | What it proves |
| --- | --- | --- | --- |
| Normal conversation/data | `build/t465-s3-regression-r1` | PASS | Two real Win16 tasks and a native peer complete INITIATE/atom ACK, EXECUTE/packed ACK in both directions, DATA/POKE/ADVISE `CF_TEXT`, normal pair retirement and rejected-post rollback. |
| Task loss | `build/t465-s3-taskloss-r7` | PASS | Sender task exits with an armed DATA alias; receiver survives and observes sender destruction; native `GlobalFree` observer remains `0`, proving original task cleanup does not steal application allocation ownership. |
| Exact alias/free bodies | `build/t465-s4-alias-r1` | PASS | Frozen pre-cohort bodies show exactly three asserted defects in 82 cases; current selected original bodies pass 101 cases with zero errors and zero remaining nodes.  It supplements, but does not replace, the real guest/native runs. |

The task-loss and normal conversation witnesses ran the same real message
probe with distinct feature flags; their records include exact guest phases,
native peer reports and source-input hashes.  No test uses a mock DDE provider
as a substitute for the selected original transport.

## Ownership conclusion

`wdde.c` remains the sole classic conversion/alias authority:

- `DDEInitiateList` is temporary INITIATE conversion state;
- `phDDEFirst` holds paired aliases only until the selected endpoint release;
- `wumsg.c::W32CancelDDEPost` owns rejected conversion rollback; and
- KERNEL global-free callbacks reach `W32DdeFreeHandle16` / registered
  `W32DDEFreeGlobalMem32` for normal pair retirement.

`wkman.c::W32FreeTask` deliberately is not a DDE allocation reaper.  The S3
witness makes that boundary observable: a surviving peer cannot have its
application-owned DATA allocation silently freed by sender task shutdown.

## Diff, publication and retained limits

T465 adds only checked-in probes, evidence and task records.  It changes no
production source, guest input, asset, installed component or release hash, so
there is no new package publication to claim.  The current published package
remains T464's coherent `0.0.464` identity.

Retained limits are intentional original/approval boundaries: DDEML's private
provider, peer/module automatic allocation reclamation, cross-worker DDE and
unselected malformed-peer recovery.  They are not hidden T465 implementation
debt and must be admitted separately if a future user requirement needs them.

## Result

S4 closes T465's selected classic DDE capability.  The next queue candidate
may reuse its public message/callback and explicit alias-boundary evidence but
must not reinterpret it as a generic DDE service.
