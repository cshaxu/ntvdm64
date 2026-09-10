# S5 Video and Patched SAS Recovery Limit

状态：静态复核完成，2026-08-09。

## 目的

本记录收束两条看似可以解除完整 `BIOS[]` 链接阻塞的线索：generic `Video` vector，以及
`c_sas_touch` / `c_VirtualiseInstruction`。结论是两者均不能作为未经证明的 SoftPC 恢复代码入口。

## `Video` 的真实 consumer 证据

固定 NT4 输入中的 generic `Video` 不是未使用的死 storage：

| 原始调用点 | 条件 | 观察 |
| --- | --- | --- |
| `base/video/ega_prts.c:1878-1883` | `CPU_40_STYLE` 的非 EVID 分支 | `init_vga_globals()` 使用 `SetMarkPointers(0)` |
| `base/video/gfx_updt.c:3617-3701` | 同一 profile 的图形 marking 状态切换 | 使用 `SetMarkPointers(0..3)`，即运行期也会重绑 mark-pointer 行为 |
| `base/video/ega_prts.c:2002`、`vga_prts.c:808` | EGA/VGA 初始化 | 进入 `init_vga_globals()` |
| `base/bios/reset.c:884` | 原始 reset 顺序 | 调用 `video_init()` |

这证明 vector 的生命周期至少包含 reset/初始化与运行期 marking 切换，不能用零表、单一
`C_Video` alias 或 no-op binding 通过链接。它也**没有**给出 5 槽 generic `VideoVector` 的历史
storage、函数地址或初始化 owner；完整 C-VIDC `C_Video` 是不同 layout。故第二个 consumer 只强化
“不可猜测”的结论，不满足恢复 initializer 的证据条件。

本地 NT4、NT4.5、NT5/XP 只读树与公开定向检索均未找到 `ccpu486.lib`、相匹配 archive member、link map
或 generator input。当前唯一可接受的后续输入仍是带版本/provenance 的历史 composition artifact。

## 两个 SAS 名称不是 OpenNT 缺失实现

`c_sas_touch` 与 `c_VirtualiseInstruction` 仅由 CCPU 的 `sascdef.c` 把它们置入 generated `SasVector`
的尾部 slots；没有任何原始 CCPU instruction source、BIOS、BOP、DEM、NTIO 或 NTDOS caller。相关 ABI
可见于 `base/ccpu386/sas4gen.h`。

名称本身来自后来的适配补丁：`refs/ntvdmx64-derived/ntvdmpatch/patches/common/fmstubs.c` 的实现会触发
`int 3`，随后返回 `NULL` 或零。这是明确的 fail-fast diagnostic，不是恢复输入，也没有定义 range、
mapping、IP、FLAGS 或 return encoding 语义。

因此：

1. 不得以 `c_sas_touch` 作为普通 guest pointer lookup 或写时 materialization；
2. 不得以 `c_VirtualiseInstruction` 建立任意 instruction/BOP interception；
3. 它们可以在 default-disabled link diagnostic 中维持 fail-closed guard，但不得进入 runtime；
4. 当前可恢复的 OpenNT CCPU 语义仍只有已单独验证的 `c_effective_addr`。

## 路线影响

完整历史 `BIOS[]` clean-link 的阻塞仍是外部历史 composition 输入和大量尚未动态到达的 host owner，
而不是这两个 SAS 槽。下一项高 ROI 工作应回到原始 reset/host composition 的动态可达性与 source owner
收集；不应为减少未解析符号而扩展 patch ABI、视频 table 或自写 BOP/DEM。
