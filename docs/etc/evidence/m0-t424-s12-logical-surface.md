# T424 S12 unified logical text surface

## Admission and baseline

Owner authorizes sequential remaining stages after S11. S11 7ff3c670d is
pushed and clean; all eight O:/winnt hashes match its published manifest.
This ledger records S12 delivery, not owner acceptance of T424. Outputs belong below
build/M0-T424/S12/r001; valid dependency cache S2/r001 is retained.

## Provenance and route decisions

| Mechanism | Source/current owner | Decision |
| --- | --- | --- |
| DOS execution, VGA selection, block/resume and completion | Original MVDM/OpenNT mirrors; NTVDM boundary client | Retain original owners, no mirror edit. Original calcScreenParams thresholds and ResizeScreenBuffer cell-grid semantics remain mandatory. |
| DOS private API grid and projection | Project-added NTCON native_console_frontend.c | Generalize to logical_surface shared by both worker kinds. |
| Native captured full-buffer tiles plus viewport text frame | Project-added NTVWM presentation.c and NTCON channel/dispatch | Retain worker capture/Unicode/font mapping; remove canonical-output bypass. Add explicit staged publication/commit so partial tiles never replace committed state. |
| Seed history matching and row bias | Project-added NTVWM presentation.c | Remove heuristic. Seed exact current logical buffer/viewport/cursor; no old-page restoration or invented history. |
| Frame assembly, ordered authenticated pipe and snapshot barriers | Existing project-added common/console and NTCON console_video/channel | Reuse complete-payload validation and ownership barriers. Storage/render stays frontend-private; transport remains common. |
| Hidden execution Console, target creation/wait | NTVWM | Retain; frontend owns no targets or execution Console. |

Recovery ladder: original worker algorithms already compose and stay in place.
Original OpenNT has no independent dual-worker frontend logical-storage owner;
the owner explicitly admits this project presentation adaptation. Reuse current
private grid and imported row-resize primitive, not a new terminal engine,
shared-library intrusion or mirror replacement. Original source files cited
for contract comparison are source evidence, not new imported build inputs.

## Implementation and verification checklist

- [x] One complete logical grid and upper-left projection, host-independent storage.
- [x] Complete atomic native publication; partial/failure/stale/disconnect preserve committed state.
- [x] Symmetric current-state final commit/seed/apply acknowledgement before resume/output.
- [x] Nonzero viewport, clipping/margins/cursor/mouse and size-conversion tests.
- [x] Retained focused, lifecycle/RPC/version, Console17/Window17, EDIT and WOW gates.
- [x] Recoverable coherent eight-file publication and reviewed delivery; S13 then T owner gate.

No runtime pass or publication is claimed at admission.

## Implementation and failure contracts

NTCON native_console_frontend.c now owns logical_surface for both kinds.
Channel output duplicates refer to it, never the visible canonical output.
Original DOS Console operations mutate the grid; complete VGA text publications
also commit to it, refreshing the channel duplicate after storage replacement.
Native capture uses protocol-22 BEGIN/END/ABORT: stage a local grid and frame,
validate geometry/cursor/completeness, then replace grid and borrowed renderer
snapshot under the existing root I/O lock. Unfinished/invalid/aborted/EOF input
keeps the previous committed snapshot and preserves the anti-replay serial.
After a successful local commit, a projection failure is a terminal-channel
fault, not permission to roll back into disposed storage or acknowledge resume.
The prior renderer bytes stay owned until replacement/forget succeeds.

The full logical buffer, attributes, viewport origin/extent, buffer-relative
cursor/shape and worker font/palette/style remain separate from host geometry.
Console projection uses the actual canvas intersection at its upper-left,
blanks right/bottom characters and attributes, hides a clipped cursor and
rejects mouse input in blank margins. Physical resize records are frontend
events, not worker execution-Console resize requests. Host resize attempts may
grow a representable canvas but never force shrink: embedded Terminal may retain
outer rows after a successful API call. Window remains common text rendering,
not a native bitmap fallback. Unchanged cursor/shape calls are skipped so the
retained 30ms native capture does not continually restart host cursor blink.

