# Published Winmine window observation

The installed `O:\winnt\system32` ten images were copied to an isolated
test package, assigned a fresh package identity, and launched through its
Run16 with a copy of `O:\w31\WINMINE.EXE`. The installed Winmine and this
guest have the same SHA-256. No installed component or guest was changed.

Two runs observed the main HWND with style `0x34ca0000`, which includes
`WS_CAPTION` and `WS_MINIMIZE`; the second observed title `Minesweeper`,
window rectangle `(-32000,-32000)-(-31763,-31961)`, and zero client area.
The native creation trace recorded requested style `0x20ca0000`, already
including `WS_MINIMIZE`. The isolated observer did not supply
`STARTF_USESHOWWINDOW` to Run16. These observations show a minimized window
in this test; they do not establish that the user's separate live window had
the same state or identify why the minimized style was requested.

The first modal probe failed with a child creation 5023; the second completed
the modal interaction. Neither run establishes visual title rendering or
isolates the cause of the user's report. Evidence is retained in
`build/t473-s11/published-winmine-review/{observed,geometry}`. The staged
package uses a fresh identity, so it is a controlled installed-image comparison
rather than a byte-identical invocation of the installed environment.

A separate observer variant supplied `STARTF_USESHOWWINDOW` with
`SW_SHOWNORMAL` to Run16. The main window then reported style `0x14ca0000`
(visible, not minimized), title `Minesweeper`, window rectangle
`(-2,-45)-(235,195)` and client origin `(6,6)`. Thus in this controlled run
the upper nonclient area is above the primary desktop origin while its
client area starts on screen. The same creation trace still records initial
style `0x20ca0000`; the experiment demonstrates a difference correlated with
the launcher's show request but does not identify the source of window
position or prove the same state for the user's window. This run's later
modal probe failed with exit 6; do not treat it as a full product pass.
Evidence: `published-winmine-review/normal-show` and `observer-normal.c`.

An elevated read-only enumeration after the user's reply found no window
with a Minesweeper title on the observer's desktop; the only NTVDM visible to
process inspection belonged to an unrelated isolated `WIN.PIF` test stage.
No inference about the user's actual HWND is made from that absence.

The read-only Windows event witness for the normal-show isolated package
recorded the first `EVENT_OBJECT_CREATE` and subsequent location events at
`(-2,-45)-(235,195)`; the recorded bad position was present at creation, not
first introduced by a later observed move. A separate current-source
diagnostic logged an explicit creation request of `(5,-38)` in one run and
`(12,-31)` in another; it is a different instrumented provider, not the
installed DLL. In the latter run the original `WK32GetPrivateProfileInt`
read `[Minesweeper] Xpos=14, Ypos=14` from `winmine.ini`, followed by the
creation request `(12,-31)`. The thunk passes the guest-provided style and
coordinates on to the native creation binding. `WU32AdjustWindowRect` tracing
did not record a matching call in this witness; the exact guest calculation
and any other metrics sources remain unproved.

The supposedly isolated probes nevertheless accessed the user's profile:
`C:\Users\neko\AppData\Local\VirtualStore\Windows\winmine.ini` was
modified at the time of the latest diagnostic and now contains
`[Minesweeper] Xpos=21, Ypos=21`. Earlier diagnostic reads were 14/14, so
the test runs changed at least those saved values. Its original values before
this sequence were not captured and cannot safely be reconstructed. Do not
run another Winmine probe that writes its profile until the test contains that
profile; do not overwrite the user's file with guessed prior values. This
configuration leak does not prove what caused the user's initial report.

For further investigation, a diagnostic-only WOW32 copy redirects the bare
`winmine.ini` profile name to an absolute test-owned file. A standalone native
fixture compiled the exact diagnostic filename helper and passed mixed-case
name resolution plus host profile read/write against that test file. Source
inspection confirms the selected WOW `GetPrivateProfileInt`,
`GetPrivateProfileString` and `WritePrivateProfileString` thunks all use that
helper. This does not prove that every possible guest or related process file
access is intercepted. The attempt to run Winmine with this diagnostic was
rejected by automatic approval review because the earlier probe had modified
the user's profile and a post-run hash cannot prevent another write. No
Winmine run occurred after that rejection. The diagnostic helper test and its
output are under `build/t473-s11/geometry-isolated`; no product binary was
changed by this test-only redirection.

