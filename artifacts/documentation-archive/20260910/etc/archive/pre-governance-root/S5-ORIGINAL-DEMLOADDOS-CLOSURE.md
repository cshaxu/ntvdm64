# S5 原始 DEM NTDOS 加载闭包

状态：原始 BYOB `NTDOS.SYS` 加载已闭合；后续 host dependency 尚待逐项采证；尚未准入 runtime。

## 目标

确认 NTIO 的 `0070:0475 C4 C4 50 11` 所到达的历史职责链是否会由原始
`MS_bop_0 -> DemDispatch -> demLoadDos` 加载 BYOB `NTDOS.SYS`，并将失败归属到
原始 owner 或现代宿主接缝。不得用自写 DOS loader、BOP handler 或 SVC handler
替代任何一步。

## 已证实路径

`ntdos64-opennt-original-demload-dos-byob-fixture` 是 default-disabled i386
fixture。它在原始 `DemInit` 后只替换 `pszDefaultDOSDirectory` 为命令行给出的
BYOB 目录，然后调用原始 `BIOS[50h]`。路径为：

```text
BIOS[50h] -> MS_bop_0 -> DemDispatch(11h) -> demLoadDos
```

历史 `DemInit` 首次读取 `VDMForWOW`。其原 owner 是
`base/mvdm/v86/scaffold/i386/softpc.c`；完整 scaffold 会同时带入入口、IVT/BDA
与 PC/AT 启动编排，因而不能导入。`historical_vdm_session_context_v1.c` 仅提供
原符号的 non-WOW `FALSE` 存储，不提供任何 scaffold 行为。

## 运行证据

以本仓库历史工具链构建的 BYOB `NTDOS.SYS` 目录作为输入后：

1. 原始 `DemInit` 成功返回。
2. 原始 `BIOS[50h]` 已进入 selector-50/service-11。
3. direct diagnostic 证明原始 `IsDebuggee()` 在现代进程返回 `0`。
4. 原始 `CreateFileOem` 的 NT4 私有 TEB 访问已由受限 F1 modern-host facade 替代。
5. direct diagnostic 的原始 `demLoadDos` 成功返回；其 SAS `0008:B000` 目标首三字节为
   `E9 A9 51`，与 BYOB `NTDOS.SYS` 一致。
6. 完整 fixture 的原始 `BIOS[50h] -> MS_bop_0 -> DemDispatch(11h) -> demLoadDos`
   成功返回；没有自写 loader、BOP 或 SVC handler 参与。

直接原因可由 `base/mvdm/oemuni/file.c:CreateFileOem` 证实：它把转换缓冲区取自
`NtCurrentTeb()->StaticUnicodeString`。这是 NT4 私有 TEB 布局假设，在现代
Windows 上不是可用 contract；并非 DEM、BOP、DOS 或 SoftPC CPU 语义的缺失。

## 已准入的窄接缝与后续规则

F1 `CreateFileOem` modern-host facade 已按原函数名、参数、OEM 到 Unicode 转换、两次
`CreateFileW` 访问策略、返回值和 `LastError` 语义接入。它以显式 resource root 限制
`NTDOS.SYS` 读取，且不读取现代 TEB 的 NT4 偏移。此项只解决已经由 `demLoadDos` 证明的
host filesystem seam，不构成完整 OEM facade。

为让 DEM 的历史 x86 `GetVDMAddr` 宏在 CCPU/SAS fixture profile 中指向已分配的 SAS，新增
`overlay/base/mvdm/inc/softpc.h`。它只在 `NTDOS64_CCPU_SAS_GETVDMADDR` 编译定义下将
`segment:offset` 交给原 CCPU `c_GetLinAdd`；默认 profile 仍为原 OpenNT 宏。它不实现 CPU、
DOS、BOP 或设备语义，且只由两个 S5 fixture 显式链接。

完整的 29-entry facade 范围、禁止复用的 archived implementation 与分组验收
顺序见 `SH-11-MODERN-OEM-FACADE-RECONSTRUCTION-WORK-ORDER.md`。不能把这项
工作误缩为单个 `CreateFileOem` replacement。

它必须：

- 位于 `local/compat/host`，有独立 manifest 与 ledger 条目；
- 只由已抵达的原始 `demLoadDos`/其他原始 OEM caller 使用；
- 不改写 `demLoadDos`，不实现 DOS/BOP/SVC/设备语义；
- 已以这个 fixture 证明本地 `NTDOS.SYS` 的原始读取成功；
- 下一项是记录 `demLoadDos` 返回后的实际历史状态变化和后续 caller，而不是新增服务实现。

当前 fixture 与 direct diagnostic 都是 trace-only，默认不构建、不进入
`ntdos64-run` 或任何研究型 runtime。
