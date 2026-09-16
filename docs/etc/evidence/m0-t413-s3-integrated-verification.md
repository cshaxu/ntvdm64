# T413 S3 — Integrated accessor repair verification

## Boundary and final product identity

Continues owner-approved repair after S2 P1 3cc7851e5 and its
[original-library proof](m0-t413-s2-accessor-restoration.md).
Original CCPU40/C-VID algorithms and the 451-line original accessor body are
unchanged. No IP-mask, x87, broker, WOW/WRITE or CPU instruction fix is included.

Final review builds the complete direct-accessor table in local storage before
publishing Video. Repeated initialization therefore never temporarily publishes
the old generated host accessors. This is the same finite binding profile, not
a new renderer, lock or CPU state machine. The complete original-library fixture
passes again, including the old-provider negative control and concurrent test.

Intentional incremental reuse: S2 formal root retains unchanged objects and
toolchain; the binding/generated dependency is rebuilt and the worker relinked.
The source manifest now names and hashes the shared original accessor explicitly.
Final worker hash below supersedes the intermediate S2/S3 link hashes. All final
acceptance uses this exact runtime package, APP_VERSION 0.0.413 / protocol 3,
MSVC x86 /MT and original CCPU40. No x64/CPU30 fallback exists.

| Product at O:\winnt | SHA-256 |
| --- | --- |
| run16.exe | E713F23D55112628982CB12A84F42E312C9952573765B0CEE2324A09223BB17B |
| basesrv.exe | D43E13F12065500F2FFB6D150F82E0BB6A175F6716E53F93814E5845DBCF9601 |
| ntvdm.exe | 14CC5E57E666DBC66527A3C4B5A515B6F9EC4D7957FB1E93F09D11F0283E78C5 |

Only these EXEs are deployed. Prior exact products remain recoverable below
build/M0-T413/S3/integration/prior-products. Harness artifacts remain in that
integration build root; all actual runtime captures are in O:\winnt\logs.

## Procedure and observed results

BuildConsoleRegression.cmd <integration-root> builds the checked-in native and
ConPTY observers. Verify-CommandExitStatus.ps1 uses that observer, final package,
fresh log prefix and a temporary unused Z: mapping of only the test build root
for the G7.COM/STREAM.CMD/EOF.CMD fixtures. The mapping is removed in finally.
No guest media is replaced, no unrelated process is terminated.

- All 17 command cases pass in m0-t413-s3-accepted-summary.json: empty, native
  zero/missing/seven, native streams/EOF, MEM, nested empty/nested MEM, repeated
  MEM, direct MEM, COMMAND /c, native seven through /c, direct guest seven,
  guest seven through COMMAND, direct native seven and EDIT return. Original
  exit statuses are retained, not rewritten to zero.
- The four real ConPTY sequences COMMAND -> MEM -> EDIT welcome/Esc -> Alt-F/X
  -> MEM twice -> EXIT pass at 80x25, 45x34, 60x50 and 120x30. Captures use
  m0-t413-s3-accepted-<size>.raw. Verify-ConsoleResizeCapture asserts original
  mode 7, one EXIT echo and exactly one blank row after each final MEM report.
  Frame counts are 29/32/28/26; final MEM blocks are 2/2/3/2 respectively.
- Mouse variant passes with 29 frames and two MEM blocks; its journal contains
  original move/down/up delivery. The resize variant changes the live ConPTY
  during EDIT to 45x10, 120x30 and back to 80x25; 38 frames, two MEM blocks,
  normal blank gaps, mode and EXIT checks pass. These are real host size/input
  events, not modified guest registers.
- Native console_resize_contract passes row crop/grow, cursor clamping and
  invalid handle/size/window/read-only rejection. See m0-t413-s3-resize-native.txt.
- Five final-hash native short-window runs pass, logs
  m0-t413-s3-verified-short-01 through -05. Every geometry record proves buffer
  80x300, viewport 0,36..79,40 (80x5), cursor row 40 and 40 prefilled lines.
  Each run reaches EDIT, returns and completes four MEM commands; all four
  per-command snapshots contain the final HMA line. Original exit 1 is retained,
  no timeout or C-VID exception occurs. This is 20 post-EDIT MEM completions on
  the final exact worker, not merely five launches.

## Adverse observations and harness corrections

Retain failed observations rather than relabel them as passing:

1. First native matrix's nested-MEM assertion failed because the old observer
   captured only 4096 characters. The retained snapshot ends mid-second MEM
   report in the wide Console. The observer now captures actual buffer capacity
   with an explicit 4-MiB bound. The unchanged requirement (three distinct-depth
   reports) then passes; no product change was used to bypass the assertion.
2. Short-window setup later exited 93 before launching any product. Its
   geometry-check record reports SetConsoleWindowInfo step 4, error 87,
   buffer 80x300 and viewport 20x5. The observer now explicitly allocates its own
   Console and, only when its physical maximum is below 80 columns, reduces
   that disposable Console's font. It still requires observed 80x5 and cursor
   row 40 after 40 history lines. A requested size is never counted as observed.
3. Five earlier short-window successes used intermediate worker links. They
   corroborate the repair but are not substituted for final-hash acceptance.

The observer uses a visible COMMAND prompt after S2 retired the diagnostic
INTx hook. Reporting labels match that gate; it does not fabricate a DOS BOP.
Full captures correct test evidence only; they do not alter rendering or guest
semantics. The inherited S11 blank-gap binding remains unchanged.

## Final footprint and limits

Re-run AuditCvidAccessorProfile.mjs --output
build/M0-T413/S3/integration/source-audit; preserve the S1 baseline separately.
CCPU is 14 changed files, +304/-61; C-VID is seven, +33/-21. Total mirror
distance is 419 versus 506: 87 fewer diff lines, eight files restored, 17.19%.
The four-file private overlay cohort grows 125 to 136 lines: 11 necessary
binding lines. Combined mirror-distance plus overlay lines is 631 to 555,
76 fewer lines. Build generators/tests are separate, not hidden in that figure.
Generated glue retains 20 typed conversion functions for ten differing pairs;
it is not zero autonomous binding. All 76 field-access bodies are original.

The independent IP-mask finding remains in the S1 report with an explicit
instruction-boundary repair scope; it is not claimed fixed. Full WRITE/x87 and
the historical MEM largest-executable-size anomaly are not validated here.
The latter also appears in the retained T412 final MEM capture, so this work
does not claim to have introduced or repaired it. Actual user Terminal/RDP
visual acceptance remains separate from automated native/ConPTY evidence.
