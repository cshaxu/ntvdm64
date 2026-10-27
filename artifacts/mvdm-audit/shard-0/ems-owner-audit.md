# Original expanded-memory manager and writeback boundary

`softpc.new/base/dos/emm_mngr.c` is selected in the cached x86 worker graph;
the map attributes its allocation, mapping and `LIM_b_write`/`LIM_w_write`
symbols to `original-softpc-dos:emm_mngr.obj`. The 2,129-line original and
current source differ only by the DIV-061 `<stdlib.h>` declaration and the
DIV-148 conditional enabling original LIM writeback under the selected CCPU
profile. The line-ending-normalized comparison was used to distinguish this
from a spurious whole-file diff. The independent source-line metric is 1,066
of 1,067 original nonblank code lines retained (99.91%); it is not functional
coverage, and the file remains partial.

The actual CCPU byte and word write paths call `LIM_b_write` and
`LIM_w_write` for `SAS_MM_LIM`; original writeback synchronizes other windows
and notifies SAS. The original block is guarded out for NTVDM; the selected
build explicitly enables it with `MVDM_SOFTPC_RECOVER_LIM_WRITEBACK` rather
than implementing a replacement algorithm. The original string entry also
exists in the selected source, but this review did not establish a selected
caller; do not infer string writeback coverage from the byte/word references.

Historical T420 S30 guest EMS coverage includes allocation, one-window byte
write, remap/readback, unmap and free. Its pure-DOS profile conclusion has a
documented later correction, so it must not be cited as proving that profile.
Separately, T406 S2 records a formerly failing conventional cross-window
INT 67h/57h copy and a later successful original-boundary lease repair and
guest integration with a different historical executable hash. Its test also
exercises two-window alias word writes and move/exchange. These are meaningful
behavioral witnesses, but neither establishes every EMM API, allocation
failure, reset/teardown, string writeback, or current-package behavior.

The existing independent guest probe was rerun against the S10 package in
`build/t473-s10/ems-current-r4`. It reported `EMS MAP SWITCH ALIAS UNMAP MOVE
EXCHANGE WINDOW FREE PASS`, observer `result=exited`, exit 0, and the test's
own result records cleanup of the isolated stage. The independent COM hash is
`BD1916921A4CCCF7C7B3C1C5D77FB609CFC89029D8F7627E34F8DBDDAB05CD0C`;
the staged `ntvdm.exe` hash
`01A5B459072CA23F49AA43285580BD9017395C936F9DA8B2F18EE7223A52D1F6`
matches `build/package-uuid/package-c/system32/ntvdm.exe`. The observer saw
the stage's own NTSRV child; this does not prove concurrent multi-package
isolation. Two earlier harness launches failed before integration, once from
an unset default script path under Windows PowerShell and once from unavailable
`Get-FileHash`; only the completed PowerShell 7 run is a pass. The guest
probe is independent test code; no existing guest binary was rebuilt or
modified. Full EMM API/error and teardown coverage remains unproven.
