# R2 SAS Reset Profile Audit

状态：原始 owner 审计，2026-08-09。

## 结论

OpenNT 提供了可采证的 SAS/reset 语义，但没有一条可直接搬入 x64 自研 R2 的“原样历史
software monitor profile”。当前可执行 selector-50 trace 使用的 `calloc` aperture 仍是归档
fixture；它不能成为 runtime。反过来，直接选取 `sim32.c` 的任一历史 allocator 分支也不能
解决此问题：两支分别属于不同的历史平台组合。

因此 R2 的正式方向是：以这些原始 owner 为语义 oracle，独立实现受检查 memory/ROM/A20/reset
machine state；历史 i386 闭包只保留为可重复 trace/差分参照，不能作为 x64 pointer provider。

## 原始生命周期

`ccpu386/ccpusas4.c:sas_init()` 的顺序是：

```text
SasSetPointers(cSasPtrs)
-> allocate 64 KiB scratch
-> host_sas_init(size)
-> allocate page-granular memory_type map
-> map [0, size) as SAS_RAM
-> set Length_of_M_area / CCPU_M
-> rom_init()
-> copyROM()
```

`sas_term()` 则先调用 `host_sas_term()`，再根据该函数返回值决定由哪一侧释放 backing
allocation 与 `memory_type`。这说明 allocator 的所有权和 cleanup 返回约定不可拆开复制。
`c_cpu_init()` 随后调用 `c_cpu_reset()`：设置 reset CS:EIP (`F000:FFF0`)、segments、flags、
control/debug/descriptor state 与 CPU ID；它本身不建立 SAS。

## 两个历史 allocator 分支

| 分支 | 原始 owner | 行为 | 对 R2 的意义 |
| --- | --- | --- | --- |
| 非 `CPU_40_STYLE` | `host/src/sim32.c:325-445` | 预留固定 virtual-memory aperture，再提交当前 SAS 和 page-type 区域 | 对应 x86/V86-era host 组合；可描述连续 aperture 与 resize 语义，但不能当成 R2 的 V86 运行方案。 |
| `CPU_40_STYLE` | `host/src/sim32.c:426-455` + `host/src/nt_mem.c` | `InitIntelMemory(size)` 预留 Intel address space、先提交 1 MiB+A20 wrap、维护分配 header table | 对应非 x86 CCPU/RISC profile；依赖 GDP/生成布局和 RISC host assumptions，不能宣称为 NT4 x86 原始 profile。 |

`obj.vdm/cdefine.inc:34-53` 进一步证实该分叉：原始 386 产品使用 `MONITOR + C_VID + X86GFX`；
`CPU_40_STYLE + CCPU + C_VID + SIM32` 是 non-x86 路径。因此，以 i386 编译这些 C sources 并不能
把后者变成历史 x86 CCPU build；它只能提供语义采证。

## 本轮可执行证据

独立 `clang-cl --target=i686-pc-windows-msvc` Ninja island 已完成：

1. `original-selector50-trace-fixture` 构建、运行，退出 0，且为 `COFF-i386`。
2. `direct-ccpu-full-source-closure-fixture` 已越过所有对象编译并到达链接阶段。
3. `original-sas-lifecycle-trace-fixture` 已以 `clang-cl --target=i686-pc-windows-msvc`
   构建并运行，退出 `0`，产物为 `COFF-i386`。它使原始 `sim32.c + nt_mem.c` 成为
   `host_sas_init/host_sas_term` 的唯一 owner，并通过 `ccpusas4.c:sas_init -> rom_init ->
   sas_term` 验证 2 MiB SAS、ordinary-RAM write/read、无未准入 callback 和 teardown。
   该 target 是默认关闭的 trace fixture，未执行 CPU、BIOS、BOP、DEM、设备或 DOS。
   为链接同一原始对象中未调用的 `selector_outside_table`、`read_descriptor` 和
   `VdmSetPhysRecStructs` 引入，保留 `/FORCE:UNRESOLVED` 作为诊断；没有为它们添加
   shim 或替代实现。

第二项的未解析符号不是一个 allocator 小缺口，而分为四组：

- **CCPU 未恢复 vector**：`c_sas_touch`、`c_VirtualiseInstruction`、`EDL_fast_bop`；
- **C-VIDC/generated vector**：大量 `getVideo*`/`setVideo*` entry；
- **历史 NT VDM/private host contract**：`NtVdmControl`、`VdmSetPhysRecStructs`、console VDM APIs、
  native RTL exports、VDM command/session state；
- **DPMI/debug/EMS and host facilities**：register/context bridge、fast-BOP address、keyboard,
  timer/quick-event、LIM write path、debugger and console display owners。

因此完整 link failure 不能用来授权新增“万能 shim”；它正好说明正式 bridge 必须从已经证实的
最小调用面逐项扩展。

## 对下一个 fixture 的约束

下一项只可建立 `sas_init -> rom_init -> host_sas_init/term` 的 trace fixture：

- 只使用原始 `ccpusas4.c` 与选定的 `sim32.c + nt_mem.c` owner；
- 不调用 `c_cpu_simulate`、BIOS selector、BOP、DEM 或 DOS；
- 验证 allocation、page map、ROM mapping 后的 ordinary-RAM write 和 teardown；
- 若需要遗留 foundation，只允许它失去 SAS allocation ownership，并继续把未准入 callback
  fail-closed；该 fixture 仍不得进入 runtime；
- 未来 R2 只能吸收经此 fixture 确认的中性语义，例如 allocation ownership、mapping priority、
  ROM immutability、A20/reset order，不能继承 host pointer 或 V86/private API。
