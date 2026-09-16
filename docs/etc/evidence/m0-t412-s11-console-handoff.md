# T412 S11 Console investigation handoff

## P3: cell-grid repair; owner-directed bounded S11 closure

The owner now directs S11 closure for the verified blank-gap repair and a new
S12 for the CCPU/C-VID investigation and original-source restoration. This
supersedes the earlier instruction to include the accessor repair in S11.
The 80x5 crash remains a failed test assigned to S12, not a passing S11 result
or full T412 acceptance. Live external resizing and owner Terminal/RDP visual
confirmation are not claimed by the static-size matrix; S12 retains the
adjacent geometry/adverse regression checks while repairing the crash.

The unverified accessor/build edits were removed from the product source and
retained in `build/M0-T412/S12/admission/accessor-research.patch`, SHA-256
79ACAD335642107F72FC8D8B4605C60B492DB18A9452C8095565A67D57A7A78F.
It is a research candidate, not a fix or acceptance input. Its generated S11
accessor-profile graph is also non-release material. No stash or extra worktree
is required to preserve this work.

The native cell observer, not just VT replay, reproduces the defect at 45x34:
after EDIT, repeated MEM paints text at rows 4--11 while the prompt reaches
row 25. The default-off temporary PAINT/RESTORE observations show that malformed
rows already enter copyConsoleToRegen from the native Console. Painter stride
remains 80 columns / 160 bytes; changing CPU or guest cursor semantics is not
justified. Diagnostic mirror edits were removed after collecting these traces.

Original owner: OpenNT windows/core/ntcon/server/output.c::ResizeScreenBuffer
and getset.c::SrvSetConsoleScreenBufferSize / SrvSetConsoleWindowInfo. The
original operation copies/clips cells by row, extends columns with spaces and
the row's last attribute, selects TopRow when height excludes the cursor, and
clamps cursor X to zero / Y to the final row. It does not reflow paragraphs.
Modern ConPTY can also resize/reflow storage when SetConsoleWindowInfo changes
the viewport. Restoring only SetConsoleScreenBufferSize is insufficient.

Recovery ladder: the original translation unit cannot compose directly because
it owns private SCREEN_INFORMATION/ROW, CSR allocation and GDI window painting.
The selected same-shaped facade retains the reached cell-copy, clipping,
attribute and cursor rules using Read/WriteConsoleOutputW around the native
geometry operation. No external-source intrusion or CPU changes are selected.
This finite facade is the recovery exception; it does not invent a worker,
alternate presenter, stream-mode override or persistent second screen model.
Native validation decides invalid handle/size/window/access failures. Partial
cell transfers fail explicitly. A failure after native resizing is not claimed
to roll back the native geometry; the original caller retains error handling.
The reached read/write Console handle is required, not an arbitrary write-only
handle. This is a finite modern binding, not a whole Console Server recovery.

Retained runtime inputs below the prescribed logs directory are
m0-t412-s11-baseline-cells.raw[.cells.txt], m0-t412-s11-cells-boundary.txt,
m0-t412-s11-noreflow.raw and m0-t412-s11-window-grid.raw[.cells.txt]. The first
two-API candidate removed the repeated-MEM blank gap in native cell data. The
final shared implementation also blanks newly added rows after window resizing,
removing a duplicate EXIT echo left by reflow. xterm
headless is isolated under build/M0-T412/S11/terminal-check, not a dependency.

### Verified delivery and limits

Closure publication copies the three exact products below to `O:\winnt`.
The intermediate capture `m0-t412-s11-closure-45x34.raw` passes on the earlier
link (worker SHA-256 9728C7390F6E6FBDC8E80F12C17BA1D3C48F62783E4EEB156DEA4A5C6C8F6DAE).
After withdrawing the accessor candidate, final build revalidation recompiles
the unchanged binding/termination sources and relinks the worker. Four fresh
`m0-t412-s11-final-<size>.raw` runs and all 17 cases in
`m0-t412-s11-final-summary.json` pass on this final link. Each final Console
capture has original exit 1, MEM gaps 1, restored mode 7 and one EXIT echo.
All three destination hashes match the following final-product table.
The final `m0-t412-s11-final-mouse.raw` run also passes: 26 frames, two MEM
blocks with gaps 1/1, output mode 7 and one EXIT echo. Its mouse journal
retains stage-1/2 move/down/up button values 0/1/0 and stage-3 delivery.
The isolated native resize fixture passes again. Invoking that fixture directly
through the agent's inherited Console failed its row check; its explicit
independent-process launch passes, so the inherited launch is not counted.

