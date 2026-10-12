# T445 S2 accepted palette repair delivery

## Scope and review

Owner explicitly accepts the concurrent repair and requests its commit/push
as new S2. Standalone UI work moves to S3; no UI implementation is included.
Production change is confined to NTVWM console_state.c. No mirror/guest,
protocol or other runtime component source changes.

Palette application formerly passed inclusive getter bounds to the InfoEx
setter and reused source geometry/maxima after resizing. The repair rereads
current geometry, copies only palette/popup attributes and converts right/bottom
to exclusive bounds. Existing final geometry verification remains intact.
Validated positive buffer sizes bound the conversion within SHORT range.

The focused fixture checks the actual setter before later restoration could
hide a transient shrink. Ordinary capture tests cover repeated palette changes,
resized/scrolled viewports, bottom-right cursor and retained edge cells.

## Retained build and runtime evidence

Evidence root: build/side-palette/r001. This is pre-existing accepted work,
not a fresh full-suite execution by the closure audit.

- run.ps1, build.txt and exitcode.txt: optimized native worker build succeeds.
  fixture6 ordinary/observed tests pass. The child broken-pipe diagnostic in
  ordinary output belongs to its deliberate resource-lifetime scenario; the
  parent test succeeds.
- negative/result.txt: old production code fails the exclusive right/bottom
  conversion assertion; the negative test is not counted as a product pass.
- product2/timings.json: WOW frontiers, Console-17, Window-17, RPC and native
  GUI gates pass. Full runner stops at version-negatives; it is NOT a Full pass.
- version.ps1/version.stdout.txt/version-exitcode.txt: separately pinned
  0.0.444 baseline passes all five version/interface rejection cases. This
  retains the accepted package version rather than claiming a new 0.0.445 build.
- remaining.ps1/remaining-exitcode.txt and remaining/result.txt complete strict
  DIR, modern EDIT return, cooked return, rapid/native and interactive relaunch,
  session isolation, retirement wiring and nested Window handoff.
- native-fixtures and gameplay: presentation, input return, lifetime and real
  Minesweeper flag/reset/open/close plus launcher completion pass.
  SOL/WRITE frontier observation is not full application/gameplay acceptance.
- publish-exitcode.txt: candidate published; nine images unchanged, NTVWM
  replaced in assets/release and configured O:/winnt/system32.
- smoke-result.txt: deployed interactive COMMAND and CMD VER/EXIT pass.

The closure check under build/M0-T445/S2/r003 independently compares all ten
candidate, repository-release and deployed image hashes, checks governance,
links and diff hygiene. NTVWM SHA256:
DAE6785D620147CA0A1DED5401A99391730CDE3BBA230F660B07617B98182019.

## Disposition

User acceptance is explicit. This delivery includes all six previously
uncommitted repair/test/asset files plus the sequencing and evidence records.
S2 closes with its commit/push; T445 remains open. S3 standalone UI requires
admission and owner stability acceptance before S4 product propagation.