The old native canonical-output path, projected_viewport/native_geometry_pending
state, seeded flag, seed_history_row search and capacity/row-bias heuristics
are removed. NTVWM applies the exact current logical seed to its real hidden
Console, with existing read-back acknowledgment. The original old-owner final
paint/release and incoming apply-before-resume barriers remain in production.
Only actual return to the outer shell imports its legitimately changed canonical
page at reacquisition; an internal worker handoff never seeds from that page.

DOS viewport normalization copies the current viewport and transforms the
cursor before resizing through the existing OpenNT console_grid primitive.
Its original VGA height selection and cell-row retention remain unchanged.
No initial CMD page, paragraph reflow, bottom alignment or row compensation is
introduced. No changes touch MVDM/OpenNT mirrors, opennt-abi or imported libraries.

## Attempts and verified checkpoints

Initial private fixture and script invocations are retained as failed attempts,
not passes. Obsolete native-origin fixtures were changed to target the actual
logical seed; console_video retirement assertions were corrected to retain
the existing anti-replay serial while disposing pixels. Expected process exit
codes, transport/ownership negatives and handle assertions were not relaxed.

An actual EDIT regression exposed a stale duplicate after complete VGA grid
replacement; the production channel now refreshes that duplicate. An actual
outer-CMD relaunch regression preserved VER at step 4 but lost its cells on DOS
reacquisition at step 5. The normalization's raw temporary resize was replaced
with the existing cell-grid primitive at both reached calls. r006 proves the
unchanged relaunch assertion, including VER cells surviving second DOS startup
and outer cooked CMD exit 19. These failures and fixes remain in build logs.

Final focused capture is 415/0, input return 677/0, font/text handoff 32/0.
Production channel fixtures cover complete/partial/abort, EOF, stale ownership,
offset viewport, cursor and four canvas intersections, cancellation, snapshot
lock and exact handle equality. An additional concurrent reader checks that
another thread sees the old complete grid between staged tile operations.
The normalization fixture preserves the old cell and transformed cursor.
Runtime matrices, retained integration and release results are recorded below
only after their final runs complete.

## Final verification and release

All paths below are relative to build/M0-T424/S12/r001 unless cache is stated.
The final production revision includes the DOS frame dispatch/import/handle
refresh under one root lock; channel-r008.txt and channel-thresholds-final.txt
exercise that exact code. Ten VGA threshold cases prove 22/25/28/43/50 mapping,
cursor-containing tail retention and blank growth. Canvas cases cover 100/60
columns and 30/20 rows, clipping without storage loss, blank cells/attributes,
cursor reappearance and margin-input rejection. ConPTY normalization is also
exercised by the real relaunch and strict DIR probes, not just mocked resize.

Commands used (PowerShell is pwsh, x86 environment from VsDevCmd):

```text
cmd /c build\M0-T424\S2\r001\run-ninja-parallel.cmd
pwsh -NoProfile -File build/M0-T424/S12/r001/run-affected-final.ps1
pwsh -NoProfile -File build/M0-T424/S12/r001/run-private-focused.ps1
pwsh -NoProfile -File build/M0-T424/S12/r001/run-matrices.ps1
pwsh -NoProfile -File build/M0-T424/S12/r001/run-service-gates.ps1
pwsh -NoProfile -File build/M0-T424/S12/r001/run-versions-wow.ps1
pwsh -NoProfile -File build/M0-T424/S12/r001/run-final-gates.ps1
pwsh -NoProfile -File build/M0-T424/S12/r001/run-isolation-repeat.ps1
cmd /c build\M0-T424\S12\r001\build-terminal-observer.cmd
pwsh -NoProfile -File build/M0-T424/S12/r001/run-strict-dir.ps1
pwsh -NoProfile -File build/M0-T424/S12/r001/freeze-publish.ps1
pwsh -NoProfile -File build/M0-T424/S12/r001/freeze-publish.ps1 -Publish
pwsh -NoProfile -File build/M0-T424/S12/r001/postpublication-smoke.ps1
```

