# T484 S4 P30: network profile preparation

## Question and inputs

Complete the optional Empty/WfwNet preparation reopened by P29. The retained
Win16 message test has real coverage recorded in T483 S3 batch9; absence of
a current suite caller does not make it obsolete. Inputs are the existing
candidate-final package and its pinned WFWNET.DRV. Original guest binaries,
product sources and winnt.zip are unchanged.

## Ownership and procedure

The runner invokes the shared CMD fixture builder and prepare-wow-network.cmd.
CMD requires an owned immediate O:/tmp stage and checks the WfwNet driver hash.
The small test-only native tool updates one boot key and rejects malformed
selection, missing/duplicate boot sections and embedded NUL before mutation.
It edits a private sibling and replaces the owned file only after readback.
This avoids the host SYSTEM.INI registry mapping; the original filename is
never passed to the Win32 profile API. No production configuration API is added.

## Actual observations

Evidence root: build/t484-s4-network-profile.

- PID15672: cached full host build passed. Initial contract failed because a
  cache-flush API return was incorrectly treated as the key-write result.
- PID28924: corrected native tool passed both profiles, unrelated Latin1-byte
  preservation and negative contracts. Real Empty passed USER16 two-task
  messages, native peers and normal task retirement. WfwNet stopped before
  guest execution because the CMD hash subcommand quoting was incorrect.
- PID13244: corrected CMD quoting passed real WfwNet, including both native
  peer directions and normal task retirement; network-result.json records
  exit0 at 2026-11-24T20:15:44Z.
- All11 raw product images and22 dual-publication hashes remain identical.
  Existing publication is reused, not claimed as freshly rebuilt product
  bytes. The final changed native fixture was rebuilt and tested; the later
  CMD quoting fix requires no host recompilation.

## Conclusion and limits

Preparation is now CMD/native throughout this caller. Standalone PowerShell
assertions remain allowed. Frozen census becomes346/355 disposed,9 pending;
the five overlapping supplemental chains are unchanged. This is targeted
acceptance, not a new Full run or S4/S5 closure.
