# S10 shard 3: DOS keyboard layout variants — selection and semantic boundary

This follows `dos-device-locale-role-review.md` on six pending original source files: `kdfbl097.asm`, `kdfbl098.asm`, `kdffr.asm`, `kdfru.asm`, `kdfuk168.asm`, `kdfur094.asm`. Original/current hashes are in that report and in the shard ledger. Read-only analysis; no guest rebuild or input test.

`keyboard.lnk` links KDFNOW plus an explicit layout list. `kdfnow.asm` describes and indexes that NT DOS set; it names the selected French `KDFFR189`, English `KDFUK166`, and Russian `KDFRU091`. It does not follow that the unselected source variants are semantically identical or that every language/layout is supported. Source comments in multiple files incorrectly say Spanish or Russia, so labels alone are not evidence.

| Pending inputs | Observable distinctions | Conclusion |
|---|---|---|
| `kdfbl097`, `kdfbl098` | Export `BL1_*` and `BL2_*` respectively (lines 19-23); neither object appears in selected response list. | Different public translation identities; keep locale-specific compatibility pending, not proof of equivalent substitution or general keyboard failure. |
| `kdffr` | Exports `FR_*`; selected `kdffr189` exports `FR2_*`. | Distinct variants, both code tables require comparison and guest layout selection tests before any semantic-coverage conclusion. |
| `kdfru` | Exports `RU1_LOGIC`, `RU2_LOGIC`, `RU3_LOGIC`; selected `kdfru091` exports `RU1_LOGIC` and variant-specific translation symbols. | Cannot collapse multi-layout file into selected single variant based solely on language label. |
| `kdfuk168` | Exports `UK1_*`; selected `kdfuk166` exports `UK2_*`. | Distinct layout identity; no claim of interchangeability. |
| `kdfur094` | Exports `UR1_*`; no UR object in the selected link list. | No semantic replacement or unsupported-language claim follows from the inspected recipe. |

The retained `KEYBOARD.SYS` has archived/installed exact-hash evidence in the earlier report, but this does not bind its creation to the inspected recipe or prove locale behavior. File role can be classified as retained historical guest-build source; affected variant behavior and selection remain **pending**, with no inferred restored percentage. The available source evidence does not demonstrate a product bug.

Replay:

```powershell
Get-Content src/mvdm/dos/v86/dev/keyboard/keyboard.lnk
rg -n 'KDFFR189|KDFRU091|KDFUK166|FR2_LANG_ENT|UK2_LANG_ENT' src/mvdm/dos/v86/dev/keyboard/kdfnow.asm src/mvdm/dos/v86/dev/keyboard/makefile
$names=@('kdfbl097','kdfbl098','kdffr','kdfru','kdfuk168','kdfur094','kdffr189','kdfru091','kdfuk166')
foreach($name in $names) { Select-String -Path "src/mvdm/dos/v86/dev/keyboard/$name.asm" -Pattern '^\s*PUBLIC\s' | Select-Object -First 4 }
```
