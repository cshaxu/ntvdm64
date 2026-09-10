# S5 Sysinit CONFIG Host Seam

状态：2026-08-09。本文审计固定 NT4 EN-US 路径中 `DosInit` 返回后的第一个
非 debug、非设备的 command-host 边。它不实现 shim，不授权访问宿主 Windows
目录，也不把 CONFIG 处理误归类为 SoftPC 或 DOS kernel 语义。

## 原始调用顺序

`base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm` 在 `DosInit` 返回、选择默认 drive
之后，以其自身 `sysinitseg:config` 缓冲区作为输出地址执行：

```text
DS:DX = sysinitseg:config
C4 C4 54 0C
  -> BIOS[54h]
  -> MS_bop_4
  -> CmdDispatch(SVC_GETCONFIGSYS = 12)
  -> cmdconf.c::cmdGetConfigSys
  -> return to sysinit1.asm::doconf
```

`MS_bop_4` 从 `CS:IP` 读取 service byte，调用完整 `CmdDispatch`，随后仅消费一个
service byte。monitor 仍只负责原始 BOP dispatch 与 IP 语义；它不得自行填充
`config` 缓冲区或绕过 `cmdGetConfigSys`。

## 原始 owner 的实际数据流

`cmdGetConfigSys()` 的输入是 `DS:DX` 指向、至少 64 bytes 的 guest 输出缓冲区。
函数先调用同文件的 `ExpandConfigFiles(TRUE)`，之后把 `pchTmpConfigFile` 从 ANSI
转换成 OEM，并经原始 `GetVDMAddr(getDS(), getDX())` 写回 guest buffer。失败时原始
代码走 `RcErrorDialogBox()` 与 `TerminateVDM()`，不会以 DOS error code 继续。

`ExpandConfigFiles(TRUE)` 的历史 host 数据流不是一个缺失 API：

```text
GetWindowsDirectory
  -> GetShortPathNameA(system root)
  -> GetPIFConfigFiles(TRUE, raw CONFIG input)
  -> CreateFile / GetFileSize / ReadFile
  -> cmdCreateTempFile / GetTempPath / GetShortPathNameA
  -> filter CONFIG commands, append country= from locale/OEM CP
  -> WriteFile / SetEndOfFile
  -> pchTmpConfigFile
```

它会读取历史 Windows-profile 的 CONFIG 输入、创建 host temporary file，并将
`%SystemRoot%`、locale 和 OEM codepage 投射到 DOS。这个行为在当年的 NTVDM
环境中是原职责，但与本研究项目的 non-invasive、BYOB、contained namespace 要求
直接冲突。

## 正确的现代 seam 分类

这不是 PC/AT、FDC/HDC、CMOS、BIOS 或 DOS INT 21h 的缺口，也不是可以用
“空 CONFIG 字符串”替代的 DOS SVC。它是 **command host configuration projection**：
原始 service 编号、`DS:DX` guest-output contract、BOP/IP consumption 与随后
`doconf` 的 DOS open/read 顺序必须保持；改变的是原实现所依赖的 ambient Windows
profile、temporary-file 与 locale source。

未来实现只能在 command-host 边界提供一个显式的、session-local profile projection：

1. profile 在 guest 发生 mutation 前声明 CONFIG input、locale/codepage、允许的
   line subset 和 session temporary namespace；
2. `cmdGetConfigSys` 仍以原始 `DS:DX` 输出一个 DOS-visible path；随后由原始
   `doconf` 和 DOS open/read 消费该 path；
3. 默认最小 profile 必须拒绝 `DEVICE=`、`INSTALL=`、`SHELL=` 与 `DOS=HIGH`，不能
   静默借此引入 keyboard、mouse、EMS、HMA、任意宿主路径或第三方 executable；
4. 不能读取 `%WINDIR%`、用户 CONFIG/AUTOEXEC、环境变量或 host temporary directory，
   也不能把它们作为 fallback；
5. 失败必须在该 host projection 以明确定义的 session stop/diagnostic 表示，不能由
   fixture 预写 guest buffer、伪造成功或让 `TerminateVDM` 杀掉宿主 process。

这种 projection 是未来 research wrapper/mantle profile 的命名语义；通用 CPU、
machine core 不应知道 `CONFIG.SYS`、`COUNTRY=`、`COMMAND.COM` 或 DOS path。

## 对当前 S5 的结论

`cmdGetConfigSys` 不能直接加入现有 default-disabled original fixture：直接执行会按
历史策略读取/创建 ambient host 文件，既不安全，也不能证明 contained runtime。
在有上述 profile projection 的受控实现之前，S5 应继续做两类工作：

1. 以 source audit 记录 `DosInit -> sysinit -> CMD service` 的真实边界；
2. 沿无 ambient host I/O 的原始连续路径继续闭合 monitor、guest-memory 与 dispatch
   语义。

构建第一个 command smoke test 前，必须单列该 projection 的 contract、failure policy
及正反 fixture；不能用现有 `cmdconf.c` 的 Windows-directory 行为冒充已经支持
`COMMAND.COM /C`。
