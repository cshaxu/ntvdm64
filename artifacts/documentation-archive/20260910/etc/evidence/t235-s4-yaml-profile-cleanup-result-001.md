# T235 S4 fixed-YAML profile cleanup result

## Question

Can T235 remove the retired JSON `profile.json` admission mechanism without
changing the original OpenNT COMMAND/PIF path restored by S3?

## Change

- Removed `src/cli/byob_profile.c`, its JSON-only CTest, the obsolete Bochs
  argv relay, its probe and its policy fixture.
- Removed the excluded legacy runtime target that existed only to assemble the
  JSON profile parser with retired adapter-runtime fixture code.
- Removed the parser from the formal T225 full-module manifest. The product
  runner and native path now obtain their four component identities, target,
  `CONFIG.NT`, and `AUTOEXEC.NT` solely from sibling `ntvdmcfg.yaml`.
- Added the product-facing `ntdos64_startup_selection` spelling for YAML
  output. The retained underlying record layout remains available to older
  adapter evidence but is no longer a JSON parser contract.

The remaining tracked occurrences of `profile.json` are indexed historical
fixtures/tests and evidence only. They are not listed by the product CMake
targets or the formal module manifest. They are retained rather than rewritten
because historical evidence must remain auditable.

## Procedure and observations

1. Generated and built `D:\tmp\ntdos64-M0-T235-S4-formal-r1` with
   `New-T225S7FullNinjaGraph.ps1`; Ninja completed all 328 actions and linked
   `bin\ntdos64-native.exe` with no `byob_profile.c` object.
2. Ran the formal `t234-s2-command-pif-direct-fixture.exe`; it exited zero and
   reported original OpenNT PIF title/command/directory/target/close-on-exit
   plus early CONFIG/AUTOEXEC selection verification.
3. Generated and built `D:\tmp\ntdos64-M0-T235-S4-runner-r2` with
   `New-T235S1RunnerNinjaGraph.ps1`; the nine-action runner graph linked
   `ntvdm64-0235.exe` and `runner-engine-probe.exe`.
4. Ran `tests/ntdos64-run-policy.cmake` and
   `tests/ntdos64-run-s1-format-policy.cmake` against that runner; both CMake
   policy scripts exited zero. Their expected rejection diagnostics cover
   missing/invalid YAML, unsupported options and missing explicit engine.
5. Searched the product CMake path, CLI directory and formal module manifest:
   no `byob_profile.c`, `--byob-profile`, `--byob-root` or `profile.json`
   product-entry occurrence remains.

## Interpretation and follow-up

T235's product configuration is now fixed YAML only. This cleanup does not
claim a broader COMMAND lifecycle or guest-execution closure: PIF behavior
remains the original recovered `cmdpif.c`/`nt_pif.c` route verified above.
