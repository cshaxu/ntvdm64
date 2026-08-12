# S5 Shared Aperture Transport Admission

状态：2026-08-09。本文是对已有 S2 v1 bridge 和 shared-aperture 采证的边界校正；不新增 runtime、BOP、DEM、DOS 或设备实现。

## 已证实的两件不同事情

1. `shared_aperture_sas_host_v1.c` 已证明：原始 `sas_init(2 MiB)` 可以经其原有的
   `host_sas_init` allocation seam 使用命名 section view；`sas_term` 后由该 host seam 释放
   映射。x64 S4 monitor 随后能从同一 physical offset 取一条普通 `NOP`。这只是 backing 和
   生命周期采证。
2. `historical_bios_bridge_v1.c` 已证明：原始 `BIOS[50h] -> MS_bop_0 -> DemDispatch(14h)`
   date trace 可以经一个 i386 fixture 调用。该 fixture 自己执行 `sas_init`、`c_cpu_init`，并把
   service byte 写进 `ntdos64_ccpu_sm0_ram()`。

这两条结果不能相加为“S4 已调用历史 BIOS”。v1 bridge 没有接受外部 aperture，也没有接受
外部 SAS 生命周期；它使用的 RAM 所有权与 shared-aperture fixture 不同。

## v1 的永久限制

`historical_bios_bridge_v1` 必须保持下列约束：

- 只用于已存在的 `50h/14h` bounded date trace；
- 不能增加 selector、service byte、寄存器字段或 flags；
- 不能作为 S4 的 callback，不能接入 `ntdos64-run`、默认 CTest 或研究 runtime；
- 不能以“同样调用了 `BIOS[50h]`”作为 shared backing、原始 DOS init 或完整 CCPU 闭包的证据。

它是 `trace/fixtures`，不是 `compat/host` 的候选实现。

## 后续 transport 的准入合同

只有同时满足以下条件，才允许新建一个仍为 default-disabled 的 S5 transport fixture：

1. **单一 backing**：i386 历史侧的 `Start_of_M_area` 必须由原始 `sas_init` 经
   `host_sas_init` 取得，并且等于 x64 S4 monitor 收到的 section view。禁止 copy-in、copy-out
   或第二块 SAS RAM。
2. **历史生命周期 owner**：transport 不得调用 `sas_init`、`sas_term`、`c_cpu_init`、
   `c_cpu_terminate`。这些仍由一个明确、可审计的历史 session owner 调用；若尚无该 owner，
   则 transport 不可实现。
3. **窄而版本化的状态 record**：record 只携带已由 NTDOS caller 证明需要的 real-mode register
   snapshot、selector、disposition 和固定宽度状态 patch。禁止 host pointer、CRT ownership、C++
   object、任意 flags 或任意 mode switch。
4. **原子性**：i386 侧必须先验证完整 request、调用原始 `BIOS[selector]`，再生成完整 response；
   x64 S4 侧必须先验证完整 response，随后一次提交允许字段。失败、timeout 或不支持时 guest
   state 与 guest memory 均不变，且停止。
5. **NT4 IP 规则**：对 `50h`，call-in 时 IP 指向 service byte；`MS_bop_0` 返回后由其自身消费
   该 byte。transport 和 S4 都不得提前或重复消费它。
6. **不改写 guest 指令/服务**：transport 不得写入 service byte、构造 IVT/BDA、伪造 BIOS 返回、
   重定义 DEM/SVC 或代替 PC/AT handler。
7. **逐服务准入**：首个候选只能复现已有 caller-first trace 的一个服务，并附带同一 backing 的
   pre/post memory assertion；一个服务的成功不准入其它 service。

## 当前结论与下一步

目前仅满足第 1 条的 backing/lifecycle 基础采证，尚无满足第 2 条的可恢复历史 session owner；
因此不能把 v1 改造成 shared transport，也不能开始串接 `0F -> 1B -> 32`。

下一项高 ROI 工作是静态恢复该 session owner 的真实入口与责任边界，或获得完整的历史 CCPU
composition input。若两者都不可得，S4 保持为 software-monitor mechanics，原始 BIOS/DEM
fixture 继续分别作为只读行为 oracle，而不是用自写 glue 把它们硬拼起来。
