# S5 Neutral Machine Requirements Update

状态：2026-08-09。本文将近期 NT4 historical-source evidence 转成对 future core/mantle 的**消费者需求**。
它不修改 `ntvdm64`，不提出 API 实现，也不把 DOS、BOP、SVC、`NTDOS.SYS` 名称并入 core。

## 证实的中性需求

| 中性能力 | 历史证据 | 所在层的含义 |
| --- | --- | --- |
| 一致的可访问 guest RAM 容量 | NT4 `memory_size()` 从 BDA `MEMORY_VAR` 返回；reset 写入该 value | core/machine 必须能让 profile 在 reset 边界确定 RAM 布局，并让受检查 memory access 与该布局一致。 |
| profile-owned machine initialization bytes | NT4 `15h/AH=88h` 经 CMOS extended-memory bytes 返回容量 | mantle/profile 应拥有 BDA/CMOS/ROM 等 initial-state 组合；core 只提供受约束的映射与 reset application。 |
| 指令边界的精确可观察性 | NT4 `MS_bop_0/2/4` 在原 dispatcher 返回后才消费 trailing byte；XP 不同 | core execution/transition 需要能报告并原子地应用受限结果；不能由 handler 任意改 mode、IP 或 FLAGS。实际 historical byte semantics 属于 mantle/adapter。 |
| checked ordinary-memory aperture | `MS_bop_*` 从当前 `CS:IP` 取一 byte；`demLoadDos` 向 live `DI:0` 写入 | core 必须让 consumer 在明确地址、长度和权限下读/写普通 RAM；不应让 consumer 获得任意 host pointer、ROM/device callback 或隐式 A20 绕过。 |
| 生命周期与 failure boundary | 历史 DOS load 成功后仍可能在 CONFIG projection 停止；不能合成继续成功 | product/session 控制必须能表示 structured stop、原因分类和 redacted diagnostics；这不等于 core 知道 DOS/CONFIG。 |

## 明确不属于 core 的内容

- BOP selector、service byte、`BIOS[]` table、DEM/CMD dispatch、DOS drive letter、CONFIG/COMMAND policy；
- BYOB 文件名、hash profile、directory/temporary-file strategy、Win32/Win64 executable handoff；
- 固定 640 KB、CMOS register layout、PC/AT device topology、A:/B: 介质选择。

这些是 mantle/profile 或外部 adapter 组合出来的 consumer policy。core 的契约只应确保它们无法越过
checked memory、reset boundary、transition disposition 与 host capability isolation。

## 当前不能据此要求 core 做的事

1. 不能因为 `BOP 12h/15h` 出现，就把完整 PC/AT BIOS/CMOS/FDC/HDC 下沉 core。
2. 不能因为 XP 先推进 IP，就把该顺序设计成 generic transition 默认行为；固定 NT4 consumer 需要
   post-dispatch consumption。
3. 不能以现有 CCPU/C-VIDC 缺失 composition 为由，让 core 模拟或提供历史 video/BIOS owner。
4. 不能用 CONFIG projection、drive scan 或 host process launch 证明 core machine backend 已足够。

## 交付给未来设计的最小验收问题

在 core/mantle 设计评审时，对每个候选接口只问以下问题：

1. 它是所有 machine consumer 都需要的受约束能力，还是某个 profile 的 DOS/PC-AT 策略？
2. 它能否在 reset 后、执行前被完整验证并原子提交？
3. 它对 memory/IP/FLAGS/mode 的写入范围是否可枚举、由 core 验证，且失败不留下半提交状态？
4. 它是否能在不暴露 guest bytes 或 host paths 的情况下产生 redacted diagnostic？

若答案依赖 `BOP 50h`、`COMMAND.COM`、`CONFIG.SYS`、具体 CMOS register 或 Windows 安装目录，该接口
不应进入 core；应留给 mantle profile 或外部 adapter 的版本锁定实现。

## 关联证据

- `NT4-NT5-BOP-ORDERING-DIFF.md`
- `S5-SYSINIT-MEMORY-BOP-BOUNDARY.md`
- `S5-HOST-SEAM-ADMISSION-SNAPSHOT.md`
- `docs/research/ccpu-bios-closure/S5-MONITOR-HISTORICAL-HOST-ABI-CUT.md`
