# T484 S3 P121 task-close contract and Winsock producer

B0195 is only partially migrated: task-close-result now calls a CMD/native target
linking the actual selected wow_task_termination object. It no longer requires
worker/provider directory setup or compiler-environment initialization. Its
unchanged C fixture verifies original-ticket cancellation, reused guest task ID,
serial admission and no reopen on successful close. The new runner owns the
process and its30-second deadline. Other29 public lifecycle cases and their
fault variants still use the old producer and remain incomplete; no B0195
completion credit is taken. Existing callers may still supply their roots.

B0196 retains the unique native Winsock1.1 blocking-hook boundary. Remove the
external CompilerDriver and direct CL call; use the existing native fixture
owner and a CMD target. The C source and assertions are unchanged. Actual
traffic is confined to the process's IPv4 loopback pair; no external host or
guest is involved. This is not Win16 networking or actual task-retirement proof.

Owned PID19528 exits0: task-close marker and Winsock result0,30 callbacks,
foreign0/outside_blocking0/cancel0/error0; cached full host build; Winsock target
hash/mtime unchanged on rebuild; x64 rejection2;11 raw host hashes and22 published
copies unchanged. Existing dual publication verified, not republished. No
production code, original guest, mirror or installed package changed.

[Receipt](../../../artifacts/build-migration/t484/s3-close-winsock/result.json)
records owned validation, cache and markers. Raw evidence:
build/t484-s3-close-winsock. Next work remains B0195's combined lifecycle graph,
including real CCPU versus explicitly doubled storage cases; do not delete
those contracts merely to avoid their current stale object-path problem.
B0166/B0169, migrated-tool retrospective review and S4/S5 remain open.
