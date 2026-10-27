# Kernel 3.1 guest source owner review

Scoped to pending kernel31 entries in shard 2; only original guest build file role is considered, no guest capability exclusion or proof of installed image provenance. No guest execution or modification and no product changes.

Rows 72; source-file-only candidates 27; pending 45; hash mismatches 0.

The 386P makefile target uses OBJ plus OBJ3 and krnl386.lnk, calls link16 and rc16 (lines 340–352), and compiles explicit dependencies by MASM or CL16 (tool definitions lines 179–185 and object-specific rules). `krnl386.lnk` actually names linked objects on lines 1–11 and kernel.exe target line 12. The CSV only promotes source named by an explicit selected object rule and present in both selected object group and 386 linker input; declarations require direct or transitive local include edges from such a source. Include/conditional paths are recorded but not evaluated as every profile.

Unverified binary/intermediate files, recipes, resource output, docs, and any unverified branch remain pending. `krnl386.res` is a build output; its existing binary is not established as the selected currently installed runtime resource. Source closure does not prove the installed immutable guest was built from these files or that any behavior passes.

SHA-256:
- `src/mvdm/wow16/kernel31/makefile` `5ddcc52bb834d447e75d85d5720ffa52968d63912a718a8769afb8555155e247`
- `src/mvdm/wow16/kernel31/krnl386.lnk` `94bd46f231b97748aea240b6b97e7828771b9cfc2ffbdb2b3e084a70beeb9cbe`
- `artifacts/mvdm-audit/shard-2/file-audit.csv` `286cb7084de84e86186acda4c7750532841ad60521baf99128e6146e459f5ff4`

## 286P additional original build evidence

`2ginterf.asm`, `2glru.asm`, and `2gmemini.asm` each have an explicit source→object make rule, membership in OBJ2, and corresponding object on original krnl286.lnk. `286P` selects OBJ2 and executes link16 (makefile:306–320); per-row exact line numbers are recorded in the CSV. This proves only historical 286 guest-build source roles, not 386 selection, current guest binary provenance, or an approved new execution profile. krnl286.lnk SHA-256 `c520ab3d01ce7f5be161126f53c8fa38f77b3b5c96ccb5847a48d7db5e956890`.
