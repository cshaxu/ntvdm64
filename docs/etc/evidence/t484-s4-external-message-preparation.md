# T484 S4 external message preparation

## Ownership and scope

P29 reuses prepare-wow-diagnostic.cmd for the default release package instead
of PowerShell ZIP extraction and image copying. Its existing verified release
copy path now accepts either two arguments (release only) or the existing
paired worker/WOW build roots with optional diagnostic override. An incomplete
symbol pair is rejected before mutation. Existing symbol/hash checks and
diagnostic restrictions remain; no alternate extraction/copy implementation.

The external runner retains approved ZIP and release-manifest assertions and
verifies staged hashes. Caller-supplied JSON/CSV package validation is unchanged.
The win16-message child now prepares source aliases through the shared CMD
owner and uses TestFiles for the whole physical-stage input set, including
external peers and cross-worker coordinators. Scope ownership, machine checks,
original guest probes and all runtime result assertions are preserved.

## Completion correction

The child was incorrectly classified complete after its S3 producer migration.
Its optional NetworkDriver Empty/WfwNet branch still modifies system.ini with
PowerShell. Reopen that entry rather than silently retain a false completion.
Its behavior is justified by the real network-profile witnesses in
[the prior consolidation review](../../../artifacts/mvdm-audit/t483-s3/batch9-review.json),
not obsolete merely because no current suite passes that optional argument.
The configuration writer remains S4 work. This delivery completes the outer
runner's default preparation and shared input copying, not that optional path.

## Verification

Owned process25228 runs build/t484-s4-external/run.ps1 from
2026-11-24T19:59:51Z to20:06:44Z, exit0. Cached full product build passes.
Shared preparation contracts verify exact11 release images with and without
symbols, unchanged MAP copies, existing-output rejection, unsupported diagnostic,
malformed manifest and image-hash failure. A separate incomplete-symbol-pair
case returns2 without creating output.

All three real message groups pass with the default CMD-prepared package:
ordinary local/external x86/external x64 (reported9 cases), cross-worker
(reported6), and pending-send (reported6). Existing x86/x64 native destruction
controls pass. The pending-send nonblocking negative exits1 and reports the
required forbidden callback ordering; it is not mistaken for a passing product
exit. Guest normal-completion, protocol, payload and lifecycle assertions remain.
These are private-desktop runs using the existing native Userenv child setup.

All11 raw host images and22 dual-published hashes remain unchanged; reuse
that verified publication. Original guest/media and assets/winnt.zip are
untouched. No new Full or network-profile runtime result is claimed. Governance,
parser and diff checks gate the commit.

## Progress

One outer preparation entry closes and one falsely completed child reopens.
Frozen census therefore remains345/355 disposed,10 pending; supplemental5.
S4 and final clean/interpreter-free S5 acceptance remain open.