The guest's NE import relocation table includes KERNEL ordinals 127, 128 and
129, which the original `kernel31/kernel.def` identifies as the three private
profile APIs. It does not directly import KERNEL `OpenFile`, `_lopen` or
`_lcreat`; however it does import ordinal 102 (`DOS3CALL`), so absence of
those imports alone does not exclude all possible DOS-level file access. A
read-only safety check under the default sandbox attempted to open the
existing VirtualStore profile for writing without writing or truncating it;
that open was denied. This is useful evidence for that process, not a proof
that the desktop test and all its descendants would have the same boundary.
No disallowed product run was attempted.

Follow-up static NE relocation analysis located all six direct KERNEL
`DOS3CALL` call sites at offsets 0x261e, 0x2640, 0x2747, 0x2772, 0x2796
and 0x27d3 in the code segment. Their immediately preceding setup selects
DOS services 0x35 (get interrupt vector), 0x25 (set interrupt vector),
0x4c (process exit), 0x25, 0x25 and 0x30 (DOS version), respectively. The
linear 16-bit disassembly found only the corresponding six `int 21h`
fallbacks. This reduces concern about a direct guest file-open at those
sites; linear disassembly and the import table cannot prove every possible
dynamic call or every other process's writes. No rejected test was rerun.

Read-only inspection of the exact Winmine NE image narrows the calculation.
Its code segment is at file offset 0x620; the relocation chain at segment
offsets 0x1847, 0x1852, 0x185d and 0x1868 resolves to USER ordinal 179,
`GetSystemMetrics` (the project's original `user.def` confirms the ordinal).
At code offsets 0x1844..0x186d the program saves metric 4 plus one, metric
15 plus one and metric 5 plus one. At 0xc11..0xc22 it sums the first two
when the menu is present; at 0x132..0x141 it subtracts the resulting values
from the saved position before creating its window. Thus the observed
`Ypos=14` and requested top `-31` imply a combined vertical subtraction of
45 pixels in that run. The native-creation witness independently recorded the
same requested coordinate. A separate read-only host query reported
`SM_CYCAPTION=23` and `SM_CYMENU=20`, matching `23+1+20+1=45` in this
desktop context; that query was not taken atomically with the diagnostic run.
This proves how the guest computes that coordinate
for the observed execution; it does not establish whether every small saved
position originated with the user, an earlier runtime, or the leaked tests,
nor whether changing generic metrics or placement would preserve other apps.
The generated disassembly is `published-winmine-review/winmine-disassembly.txt`.

A separately spawned Python child in the default workspace sandbox also
failed to open the existing VirtualStore INI with `O_WRONLY` (without writing
or truncating it). Read-only source tracing finds `run16` starts a missing
broker with `CreateProcessW`, and the broker starts the worker with
`CreateProcessW`; the inspected paths do not use `CreateProcessAsUser` or a
token-duplication API. This narrows the potential protection model but does
not prove that the elevated desktop test or a pre-existing broker is subject
to the same sandbox. The rejected Winmine action was not retried.

After the owner explicitly requested a fix for the off-screen title, the
actual VirtualStore INI was backed up byte-for-byte to
`build/t473-s11/published-winmine-review/winmine-before-position-repair.ini`
(SHA-256 `F9F974B8BE9FF9C03A7DBE55FFBBCE8A393550159B02C01D6F8CD46B0CB802A4`).
Only `[Minesweeper] Ypos` was changed from 21 to 80, after verifying that
exact hash and a unique matching entry; the resulting SHA-256 is
`9DEE67A498716EEECED182A0055B563ED1067E43E486E9EF1DE0FBEA947E505B`.
All other bytes remained identical. With the measured 45-pixel subtraction,
80 predicts a 35-pixel window top on the observed desktop. This repairs the
current saved setting only; it does not prove a generic placement policy or
the actual appearance after launch. The previously rejected Winmine probe
was not rerun, and the backup is the immediately pre-repair state, not the
unknown value before earlier tests touched the profile.

A read-only filename search of repository `build`, `artifacts`, `assets` and
`docs` found only the already identified post-test/pre-position-repair backup
and the test-owned `geometry-isolated/winmine.ini` containing only a synthetic
Ypos. Neither is evidence of the user's values before the first probe. This
bounded filename search does not rule out backups elsewhere or versioned
system snapshots; it did not change the user's current profile.

A later read-only SHA-256 recheck of the current Winmine VirtualStore INI
matched the recorded post-position-repair hash
`9DEE67A498716EEECED182A0055B563ED1067E43E486E9EF1DE0FBEA947E505B`.
This confirms the bytes match that later checkpoint at the time of the read;
it neither reconstructs pre-probe values nor establishes future test isolation.
