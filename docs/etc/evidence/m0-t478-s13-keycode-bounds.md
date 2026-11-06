# T478 S13 — exact keyboard scan-table bounds

Baseline: 9e733185a. Finding 28 repairs two original comparisons only; no new
key mapping, filter, recovery policy or guest code is introduced.

## Contract and repair

KeyMsgToKeyCode documents zero for an unmapped scan. Its existing checks reject
values greater than the table length, but equality is also outside the array.
Both original editions and baseline have the same bug: normal table length
133, enhanced table length 84. DIV-873 changes the two > comparisons to >= and
uses an existing blank line for its comment. No array, function signature,
condition selection, ordering or other executable statement changes.

A comparison restoring just these two operators and removing the comment
reproduces the baseline source (line endings normalized for the comparison
only; mirror file line endings remain untouched). Both array token sequences
still equal both original families. The existing nt_event key-down/key-up
callers invoke the host callbacks only for a nonzero result. The repair thus
uses the existing unmapped-key result, rather than adding a new input policy.
No real keyboard producing those exact scans or user-visible crash is claimed.

## Actual-object verification

Reuse the existing [fixture](../../../tests/mvdm-host/keycode_object_contract_fixture.c)
and [runner](../../../tests/observation/verify-softpc-object-contract.ps1):
Contract keycode, separate baseline/current directories under build/t478-s13.
Both link the actual cached nt_keycd.obj and nt_input.obj. The unchanged normal
suite passes 268322 assertions and observes 1184 recording callbacks. It covers
all WORD scan values except the two separately tested equalities, event-record
preservation, valid normal/enhanced mappings, BIOS conversion with real public
host APIs, and LED state/recording-callback behavior. The deliberately wrong
KEY_EVENT_RECORD size fails its compile-time ABI assertion at C2118.

The same executable's --boundary mode changes and restores only an adjacent
byte in its own fixture process; it does not modify any production process or
guest. Before repair both exact-bound scans return sentinel 123, giving 0/2
rejections and exit 1. After repair both return zero despite that sentinel,
giving 2/2 rejections and exit 0. Original mapping bytes are restored after
each witness. This is a controlled bound proof, not physical keyboard testing.

Coherent x86 worker/redirector/hook/WOW32 and x64 worker/frontend/monitor/
launcher/service/hook builds pass. All 16 selected Full gates pass in 402995 ms, including existing DOS/GUI input,
nesting, relaunch and frontend-loss flows. DOS4GW/NESticle are owner-excluded,
skipped rather than counted as passes. Application 0.0.478/protocol 67; package
3e2e62b9-6d16-442a-9286-e169566a75e7 is published only to worktree assets/release.
All ten files and manifest match the tested package; raw ntvdm.exe matches its
input hash. [Compact results](../../../artifacts/mvdm-audit/shard-3/keycode-s13-results.json)
retain both object receipts, exact-bound output and Full timings. Documentation
governance and diff checks pass.
The broader historical input/NLS matrix is not declared complete by this one
finding; the current S owns the demonstrated table-bound defect only.

No O:/winnt write, guest modification or remote push is authorized.
