# T423 S33 first-command Console preservation

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Reproducer and root cause

The owner observed `cmd.exe` at 80×30, `run16 command`, then the first
`ver` or `dir`: command output flashed and the screen became nearly blank.
The screenshot's DOS prompt alone did not prove COMMAND had crashed. A real
80×30 ConPTY observer reproduced the transient
blank page in the published S32 eight-file package. Its first 80×28 page
contained no earlier CMD banner, COMMAND banner or VER output. The S32
negative capture is
`build/M0-T423/S33/probe/conpty-negative.raw.cells.txt`.

`run16_console_prepare_dos` in `src/ntcon-exe/console_frontend.c` selected
the OpenNT-correct 28-line DOS height, but first set the shared Console
viewport to 1×1 before changing its buffer to 80×28. In this real ConPTY
path, the 1×1 viewport also made the backing buffer 1×1, destroying the
prior cells. The observed sequence was 80×30 with text → 1×1 with a blank
first cell → 80×28 with blank cells → COMMAND repainting its prompt. This is
an NTCON geometry-adaptation defect, not a guest `VER` or original MVDM
command parser defect.

The production change keeps the pre-resize viewport at the largest region
both the current buffer and target VGA page can represent, bounded by
`dwMaximumWindowSize`. It still uses the existing OpenNT row-copy resize
primitive, original 22/25/28/43/50 row selection and final logical VGA
window. It does not reflow text, inject redraws, delay production execution,
change the guest, alter a protocol or add a worker path. The similar-issue
sweep found no second one-cell pre-resize path in NTCON/NTW32/run16.

## Regression evidence

The checked-in test extension is
`tests/observation/console_terminal_observer.c` with
`--s33-first-ver` and `--s33-first-dir`. It launches a real outer native
CMD in 80×30 ConPTY, enters `run16 command`, waits seven seconds after the
first command, then runs MEM and exits. It checks the *first* 80×28 cell
snapshot for both prior COMMAND text and the command's output, so a transient
blank page fails even when the next prompt is repainted. It also requires
real MEM guest text after the wait. The test-owned Job only cleans up the
observer process tree; it is not a product lifetime mechanism.

| Package / route | First 80×28 page | Later guest MEM | Result |
| --- | --- | --- | --- |
| S32 published / first VER | blank (`preserved=0`) | present | expected negative, exit 1 |
| S33 formal / first VER | CMD, COMMAND and VER cells retained | present after seven seconds | pass, exit 0 |
| S33 formal / first DIR | CMD, COMMAND and directory cells retained | present after seven seconds | pass, exit 0 |

Raw final evidence: `build/M0-T423/S33/probe/s33-formal-ver.raw*` and
`s33-formal-dir.raw*`. Both report
`s33-child-after-command=258 final=1 preserved=1 dos-alive=1`;
258 is `WAIT_TIMEOUT` for the still-live outer CMD at the check point, and
`dos-alive=1` is based on the actual later MEM output. These bounded checks
do not prove arbitrary-duration COMMAND stability, but they disprove an
immediate first-command crash in the tested path.

The MSVC x86 `/MT` CCPU40 serial graph completed 596/596 selected commands
under `build/M0-T423/S33/candidate/`; log:
`build/M0-T423/S33/probe/serial-full.log`. The complete final eight-file
candidate was staged at `build/M0-T423/S33/runtime-candidate/`. Against
that *formal* byte set, `Verify-CommandExitStatus.ps1` passed all 17 ordinary
Console and all 17 private-desktop Window cases, including COMMAND/MEM/EDIT
direct, interactive and nested routes, EDIT then MEM, guest output and exit
codes. Logs: `formal-console.log`, `formal-window.log` under the S33 probe
directory.

The formal `console-frontend-test.exe`, `console-channel-lifetime-test.exe`
and `frontend-scope-lifetime-test.exe` passed when run with their required
independent hidden Console/stream roles. `ntw32-presentation-test.exe` had
one input-queue count failure on its first isolated run, then passed 414
checks on repeat; the S32 test also passed under the same launch conditions.
The failure is retained in `ntw32-presentation-final.log` and is not counted
as a first-run pass or attributed to this NTCON resize change. The separate
private-desktop WOW observation reached the retained S32 frontiers:
WINMINE had a visible game window, SOL its existing modal, and WRITE the
existing “Not enough memory for Write” dialog. This was not gameplay or
full SOL/WRITE acceptance. Log: `formal-wow-frontiers.log` and its three
`s33-formal-wow-*-windows.txt` reports.

## Published package and limits

The previous coherent eight-file package and `CONFIG.NT`, `AUTOEXEC.NT`,
`SYSTEM.INI` were saved under
`build/M0-T423/S33/published-backup/` before replacement. Guest binaries
and configuration were not edited. The final tested candidate hashes are:

| File | SHA-256 |
| --- | --- |
| `run16.exe` | `FAFA724C73CFC147962E461183FDD5418872D809B19C959C4FAD5C01395403F7` |
| `ntsrv.exe` | `DF40573700567BD20926574385D6CDBF2F1177F7BCD7AB3058B01B0783BF114B` |
| `ntvdm.exe` | `BA14F4997E454B23488FF6C048438CDC7D664D66EFDC2686894420EC5B8390C8` |
| `ntw32.exe` | `BBDE203EB8E0D0AB79653D7A717F38E3FE41ACF2F8F1BFD678C669A3BDF5180F` |
| `ntcon.exe` | `5835991ADE6A16D888B77E7C2E95C29CCFA933A14E02115C90DCD07CAC2CC25B` |
| `ntmon.exe` | `59D3DEBFAB067BE73ACF7F136BC5B1112E41F457206E74EE76C0A668471B550B` |
| `WOW32.DLL` | `0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A` |
| `VDMREDIR.DLL` | `FF8CE7F8272798C0A7052249AB69A451F7BED8E0EED72340D9F2D5257C45E76D` |

The owner's physical Windows Terminal/RDP replay remains for side-test;
the real ConPTY test covered the same geometry/first-command transition
without taking over the desktop. T423 remains open for owner acceptance;
S34's task-trace proposal was not implemented in this S.

All eight tested candidate hashes above were verified byte-for-byte after
publication to `O:/winnt`; only these eight binaries were replaced. Published
smoke passed `direct-mem` (0), `edit` followed by MEM (1, original COMMAND
exit behavior) and `native-cmd-dos` (0). Log:
`build/M0-T423/S33/probe/published-smoke.log`; runtime reports are under the
approved `O:/winnt/Logs2/` directory. The same first-VER ConPTY test against
`O:/winnt` reported `preserved=1 dos-alive=1` after the seven-second wait;
raw file: `build/M0-T423/S33/probe/s33-published-ver.raw*`. The temporary
`Z:` mapping used for isolated tests was removed.