Fresh formal root: build/M0-T412/S11/cell-grid, MSVC x86 /MT, original CCPU40,
APP_VERSION 0.0.412 / protocol 3. All three product links and the 4208-byte
VDM_TIB storage gate pass. The selected source contains no temporary SoftPC or
C-VID diagnostic hook. Product hashes:

| Product | SHA-256 |
| --- | --- |
| run16.exe | 532AB1738C7D0747EF6C894FE37AA9FAAFF6D56F1D090EEF578D708B3B1A778F |
| basesrv.exe | 02D93ED396792873B1BDCC5F7FCCD3566B70BB89F4E243C09BD9976ED35A967B |
| ntvdm.exe | A33D3EE327732F2A46A3F626770AADFA80DAF379BC9883E3F94F482E3F1B065E |

Initial source-equivalent link verification (the final rerun is recorded above):

- Native console_resize_contract: original row selection/crop/grow, cursor
  clamps, invalid handle/size/window and read-only handle rejection pass.
- Real ConPTY COMMAND -> initial MEM -> EDIT welcome/Esc -> Alt-F/X -> MEM
  twice -> EXIT passes at 80x25, 45x34, 60x50 and 120x30. Native cell journals
  `m0-t412-s11-verified-<size>.raw.cells.txt` show exactly one blank row between
  each final MEM block and its next prompt, one EXIT echo and restored mode 7.
  Verify-ConsoleResizeCapture.ps1 checks those facts, not just process exit.
- The same product passes all 17 Verify-CommandExitStatus cases, including
  native streams/EOF, nested COMMAND/MEM, EDIT and original exit statuses.
  Results: `m0-t412-s11-verified-summary.json` under the runtime log directory.
  Temporary Z: points only to this build fixture root and was removed afterward.
- The optional ConPTY mouse sequence records movement at row 9/column 19 with
  buttons 0, then down 1, then up 0 at original stages 1/2, with stage-3 guest
  delivery. EDIT exits normally and repeated MEM still passes the cell checks;
  retained input is `m0-t412-s11-mouse.raw[.mouse.txt,.cells.txt]`.
- At 45 columns the original fixed-cell resize clips long lines; it does not
  invent a paragraph-wrap policy. A narrow viewport is not evidence of full
  80-column visibility. Actual owner's Windows Terminal/RDP acceptance is still
  separate from native Console journals and independent headless VT replay.

Footprint: zero mirror/overlay edits and no CPU, guest, broker or protocol
change. Production additions are two same-shaped declarations/macros and one
shared synchronous Console resize implementation (109 lines total); temporary
geometry and exception observations are not part of the delivered source.

### Remaining short-window C-VID gate

The retained prefilled-history observer confirms real 40x5 and 80x5 viewports
over 300-row buffers, with 40 preceding shell lines. The 40x5 run passes with
normal guest exit 1. The 80x5 run fails: worker access violation, launcher 1067.
It must not be counted as passing. The owner moves its repair to S12 when
closing the bounded S11 blank-gap delivery; T412 is not closed by P3.

`m0-t412-s11-history-80.txt.exception.txt` maps to original
SEVID019.c::S_2696_Chain2ByteWrite_Copy's ring write. Read-only follow-up
`m0-t412-s11-cvid-2-fault.txt` records r1/Gdp=012025a8, r21=012025a8,
ring pointer=017a65e4, bounds=005a3fe0..005a4040 and GDP state=6666. Subtracting
Gdp gives 005a403c: the ring increment used Gdp, not the source's constant 4.
State 6666 belongs to SINIT013.c::CiSetVideodirty_total, which shares the global
J-code scratch registers. This strongly identifies accessor interference;
the thread/reentry owner and complete interface-profile repair still need proof.

Original vglob.c already supplies direct field access without this context
switch. It is compiled but not selected in the product map. Its CCPU header
uses VGAGlobals offset 1535, whereas C-VID uses 1280, and their VideoVector
layouts differ. Merely enabling that object or changing one vector slot is
unsafe. The owner's later direction explicitly moves this complete original
accessor/layout composition repair from S11 to S12. It remains required within
T412, not an unowned queue item. Admission is not implementation or acceptance
evidence: the repair and repeated adverse runs still need to pass.

