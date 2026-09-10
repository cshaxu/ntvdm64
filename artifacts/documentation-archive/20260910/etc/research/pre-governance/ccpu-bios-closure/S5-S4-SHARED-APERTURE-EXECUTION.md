# S5 S4 Shared Aperture Execution

状态：raw backing、原始 SAS lifecycle 与 lifecycle 后 S4 普通实模式取指采证通过，2026-08-09。

## 目标与边界

本次只验证 x64 与 i386 可以共享同一个按固定物理 offset 寻址的 section backing。它没有
调用 `sas_init`、CCPU、BIOS、BOP、DEM 或 DOS，也不加载任何 guest 二进制。

```text
x64 host fixture
  CreateFileMapping + MapViewOfFile
  -> launch i386 helper with mapping name and byte count
  <- validate helper's fixed-offset write
```

i386 helper 只以名称 `OpenFileMapping + MapViewOfFile`。两端共享
`shared_aperture_v1.h` 的整数 offset；record、command line 和 section 内容均不包含
host pointer、handle value、DOS 路径或 DOS binary bytes。

## 实测结果

构建：

```text
cmake -S . -B artifacts/build/current/default-mingw-verify
cmake --build artifacts/build/current/default-mingw-verify \
  --target ntdos64-shared-aperture-v1-host-fixture --parallel 8

cmake -S src/opennt -B artifacts/build/current/opennt-r2-bridge-ninja-i686
cmake --build artifacts/build/current/opennt-r2-bridge-ninja-i686 \
  --target ntdos64-opennt-shared-aperture-v1-helper --parallel 8
```

2026-08-09，x64 fixture 调用 i386 helper 后退出 `0`。`llvm-readobj` 确认 helper 为：

```text
AddressSize: 32bit
Machine: IMAGE_FILE_MACHINE_I386 (0x14C)
```

测试验证 host 写入 header、size 与 probe word；i386 helper 验证它们后写入自己的 probe
word 与 ready word；x64 进程在 helper 退出后看到这些写入。它证明同 backing 的 raw memory
在当前工具链下可行。

## SAS lifecycle 结果

第二个 default-disabled i386 helper 仅在原始 host-allocation seam 接入 section view：它先
调用 `ntdos64_shared_aperture_sas_v1_prepare`，再调用原始 `sas_init(2 MiB)`。原始
`ccpusas4.c` 将 adapter 返回的 view 保存为 `Start_of_M_area`，继续原始 ROM 初始化；
helper 确认 `ntdos64_ccpu_sm0_ram() == shared view` 与 `c_sas_memory_size() == 2 MiB`，在
该 view 写入一个 byte，随后调用原始 `sas_term()`。测试确认历史侧 `Start_of_M_area` 与
adapter view 都已失效，而 x64 creator 仍可看到 helper 写入。

`c_effective_addr` 的 fixture-only 编译变体在 real mode 保持原有段地址计算，在 PE/VM
时返回失败值而不尝试猜测缺失的 selector ABI。此限制只用于让 `rom.c` 的链接闭包在本
lifecycle 采证中保持 fail-closed；它不改变一般 reconstruction target。

## lifecycle 后 S4 取指结果

独立的 x64 fixture 创建同样的 2 MiB section，在 physical `0x1000` 写入 `NOP`，启动上述
i386 SAS helper 并等待它完成原始 `sas_init` / `sas_term`。随后 fixture 将仍由 x64 持有的
同一 view 交给 S4 monitor，以 `CS:IP = 0100:0000`、instruction budget `1` 执行。结果为
`IP = 0001`，且 `0x1000` 的 byte 未被 SAS lifecycle 改写。

这证明 S4 fetch 与原始 SAS owner 可以在同一 session backing 的相同 physical offset 上连续
工作；它没有执行 BOP、BIOS、DEM 或 DOS service，也没有把历史 CPU state 交给 i386 helper。

## 仍未证明事项

- 没有执行历史 BOP、BIOS、DEM 或 DOS service；
- 没有已准入的 transport request、CPU state transaction 或 BOP/DEM integration；
- 没有证明 i386 historical host 的寄存器变化可由 S4 原子接收。

下一门槛是定义并审计一个 versioned、fixed-width 的跨位数 invocation record：它只能携带
受限实模式寄存器快照、selector 与结果 disposition，先完整校验再原子提交。该 record 必须
调用原始 `BIOS[selector]`，不得成为本地 BOP/DEM dispatcher 或 service replacement。在此之前，
本结果仍不能用作 S4 到 `BIOS[50h]` 已连通的证据。
