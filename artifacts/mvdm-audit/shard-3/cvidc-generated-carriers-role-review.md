# S10 shard 3: C-VID generated and historical configuration carriers

Seven pending `softpc.new/base/cvidc` files are grouped by original generated/profile role. `cvidc-generated-carriers-observer.py` compares SHA-256 with frozen OpenNT/OpenNT 4.5 inventory and enumerates identical named copies; all seven match both originals. The retained selected-worker dependency index has zero exact hits for these seven paths. No production/guest changes or builds were made.

| File | Role and decision boundary |
|---|---|
| `acc_efi.h` | Large generated access-table initializer. Identical copies exist under original `base/inc` and MIPS/prod; zero exact C-VID path hit does **not** prove the table or CPU access semantics absent. Keep pending until selected table consumer/profile is traced. |
| `axphst_c.h` | Alpha host register/calling and FP exception constants (`AXP_*`). No exact selected dependency hit or matching named copy. Candidate alternate architecture header role, but confirm selection/translation before exclusion of file path; do not infer no x86 FP capability. |
| `bpi_c.h` | Generated BPI instruction enum; exact duplicate under MIPS/prod. No observed selected dependency for this path. Keep generated-provider/semantic use pending. |
| `c2cpusad.h` | Generated CPU-vector **name strings**, not implementation; other architecture files of same name exist with different bytes. No observed selected header dependency. Candidate diagnostic name carrier; confirm alternative generation before file-role conclusion. |
| `ccpudefs.inc` | Historical `base/cvidc/sources:91` includes it as build configuration; it defines CPU_30_STYLE and CPU_40_STYLE, NEW_CPU, CCPU, SPC386, ANSI, NTVDM, C_VID. Current worker Ninja expresses related flags directly, including CPU_40_STYLE/SPC386 but not an explicit CPU_30_STYLE. These are profile descriptions, not a proof the preprocessing conditions are identical. Historical build-control role; actual CPU behavior remains separately covered. |
| `parser.h` | Generated operand/dispatch numeric constants. No exact selected dependency hit, but path-based absence cannot rule out different generated files or indirect assembly consumers. Keep pending. |
| `trace_c.h` | Trace bit enum and ring record shape; no exact selected dependency hit. Could be debug/profile-only, but unexamined generated consumers prevent definitive exclusion. |

Selected-path checks refer to `header-inclusions.csv`'s normalized VALID Ninja dependencies, which may omit generated/assembly and other profiles. Original `sources`/current graph and identical file bytes are evidence of roles only, not original-to-current semantic equivalence or behavioral percentages. Any partial or excluded decision must be limited to the exact file/profile; original CPU, video, diagnostic and lifecycle obligations remain independent.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/cvidc-generated-carriers-observer.py
rg -n 'acc_efi\.h|axphst_c\.h|bpi_c\.h|c2cpusad\.h|ccpudefs\.inc|parser\.h|trace_c\.h' artifacts/mvdm-audit/shard-3/header-inclusions.csv
rg -n 'ccpudefs\.inc' src/mvdm/softpc.new/base/cvidc/sources src/mvdm/softpc.new/base/ccpu386/sources
Get-Content src/mvdm/softpc.new/base/cvidc/ccpudefs.inc
rg -n '^cflags\s*=' build/.ninja/msvc-x86-release/worker/build.ninja
```
