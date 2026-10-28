# DOS redirector DLC shared window ABI — shard 3

This review covers pending `inc/VRDLCTAB.INC` and the empty `inc/APISTRUC.INC`. `dlc-window-observer.py` and captured JSON compare the window header, matching C definitions, guest redirector sources and host DLC dispatcher against both original editions; eight files match both references exactly. `APISTRUC.INC` is empty in current and both original trees (SHA-256 of empty file in `file-review.csv`); historical `inc/makefile:111` identifies its original generated-input role. No guest build, product execution or network device operation was performed.

Guest `dos/v86/redir/int5c.asm:41,123-162` includes VRDLCTAB.INC and chooses DLC SVC for an INT 5C request whose first command byte is less than 10h. `vdmredir/vrdisp.c:97-99` maps service 0x26 to asynchronous completion, 0x27 to VrDlc5cHandler and 0x28 to VrVdmWindowInit. The guest window in `VRDLCTAB.INC:81-87` and C version `vrdlctab.h:134-140` have matching named components in source order; their exact packed sizes and deployed image relationship have not been measured here. Existing shard 3 private map records VrDlc5cHandler, VrVdmWindowInit and TerminateDlcEmulation, while `redir-role-review.md` bound that private link's executable text to a then-released image. This is not a live DLC call or current release identity proof.

Original `vrdlc5c.c:3503-3543` retains the 16:16 window address and a `GetVDMAddr`-derived host pointer, used later for statuses and post callbacks; `vrdll.c` invokes TerminateDlcEmulation on process detach. A valid pointer lifetime, guest-window retirement and completion race cannot be inferred from source or map. Crucially, prior `docs/etc/evidence/m0-t420-s20-vdmredir-operation-matrix.md:242` reports its examined `SVC_VDM_WINDOW_INIT` call in redir.asm is commented out and no selected caller; therefore neither successful initialization nor a current reachable stale-pointer fault is established. Other guest or external TSR callers and an available DLC provider require independent verification.

| file | proposed status | boundary |
|---|---|---|
| `inc/VRDLCTAB.INC` | pending | Original structure and source call chain established; selected guest caller, exact ABI sizes, async completion and cleanup remain open. |
| `inc/APISTRUC.INC` | excluded, empty historical file role only | Zero bytes in both originals and mirror; exclusion says nothing about nonempty `apistruc.h`, guest network structures, or redirected API capabilities. |

Prior `redir-role-review.md` already records actual static object/body correspondence and the unverified asynchronous shutdown/device boundaries. This report adds no duplicate runtime claim or restoration percentage.