The diagnostic-only source patch is explicitly retained at
build/M0-T412/S11/cvid-diagnostic.patch, SHA-256
9AA5DF69B4E057D284937D1DD775BDB81F6D666630D56C594465259C8F13728A,
with its comparison products under S11/geometry. It is not a stash, worktree,
runtime dependency or release input. Raw records remain in `O:\winnt\logs`.

## Question and bounded predecessor conclusion

On 2026-09-15 the owner requests cleanup and a new S to repair abnormal blank
rows after COMMAND -> EDIT -> exit EDIT -> MEM. S10's broker process-loss
delivery at 157267256 is retained as a bounded conclusion, not full T412
acceptance. S11 receives this regression and the adverse short-window result.
No production source or uncommitted document existed at cleanup start.
No stash or hidden unaccepted product patch is being carried forward.

## Exact inputs

Published O:\winnt products were rehashed unchanged during admission:

| Product | SHA-256 |
| --- | --- |
| run16.exe | 20709EC0F1DB68D465EB994C3D14E134618A2CB289A64B2B4080FCA6E80F6C49 |
| basesrv.exe | 2152439F487D305E77A1AFD5DCC7D7D4F0079AD48E3BBDF398749CC4AD30D8B2 |
| ntvdm.exe | 3344EA956EF4EA19E53966190C922A78A61263D408B4190EF53A57304A1EAE50 |

Formal source/build lineage: S10/process-loss, MSVC Win32/x86 /MT CCPU40,
APP_VERSION 0.0.412 and protocol 3. Immutable guest media remains in `O:\winnt`.
The owner screenshot shows MEM's last lines separated from the prompt by a
large blank area in Windows Terminal over mobile RDP.

## Procedure and observations

Prior investigation used isolated conhost observation and a ConPTY child with
explicit CONIN$/CONOUT$ standard handles. The sequence was initial MEM,
EDIT, Escape to dismiss its welcome, Alt-F/X to exit, MEM twice, then EXIT.
Raw captures remain under `O:\winnt\logs`:

| Input / observation | Result and limitation |
| --- | --- |
| m0-t412-s10-conpty-bound.raw (39053 bytes), initial 45x34 | Contains 80-column resize requests and padded frame writes; bounded VT replay produced up to 23 blank rows. |
| m0-t412-s10-conpty-80x25.raw (37905 bytes) | Same sequence, fixed 80x25 replay: at most one ordinary blank row. |
| m0-t412-s10-conpty-feedback.raw | Experimental resize feedback still clipped text and produced gaps; NOT a fix or passing test. |
| m0-t412-s10-edit-mem-repeat-40/80 observations | Ordinary conhost repeated MEM did not reproduce excessive gaps. This does not validate Terminal. |
| m0-t412-s10-edit-mem-history-40/80 exception reports | Short-window/prefilled-history tests faulted with 0xc0000005; retain as separate unresolved adverse evidence. |

The replay decoder is a bounded test model, not Windows Terminal. Neither it
nor a successful process exit proves GUI/RDP acceptance. Early ConPTY probes
with non-Console inherited handles were invalid startup controls, not product
failures. Requested dimensions must not replace observed dimensions: one
requested tall-window case actually remained 20x5 after failed resizing.

## Interpretation and source boundary

High-confidence working diagnosis is a guest/host geometry and presentation
lifecycle mismatch after EDIT enters frame output. The exact repair is not
yet established by a passing product. It is not evidence of bad MEM guest
semantics or a CCPU instruction defect.

Source inspection identifies nt_graph.c window/buffer sizing and cursor calls,
nt_fulsc.c stream-to-screen transition, and the win32 adapter's
present_text_invalidation. The latter checks WriteConsoleOutputA success but
not complete returned rectangle coverage. Original guest scrolling and mode
selection must remain source-owned; forcing stream mode after EDIT would
bypass rather than repair that contract.

Read-only comparison with the sibling softpc repository (../softpc) found its
src/compat/graphics_console_compat.c logical geometry facade and
src/lib/console-broker/win32/console.c separate cooked/raw screen buffers,
restoration, shared paint/cursor handling and returned-rectangle validation.
Its resize no-ops are meaningful only with that complete presenter. Do not
copy them independently, import the library wholesale, or claim the adjacent
project was tested under this identical RDP setup.

The short-window fault maps into generated C-VID crules (SEVID000/SEVID019);
one write is a diagnostic/history ring entry (2087), not established video
RAM corruption. Causation is unproved. S11 must triage it before claiming
display acceptance; a required CPU-semantic change hits the explicit stop
condition instead of authorizing a trace-driven instruction patch.

