# T484 S3 P149 historical audit disposition

## Scope and evidence

B0234 is a T362 conventional-device recovery source snapshot, tied to its
historical build/M0-T362/S2/x86 graph and exact source spelling. B0236 is a
T420 S23 temporary graphics-absence assertion: its own contract says to keep
it only until KVM graphics presentation arrives. Current NTCON window_controller.c
and frontend_session.c own that presentation. Both tools have no callers in
tools or test. Remove them rather than translating historical assertions into
new product requirements. Retain their historical investigation records.
This removal does not claim a new serial/printer or graphics runtime test.

B0235 remains a useful standalone read-only C-VID contract audit. It invokes
no compiler or generator. Its metadata, source-owner and optional link-map
checks still apply, so no implementation change or migration is needed.

## Verification

Owned background PID28472 passed the source contract:154 public and55 private
vector slots. PID9048 repeated it with the current native worker ntvdm.exe.map
and passed actual GDP, original VGA setup and effective-address provider
checks. Neither is a Full or runtime test.

[Receipts](../../../artifacts/build-migration/t484/s3-historical-audits/result.json).
No product source, build graph, guest or publication changed; no rebuild or
republication is claimed. Three dispositions complete:265 of355,90 remaining.
S3 continues at B0237; unresolved earlier test cases remain open.
