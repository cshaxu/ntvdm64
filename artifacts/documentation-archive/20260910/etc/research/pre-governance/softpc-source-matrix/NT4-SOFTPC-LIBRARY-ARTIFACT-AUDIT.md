# NT4 SoftPC Library Artifact Audit

## 目的与结论

本记录审计当前本机可见的历史静态库，专门寻找此前缺失的 SoftPC/CCPU/C-VIDC
组合产物或其可审计的成员来源。结论是：在本记录的限定范围内，**未发现**
`ccpu486.lib`、等价的 CCPU/C-VIDC 组合库、或导出关键 CCPU/C-VIDC 符号的
COFF 静态库。这个结论不宣称未挂载介质、其他 build drop 或未采集版本不存在该
输入。

本次结果不授权下列行为：

- 把 OMF 旧库当成软件 CPU 的证据；
- 以 `Video = C_Video`、短表复用、假 handler、reduced `BIOS[]` 或
  `/FORCE:UNRESOLVED` 推进运行路径；
- 用 NT5/XP 的库替代 NT4 组合规则；
- 以本次 not-found 结果扩大任何 DOS、BIOS 或设备 shim。

## 固定审计范围

审计时间：2026-08-09。

递归枚举的根目录为：

- `D:\home\repos.hobby\opennt`
- `D:\home\repos.hobby\opennt-4.5`
- `D:\home\repos.hobby\win2000src\private`
- `D:\home\repos.hobby\winxpscodes`

先枚举了全部 4,074 个 `.lib`；为了避免 SDK/media 符号噪声，随后对路径或文件名
含 `mvdm`、`softpc`、`ccpu`、`cvid`、`ntvdm` 的候选作严格大小写的 COFF
符号筛选。检索的精确符号集合为：

```text
_c_cpu_init
_c_effective_addr
_c_sas_touch
_c_VirtualiseInstruction
_C_Video
_Video
_MS_bop_0
_init_vga
_ev_glue
```

工具为本机 LLVM 的 `llvm-nm --defined-only --extern-only`。只把工具能够解析为
COFF archive 的结果用于符号结论；无法解析的库另行归类，绝不按“空库”处理。

## COFF 结果

| 根目录 | 路径候选数 | 可解析 COFF | OMF 或不可解析 | 关键符号命中 |
|---|---:|---:|---:|---:|
| OpenNT | 33 | 1 | 32 | 0 |
| OpenNT 4.5 | 33 | 1 | 32 | 0 |
| Win2000 private | 0 | 0 | 0 | 0 |
| XP source tree | 129 | 55 | 74 | 0 |

因此，已解析的 MVDM/SoftPC 邻近库没有提供任何缺失的 CCPU/C-VIDC 定义；也没有
出现可作为 `ccpu486.lib` 替代物的 archive。此前已确认的 i386 `ntvdm.exe` 仍只可
用作 V86 host/BOP/DEM oracle，不能反推软件 CCPU 组合。

## OMF 结果与排除理由

OpenNT 与 OpenNT 4.5 的 32 个不可解析候选，全部落在以下类别：

- `base|private\mvdm\dos\v86\cmd\append` 的 `sysmac.lib`；
- `base|private\mvdm\dos\v86\lib` 的 `oldnames.lib`、`slibce.lib`、`xmm.lib`；
- `mvdm\wow16` 的 Win16/WOW16 UI、OLE、CRT、system 库。

XP 的 74 个不可解析候选同样属于 DOS V86 16 位库、WOW16 库，或
`mvdm\tools\c816` / `c932` 的历史工具链 CRT 库。没有一个位于
`softpc.new\base\ccpu386`、`softpc.new\base\cvidc`，也没有名为
`ccpu486.lib`、`ccpu386.lib` 或 `cvidc.lib` 的成员库。

所以“LLVM 不能读 OMF”在这里不是待补一个解析器的 CCPU 恢复路线：这些文件的路径、
命名和 16 位工具链归属已经将其与缺失的 32 位软件 monitor product 分开。若未来拿到
同版本的 OMF/COFF 转换记录、build map 或 member list，仍须按来源单独复审。

## 对路线的影响

1. 历史输入恢复路线仍保持开放，但下一类合格证据只能是 `ccpu486.lib`、可核验的
   member list/link map/listing，或 `host/genPg`/EDL 的原始生成输入。
2. 当前可继续的唯一代码恢复候选仍是有真实调用者证据的 `c_effective_addr`；
   `c_sas_touch` 与 `c_VirtualiseInstruction` 继续 fail-closed。
3. 完整 `BIOS[]` 的 C-VID 静态闭包仍是独立问题。它没有成为 NTIO/DOS 的动态
   video 依赖，也没有授权 host、设备或 DOS runtime 替代物。
4. 下一个有效的采证动作是围绕完整 `BIOS[]` 的原始 owner 输入继续建立可重放的
   静态闭包清单；不是继续扫无关 SDK 库，也不是提前实现外壳、mantle 或 core 对接。

## 关联记录

- `NT4-NT5-SOFTPC-EVIDENCE-LEDGER.md` 的 E-BUILD-005 至 E-BUILD-007；
- `NT4-CVID-PROVIDER-CLOSURE-AUDIT.md`；
- `R1-CCPU-CVIDC-COMPOSITION-RECOVERY-SPEC.md`；
- `S5-BIOS-STATIC-DYNAMIC-OWNER-LEDGER.md`。