- Final-source Console17 + Window17: all 34 pass (r008 matrix reports).
  These include direct/interactive/nested COMMAND, MEM, EDIT and native return.
- Actual modern EDIT return and relaunch: edit-return-r009 and
  closure-relaunch-r009 pass; original cell, cooked-input and exit-19 assertions
  retained. Strict 80x30 ConPTY DIR: final=0, entered-dos=1, dirty-prompt=0.
- Independent native sessions: r009 plus three consecutive r010/r011/r012
  runs pass selected-worker close and unrelated session exit 23. Both worker
  and frontend death cases (four r009 retirement reports) pass.
- Retained RPC (11 cases), GUI routing (five scenarios including default
  startup, explicit wait=37 and released-worker GUI survival), and all five
  application/protocol/RPC mismatch negatives pass. Regenerated MIDL uses
  interface34; APP 0.0.424 protocol34 and I/O22 are coherent.
- WINMINE reaches its visible main window; SOL/WRITE preserve their existing
  out-of-memory dialog frontier. These are not full SOL/WRITE usability passes.
  Final subsequent changes affect only frontend DOS text locking/tests;
  original worker and WOW source/binary stayed unchanged.
- No focused handle leaks, import/provider drift or library modification:
  44 pinned library inputs plus license remain identical. Frontend link
  ownership check passes; original mirrors/opennt-abi have no S12 diff.

One real r008 isolation run failed the unchanged management-RPC success
assertion. At that time the fixture did not print the error code, and its
cleanup left no live failed snapshot; the cause is not proved. Added failure
diagnostics print the actual RPC error and worker/target state without retry,
timeout change or weakened assertion. Four subsequent same-production-source
runs pass, but this is not a causal repair. Preserve
closure-two-sessions-r008.txt.management.err/log and the passing r009-r012
reports; investigate on recurrence. Initial strict-probe failures were missing
executable/include/C11 build configuration, not claimed runtime passes.

Eight x86 products published to O:/winnt after source freeze (246 inputs).
The prior accepted S11 package is hash-verified in accepted-s11-recovery with
recovery-manifest.json. published-manifest.json is compared byte-for-byte to
O:/winnt; guest, SYSTEM.INI and NTVDM.REG were not rewritten. No Z: mapping
remains. Publication smoke passes 12 immediate native/DOS pairs, 12 immediate
interactive CMD launches, DOS/native/DOS current-cell return and cooked outer
CMD exit 19, GUI startup=0 and explicit wait=37. All eight published hashes
still match after those probes. Governance, relative-link and diff checks pass.

| Product | Published SHA-256 |
| --- | --- |
| run16.exe | 3446C21CA5CEAE65AD7A4059CE7CFA5D36B7DA5D780103CEFDA00AF7ED48D696 |
| ntsrv.exe | 098BAAB8F0B42E40FE51D388B5E498F040A6EF66C83C9B18E299F3319588BD55 |
| ntvdm.exe | F2A9638B5A6A8A01AEAFEA6BD8793D6E41BFB573D9DD2B3D71DA25F69F5428F6 |
| ntvwm.exe | 7817EAD47BF820F3FA325395CC5F43A4C05D3A317F25E7CAD684182DBCE4EABF |
| ntcon.exe | 277ACE6D2CB16046F0D05CC7DB64D6B304E6882C93E3D4FC2075CC7D07FC2340 |
| ntmon.exe | 3DF1024457A4B8D6EFB5C77FF7F566DDDFF202D0C50484478FF5BF0AB532E5F7 |
| WOW32.DLL | BEB688FE832921D788DB297787B2D1062D7FDD095AF1A304824F31DE7F19D0C1 |
| VDMREDIR.dll | 370272091EE1E84F67D8E42A72F03F3F7FC64489C0DE985DC16B495FA6357CDD |

Remaining boundaries: no host scrollback guarantee, no manual current-desktop
focus/RDP-pointer claim, no new Unicode terminal engine and no polling cleanup.
T424 remains open for owner product audit after S13.
