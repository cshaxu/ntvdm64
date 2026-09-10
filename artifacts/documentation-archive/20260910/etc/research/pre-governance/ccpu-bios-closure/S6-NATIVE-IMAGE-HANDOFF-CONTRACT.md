# S6 Native Image Handoff Contract

状态：已完成静态审计与 runner 边界收紧，2026-08-09。

## 问题

固定 NT4 `COMMAND.COM` 可以请求 host 运行非 DOS 映像。研究 runner 同时需要在命令行上直接接收
PE32/PE32+ 程序，并把它们留在现代 Windows 宿主。本记录区分这两个表面相似、但所有权完全不同的
路径，防止用现代 CLI 偷换历史 DOS service，也防止把 Win16 NE 当成 DOS 程序送进 engine。

## 历史路径

`src/opennt/base/mvdm/dos/command/cmdexec.c` 中的原始职责如下：

1. `cmdCheckBinary` 从 guest `DS:DX` 取得路径、从 `ES:BX` 取得 DOS EXEC 参数块，并通过
   `GetBinaryType` 分类（137--184 行）。DOS 映像直接以 CF clear 返回（188--191 行）。
2. 对非 DOS 映像，它拒绝在 first-call/AUTOEXEC 阶段运行（208--217 行），随后把 guest EXEC
   改写为 `COMMAND.COM /z <原目标>`，并重写 DS:DX、ES:BX 指向历史 `SCSINFO` storage
   （219--300 行）。这仍是 guest 内部的 DOS 流程，不是 `CreateProcess` 的直达入口。
3. 后续 `cmdExec` 读取 guest `DS:SI` command tail、ES environment、SS:BP 标准句柄，并转到
   `cmdExec32`（538--585 行）。`cmdExec32` 会启动额外线程、阻断/恢复旧 event thread、调用
   `GetNextVDMCommand` 实施 VDM re-entry，并以 CF/AL 把结果反馈 guest（429--488 行）。
4. `cmdCreateProcess` 进一步替换进程标准句柄、读取每盘当前目录环境变量、转换 guest environment，
   然后以历史 `CreateProcess`/等待序列运行子进程（306--426 行）。

因此历史 `SVC_CMDCHECKBINARY`、`SVC_CMDEXEC` 依赖完整 COMMAND/DEM/console/event/re-entry
协议。它不是当前 S5 的 CPU、BOP、DEM 或 CONFIG seam 的替代实现目标。

## 现代 CLI 路径

`src/runner/ntdos64_run.c` 是另一条产品入口，且不进入 guest：

| 输入 | 决策 | 结果 |
| --- | --- | --- |
| PE32 / PE32+ | `classify_image` 读 DOS/PE header（116--157 行） | `run_process` 使用 `CreateProcessW`，仅等待/回收本次 child，并返回其 exit code（159--204、255--257 行） |
| DOS COM/MZ | 验证显式 BYOB profile/root 后，将完整 argv 交给用户指定 engine | runner 不读取、缓存或解释 guest bytes（258--291 行） |
| NE | 明确拒绝 | x64 项目没有 Win16/WOW host path，不能错误地把它降格为 DOS |
| 未知 | 明确拒绝 | 不做扩展名猜测或隐式 host execution |

这一路径只使用文档化的 user-mode APIs；child cancellation 仅影响 runner 自己创建的 process group。
它没有 DOS 寄存器、SCSINFO、VDM re-entry、私有 console API、全局标准句柄替换或历史 host directory
依赖。`tests/ntdos64-run-policy.cmake` 覆盖 PE child exit-code 透传、缺失 BYOB 的 DOS 拒绝，
以及 profile/root 已验证时的 engine argv 交接。NE 分支已有 source-level policy audit；待取得一个
可自由使用的真实 NE fixture 后再加入二进制分类回归，不能用不完整的 MZ 文本假装 NE。

## 边界决定

1. 命令行上的 PE32/PE32+ 是 runner 的产品 dispatch，不是 historical COMMAND service，也不需要
   SoftPC/CCPU 或 mantle。
2. guest 内部触发的 `SVC_CMDEXEC` 仍是未来 command-host seam。除非整个原始调用链已运行并能保存
   guest-visible completion 语义，否则不得把它改接到 runner 的 `run_process`。
3. NE/WOW、PIF、16-bit Windows 兼容和旧 `COMSPEC` shell-out 不在当前 x64 研究 profile 内；拒绝比
   误路由更正确。
4. 未来若要支持 guest 发起的 native-image handoff，必须另立 caller-first contract：受限 namespace
   解析、显式 streams、环境快照、一次 child 生命周期、明确 completion record，以及不进入 DOS 的
   失败结果。不得复用历史全局 event/console/re-entry 状态。

## 对 S6 的意义

这项完成了“CLI 直交 PE”这一独立能力，并把 Win16 排除得更明确；它不缩短 NTIO 到 COMMAND.COM
smoke 所需的历史执行链。S6 的最高 ROI 仍是 profile-owned CONFIG/AUTOEXEC、contained DOS namespace、
原始 DOS open/EXEC 和 command startup record。native-image handoff 仅在这些路径真实到达时再作独立
host seam。
