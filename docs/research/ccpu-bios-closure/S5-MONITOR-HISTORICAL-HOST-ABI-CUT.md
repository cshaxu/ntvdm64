# S5 Monitor / Historical Host ABI Cut

状态：source-level ABI 审计，2026-08-09。该文档定义下一轮 S5 研究可证明的
接口边界；它不准入新的 helper、host shim 或 runtime。

## 问题

当前 `historical_bios_bridge_v2.c` 通过下列 CCPU 专用对象进入原始
`BIOS[50h]`：

```text
c_cpu_init / c_cpu_terminate
c_set* / c_get*
ntdos64_ccpu_sm0_ram
sas_init / sas_term
```

这证明现有 trace fixture 的实现使用 CCPU/SAS 存储，但不能证明这些对象是
`MS_bop_0` 的历史协议要求。尤其是 NT4 x86 的产品配置是 `MONITOR`，不是
`CPU_40_STYLE + CCPU`；两者不得因当前 i386 编译探针方便而混为一个 runtime
profile。

## 原始 selector-50 的最小语义

原始 `base/mvdm/softpc.new/base/bios/bios.c` 在 `NTVDM` 构建下将
`BIOS[0x50]` 指向原始 `MS_bop_0`。后者在
`base/mvdm/softpc.new/host/src/nt_bop.c` 中严格按此顺序执行：

1. 以 `getCS():getIP()` 形成 real-mode `SEGOFF`；
2. 调用 `Sim32GetVDMPointer(..., 1, FALSE)` 读取一个 service byte；
3. 调用原始 `DemDispatch(DemCmd)`；
4. 以原始 `setIP(getIP() + 1)` 消费该 service byte；
5. 除 `14h/15h` 外，保留原始 `IDLE_disk()` 调用。

因此，`50h` 不是由 bridge 重新解码的 DOS SVC。bridge/monitor 只可在 CPU
已经识别到 BOP 后移交 selector；service-byte 读取、DEM 分派、IP 消费和 idle
判定都仍属于原始 owner。

## 两种已见 ABI profile

| 面 | 当前 CCPU probe profile | NT4 x86 historical profile | 对重建 monitor 的含义 |
| --- | --- | --- | --- |
| 寄存器 accessor | `cpu.h` 的 `c_get*` / `c_set*` | `softpc.h` 的 `VdmTib.VdmContext` 宏 | 需要一个有明确定宽 state backing 的 ABI facade；不能把 CCPU accessor 当作历史 x86 owner。 |
| pointer 转换 | `sim32.c` 的 `CPU_40_STYLE + CCPU` 分支最终调用 `c_GetLinAdd` | x86 `MONITOR` 使用 monitor 侧的 VDM address/context 约定 | 只需为已准入的 real-mode ordinary-memory aperture 提供 checked translation；不能宣称复原 V86 或 protected-mode mapping。 |
| CPU 生命周期 | `c_cpu_init/c_cpu_terminate` | kernel V86 monitor 生命周期 | 重建 monitor 必须拥有自己的 CPU/memory lifecycle；不可调用 V86，也不可将 CCPU 生命周期伪装为 NT4 x86 lifecycle。 |
| BOP/DEM 语义 | 原始 `MS_bop_0`、`DemDispatch` | 同一原始 owner | 必须保持原函数边界，不可本地替代。 |

`src/opennt` 当前 `ntdos64-opennt-nt-bop-objects` 的实际编译定义含
`CCPU`、`CPU_40_STYLE`，并不等于 NT4 x86 product build。这使它适合作为
函数级行为和符号依赖的采证输入，却不构成将 CCPU storage 接入重建 monitor 的
依据。

## 允许研究的窄 ABI facade

若后续要令 reconstructed monitor 调用原始 `BIOS[50h]`，facade 必须同时满足：

1. **state**：仅映射已存在 transport record 中的 real-mode 可见字段。第一轮
   只能包含 AX/BX/CX/DX、SI/DI/BP/SP、CS/DS/ES/SS、IP、FLAGS；不得接受
   CRx、descriptor cache、A20、IRQ 或任意 mode switch 写入。
2. **memory**：`Sim32GetVDMPointer` 等价入口只可检查并映射 monitor 已拥有的
   ordinary RAM aperture。地址、长度、segment:offset 溢出或映射种类不合法时，
   必须在原始 handler 之前 fail-closed；不得提供 ROM/device callback、V86
   alias 或 unrestricted host pointer。
3. **call**：facade 只能调用一次由 monitor 已识别的、固定 selector 的原始
   `BIOS[selector]`。它不得读取 service byte、分派 DEM、修正 IP 或合成 FLAGS。
4. **commit**：请求预检成功后，完整装入 state，调用一次原始 entry，再完整读回
   state；在进入原始 entry 前失败时不得改变 monitor state/aperture。进入后发生的
   DOS/host 副作用仍由历史 owner 处理，facade 不得伪造 rollback。
5. **lifetime**：facade state 与 memory backing 的建立、冻结、销毁必须由
   reconstructed monitor session 明确拥有；它不得依赖 CCPU globals、SM0 RAM、
   `VdmTib`、旧 kernel V86 context 或 host CRT object 穿越位宽边界。

这是 ABI adaptation，不是新 monitor instruction backend，也不是 DOS runtime。

## 尚未解决、且本 cut 不绕过的门槛

即使上述 facade 设计可被证明正确，它**不能**使当前 full `BIOS[]` helper 自动
可链接。`bios.c` 的完整 static initializer 仍对所有已选 entry 产生引用，并把
C-VIDC generic 81-slot `Video` provider、event/display/input/timer 等静态 owner
带入 link closure。

因此，下一次实现前必须先完成两项彼此独立的工作：

1. 对完整 `BIOS[]` 的 static owner closure 继续采证；generic `Video` 只能通过
   历史 composition artifact，或独立、profile-aware 的 reconstruction admission
   解决。不得使用 `Video = C_Video`、5-slot `ntstubs.c::Video`、缩减表或
   `/FORCE:UNRESOLVED`。
2. 针对 selector `50h` 的 reconstructed-monitor ABI facade 建立 compile-only
   contract 与正反 fixture，证明它没有复刻 `MS_bop_0` 的 service/IP 语义，也没有
   引入 CCPU/V86 ownership。

第 2 项只能在第 1 项不被绕过的前提下进行；它可减少 profile 错配，不是 static
link closure 的替代方案。

## 证据与后续顺序

- static/dynamic selector 与 owner 区分：
  `docs/research/ccpu-bios-closure/S5-BIOS-STATIC-DYNAMIC-OWNER-LEDGER.md`
- 当前 cross-bitness record 规则与 helper 限制：
  `docs/S5-CROSS-BITNESS-TRANSPORT-CUT.md`
- NT4 x86 `MONITOR` 与 CCPU profile 不可混用的依据：
  `docs/R1-CCPU-PROFILE-SEPARATION.md`
- generic `Video` composition 证据：
  `docs/research/softpc-source-matrix/NT4-CCPU-CVIDC-PRODUCT-REACHABILITY.md`

结论：S5 的正确下一步是先把“重建 monitor 提供的 mechanical ABI”与
“原始 historical-host 的 BOP/DEM 语义”在 source/fixture 层分离；不得把
CCPU probe bridge 升格为 runtime。完整 BIOS table 的静态闭包仍是独立硬门槛。