## Cleanup and intentional reuse manifest

Research source remains only in build/M0-T412/S10/console-width for intentional
S11 reproduction review, not a formal source/build dependency or delivered
fix. The generated observer includes the existing observation test; review and
promote only the minimal reproducible fixture into tests before acceptance.
The following SHA-256 identities freeze those inputs:

```text
decode.mjs 874F736ECD93050B784DD7C550D11BBEDC13F01E158598D8F09B384E04DD73D6
generate.mjs 1DB02A880487274C7CFF29C414DD841CE798DB710FF1F76CB4AAA679CDFAA814
observer_sequence.c 0E1DE87C031B11A7E1567FF5EF46CDC1AC5972DFC8EA76FF50F582AC37D5FC8E
pty_probe.c FC93144DCC344B42AF62D8E37B7256C7868AF0013555AA3548F517B794E5D399
run.mjs 99C728B0CF7764CC79389DA2B339EE42FAC0DA149612953DF465946B37DE5A05
width_observer.c BB859536EF3ACC17FD2C9ABC724C63BAE40192C80D2F9FD0F4341B6BDD271DAC
```

Remove only width_observer.exe/.obj and pty_probe.exe/.obj from that exact
research directory. Their source is retained; rebuilding must use a fresh
S11 run root and the recorded x86 /MT toolchain. No raw observation is deleted,
no formal S10 build baseline or O:\winnt product is replaced. A fresh process
inventory at admission found no run16, basesrv, ntvdm or research helper
processes; no process termination was necessary.

## Next verification

### Rejected implementation experiment

The inactive-buffer capability probe under build/M0-T412/S11/buffer-probe,
compiled with MSVC x86 /MT, reports before=45,34 after=45,34 backing=80,25
resize=1,1. No resize sequence appears in its ConPTY output. This proves a
platform mechanism, not repaired guest execution.

Recovery ladder: (1) original nt_graph/nt_fulsc algorithms remain selected;
the NT4 server bitmap.c SrvInvalidateBitMapRect/WriteToScreen translation
unit cannot compose because it requires CSR Console objects, private locks
and GDI window/server state; no whole-server compilation is claimed.
(2) the attempted facade retained Console API shapes over native inactive
cell storage, keeping inherited stream handles intact. (3) a candidate
ResetConsoleState release hook ended the binding; no algorithm overlay was
attempted. (4) finite viewport/input translation was experimentally labelled
ADAPTER-WIN32-052. These identifiers are withdrawn with the failed code, not
accepted production recovery entries.

The first focused fixture disproved native window/cursor isolation: manipulating
an inactive buffer's window still affected shared Console geometry/cursor.
A revised candidate retained native cell storage but held logical geometry
locally. Its native fixture passed geometry, cursor/mouse mapping, release,
rebind and invalid-handle checks. A fresh x86 /MT three-program link succeeded
under build/M0-T412/S11/formal, including the VDM_TIB storage gate.

Integration nevertheless FAILED: m0-t412-s11-conpty-45x34.raw under the
runtime log root completed COMMAND -> MEM -> EDIT -> MEM twice -> EXIT,
but bounded replay still showed corrupted text and an 11-row gap. Resize
sequences were absent. Guest COMMAND's final 1 is its original result, not
an assertion that rendering passed. The harness's first empty capture was
an executable-quoting defect and is not product evidence.

This falsifies resize suppression/viewport projection as a sufficient fix.
Do not claim the remaining cause is proved: original stream/frame transitions,
screen-to-regen copy and native-child handoff require correlated cell/cursor
observations before another implementation. No broader CPU change is justified.

All candidate production, fixture and build-selection edits were withdrawn.
Their reviewable diff and added sources are intentionally retained only at
build/M0-T412/S11/rejected-candidate.txt, SHA-256
B52C6332DFCCC2330E0A31586EE55B863EC4B37D35672FBDC7172E1282143679.
The failed formal build and prior-products backup are retained as identified
comparison artifacts, never a new default or release input. The prior three
products were restored to the exact hashes above; the test's Job cleaned its
own process tree and no package processes remained before restoration.
No user testing is requested for this rejected candidate.

The [working plan](../operations/m0-t412-broker-restoration-plan.md)
defines source audit, one coherent boundary repair, and real-program checks.
This record is investigation preservation and admission evidence only;
repair implementation and repaired-product acceptance remain pending; the
failed candidate's successful build is not repair acceptance.
