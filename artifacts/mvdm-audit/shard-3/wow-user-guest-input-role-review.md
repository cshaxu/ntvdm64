# S10 shard 3: WOW16 USER source and build-tool family

Seven pending files from `wow16/user` are grouped by original USER build. `wow-user-guest-input-observer.py` / JSON captures per-file current and original inventory hashes plus installed USER.EXE hash. No guest build or new control interaction test. This complements shard-1 `guest-user-historical-review.md` and shard-3 `wow-usercli-role-review.md`.

| File | Evidence and scoped classification |
|---|---|
| `editml.c` | Original multiline EDIT implementation; references `MLSizeHandler` / `MLSetTextHandler` from edmlonce. The inspected WOW16 USER `makefile` object groups do not select `editml.obj`. Historical alternate guest source role for that recipe only; do not infer EDIT is absent or has equivalent behavior elsewhere. |
| `edslrare.c` | Original single-line EDIT rare routines (selection etc.); the same observed WOW makefile object groups do not select `edslrare.obj`. Historical alternate guest source role; functional coverage belongs to actual selected control owner. |
| `msutil.c` | Contains `GetCharDimensions`, mentioned as used by edit controls; inspected object groups do not select `msutil.obj`. Historical source role; determine selected dimensions provider before functional conclusions. |
| `msglayer.inc` | Included by `msglayer.asm`, explicitly in `msglayer.obj` makefile dependency and object list; provides message-validation constants/macros. Guest build interface, not proof each message is validated or source produced deployed USER. |
| `wowasm.sed` | Historical assembly conversion rules used by `wowasm.cmd`, `wowasmc.cmd` and `wowasmk.cmd`. Generated BUILD path/calling profile not established; preserve as build-time transformation input, not runtime message provider. |
| `wowasmc.cmd` | Historical script applies the sed transformation and retries assembly around jump-range diagnostics in its named output. A script invocation is not observed in the inspected USER makefile; alternate build invocation remains unverified. Historical build helper only. |
| `k/daytona/USER.EXE` | Retained hash `6746088F2A87CD366E4FF5DA424B39B5BD91586D5F7570AFBF3C05BCA94E935C` matches installed system32 and approved guest manifest. Partial guest-image identity; original-edition and source-to-binary lineage are as captured in JSON, and no USER feature is proven solely by the hash. |

The original C files may be classified as inactive source roles in this specific historical build, but EDIT behavior, message conversion, callback lifetime and control failure remain required capabilities. Source and build evidence does not prove the installed image's producer or current runtime path. The project forbids guest rebuilding; no feature percentage follows from this audit.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/wow-user-guest-input-observer.py
Get-Content src/mvdm/wow16/user/makefile
rg -n 'MLSizeHandler|MLSetTextHandler|GetCharDimensions' src/mvdm/wow16/user/editml.c src/mvdm/wow16/user/msutil.c
rg -n 'msglayer\.inc|wowasm\.sed' src/mvdm/wow16/user
rg -n -i 'WINNT/SYSTEM32/USER\.EXE' assets/winnt-guest-manifest.json
```
