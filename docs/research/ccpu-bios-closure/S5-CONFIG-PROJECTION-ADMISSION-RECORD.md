# S5 Configuration Projection Admission Record

状态：研究准入记录，2026-08-09。本文把已确认的 NT4 `SVC_GETCONFIGSYS`
边界收敛为将来实现前必须满足的合同；它不实现 shim，不修改 OpenNT 输入，也不把
历史 `cmdGetConfigSys` 纳入当前 runtime。

## 已证实的历史合同

固定 NT4 EN-US 路径为：

```text
sysinit1.asm::ProcessConfig
  DS:DX = sysinitseg:config
  C4 C4 54 0C
    -> BIOS[54h] -> MS_bop_4 -> CmdDispatch(12)
    -> cmdconf.c::cmdGetConfigSys
    -> ExpandConfigFiles(TRUE)
    -> OEM temporary-path 写回 DS:DX
  -> sysconf.asm::doconf
  -> 原始 DOS open/read CONFIG 内容
```

`sysinit2.asm` 将 `config` 定义为恰好 64 个零字节。`cmdGetConfigSys`
同样把 `GetVDMAddr(DS, DX)` 的 `OEM_STRING.MaximumLength` 固定为 64。
因此这不是一个可变长 host 字符串接口，也不是可由 monitor 绕过的 DOS service。

历史 `cmdconf.c` 的 `ExpandConfigFiles(TRUE)` 有两类混合职责：

1. 保留给历史 COMMAND owner 的行为：生成一个 DOS 可见路径，并让随后原始 DOS
   代码打开、读取、解析该路径指向的字节。
2. 不可直接带入 contained runner 的旧 NT 产品策略：读取 Windows directory 下的
   `CONFIG.NT`，展开 `%SystemRoot%`，从宿主 locale/OEM code page 生成内容，并在
   宿主 temporary directory 建立文件。

第一类必须保留原 BOP、`DS:DX`、IP 消费和 DOS I/O 顺序；第二类不能成为 BYOB
运行时的 ambient fallback。

## 将来实现的最小准入合同

在一个已存在、受限的 DOS namespace 和文件 provider 之前，不得实现该 seam。届时
实现必须满足全部条件：

1. **显式输入。** 每个 session 在 guest 启动前提供 immutable configuration profile：
   配置文本或受控源、OEM codepage、允许语句集合、以及 session-local output name。
   不读取 `%WINDIR%`、用户配置、环境变量或宿主 temporary directory。
2. **DOS-visible 输出。** 输出必须是 session namespace 内可由后续原始 DOS
   `open/read` 找到的文件；写入 `DS:DX` 的只能是其 OEM 路径、NUL 终止且不超过
   63 个 payload bytes。不得将 host absolute path、handle、pointer 或预解析结果写入
   guest。
3. **原始控制流。** `BIOS[54h] -> MS_bop_4 -> CmdDispatch(12)` 保留。完成后回到
   `ProcessConfig`，由原始 `doconf` 和后续 DOS I/O 消费输出。projection 不得代替
   `doconf`、INT 21h、DOS parser 或任何 CONFIG 指令语义。
4. **限制与升级。** 第一轮 profile 可明确拒绝尚无实证支持的 `DEVICE=`、`INSTALL=`、
   `SHELL=`、`DOS=HIGH` 等语句，但必须以可诊断的 session stop 拒绝，不能静默删改。
   对这些语句的支持只能在对应 device/HMA/command caller 已闭合后单独准入。
5. **原子失败。** 输出文件、guest buffer 和 session metadata 必须在全部验证成功后
   一起提交。任一验证或写入失败时，保留先前状态并产生结构化、脱敏的 session stop；
   不调用历史 `TerminateVDM()` 杀死宿主进程。
6. **生命周期。** 输出为单 session 资源，只在原始 `DeleteConfigFiles` 等价的 session
   cleanup 点释放；不得泄漏到宿主全局临时目录或下一 session。

## 负面验收

以下任一实现都不得提升为 S6 runtime 证据：

- fixture 预先填充 64-byte `config` 缓冲区；
- 返回空 CONFIG 或 host 路径来伪造成功；
- 直接调用 `cmdGetConfigSys`，使其读写 ambient Windows profile/temp 文件；
- 在 BOP/monitor 中自行解析 CONFIG 或跳过 `doconf`；
- 以现有 `cmdconf.c` 的 `TerminateVDM` 行为结束 research runner。

## S5 结论

这是一条 COMMAND host configuration seam，不是 SoftPC CPU、SAS、PC/AT 设备或
core/mantle 能力缺口。当前只完成了其 source-derived admission record；下一步仍应
先用连续的原始 trace 确定 monitor/transport 的下一个动态停点。只有文件 namespace
和上述 profile contract 都有正反 fixture 后，才可建立 contained configuration
projection。

相关证据：

- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm:1153-1170`
- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit2.asm:1534-1536`
- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysconf.asm:521-2074`
- `src/opennt/base/mvdm/dos/command/cmdconf.c:51-82, 193-268`
- `docs/research/ccpu-bios-closure/S5-SYSINIT-CONFIG-HOST-SEAM.md`
