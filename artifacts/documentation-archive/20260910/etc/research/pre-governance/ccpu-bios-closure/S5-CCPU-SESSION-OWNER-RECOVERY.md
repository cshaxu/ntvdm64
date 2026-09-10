# S5 CCPU Session Owner Recovery

状态：2026-08-09。只读源代码采证。目标是判断是否存在可用于 S4/shared-aperture 的**历史 CCPU
session owner**，而不是创造新的 `sas_init/c_cpu_init` lifecycle adapter。

## 找到的历史顺序

NT4 `base/support/main.c` 的公共 SoftPC 启动顺序是：

```text
init_virtual_drivers()
  -> config()
  -> cmos_pickup()
  -> cpu_init()
```

其中 `host/src/config.c:531` 在完成历史 configuration/PIF/内存尺寸决策后调用：

```c
sas_init(vdmMemorySize * ONEKB);
```

仅在 `CPU_40_STYLE` 条件下，紧接着调用 `InitNtCpuInfo()`；随后 `main.c:246` 才以 profile
宏调用 `cpu_init()`。CCPU profile 的 `cpu4gen.h` 将该宏映射为 `c_cpu_init`，而
`base/ccpu386/ntstubs.c:56-58` 的 `a3_cpu_init()` 也只是直接转发到 `c_cpu_init()`。

这说明 SAS -> CCPU 的历史顺序本身并不模糊：configuration 决定 VM memory size，原始 SAS
取得 backing，CPU 初始化在其后发生。它同时说明 `historical_bios_bridge_v1` 的手工
`sas_init(2 MiB); c_cpu_init();` 不是这个历史 session owner。

## 为什么它不能直接用作 NT4 x86 的 owner

NT4 `obj.vdm/sources` 的 product 选择把 x86 与 CCPU 分成互斥分支：

```text
x86:       MONITOR + C_VID + X86GFX
non-x86:   CPU_40_STYLE + CCPU + C_VID
```

因此上述 `CPU_40_STYLE` 的 configuration/CCPU 初始化顺序是 software profile 的原始证据，
但不是 NT4 i386 V86 product 的可执行 session owner。把它直接套到 S4/x64 会有两个越界：

1. `config()` 读取 PIF、console、环境和历史 host-global state；当前没有可准入的 contained
   session projection 来替代这整段输入图。
2. 它依赖缺失的 `ccpu486.lib` composite 与其生成的 vector/profile 初始化；现有 `ccpu386`
   C 文件或 V86 `ntvdm.exe` 都不能代替。

故不能以“源码有调用顺序”为理由，在本项目新建一个 `config -> sas_init -> c_cpu_init` helper，
也不能让 S4 transport 自己承担它。

## 对 shared-aperture transport 的结论

`S5-SHARED-APERTURE-TRANSPORT-ADMISSION.md` 的第二道门仍未满足：没有一条可在当前 x64
研究路径上重放、同时保持 NT4 historical ownership 的 CCPU session lifecycle。

目前允许保留的证据为：

- 原始 SAS 可以经 `host_sas_init` 使用 named section；
- S4 可以在生命周期后从同一 backing 取普通指令；
- 原始 BIOS/BOP/DEM services 可在各自 bounded i386 trace fixture 中观察。

目前不允许的动作：把三者放进一个自写 owner、让 transport 调 `sas_init/c_cpu_init`、借 V86
product 的 `MONITOR` 代码充当 software CCPU，或用任何 no-op host configuration 让链继续。

## 下一项可执行高 ROI 工作

优先级不是再扩 S4 opcode 或服务数量，而是取得并审计版本匹配的 `ccpu486.lib` member list/map
或 EDL/generator input；它们是能把 software profile 的历史 composition 与 lifecycle 实际闭合的
唯一新增证据类别。若没有该输入，S4 继续作为独立、fail-closed 的 CPU mechanics，而原始 host
fixtures继续作为 caller-first oracle。
