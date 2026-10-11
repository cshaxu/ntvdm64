# Proposal — 项目自主实现特判与契约绕行专项修复

## 状态与目标

所有者于 2026-10-06 要求，将自主实现代码中的 hacky 特判调查作为
独立专项修复候选 T 插入队列第 3 条，原第 3 条及以后顺延。该候选
现已准入为 M0 T443；本 proposal 是该 T 的分 S 设计记录。活动 S 的
唯一权威仍是 [CURRENT](../states/CURRENT.md)，准入、证据与收口遵循
[执行规则](../rules/EXECUTION.md)。

修复依赖文件名、argv 形状、命令文本启发式、宽泛错误 fallback 或
不等价成功返回的项目自创绕行，使执行走明确、可验证的正常合同。
不能用通用名称、增加 wrapper 或几条验收路径通过代替修复。保留
真正的文件格式、设备、资源身份、权限及原始兼容边界；不以“去特判”
为由删除必要语义或重写原始流程。

## 调查依据与置信度

2026-10-06 对 run16、NTSRV、NTCON、NTVWM、NTMON、common、worker-base、
Hooks 及 NTVDM/WOW 项目适配进行字符串比较、固定路径/文件名、类型
处理、fallback 和成功返回的源码扫描，并读取命令创建、搜索、Hook、
字体、Console 和线程唤醒调用边界。既有
[COMMAND 流恢复证据](../etc/evidence/m0-t420-s34-command-stream-recovery.md)
说明 composite tail 补丁的来源和有限成功回归。

下列特判机制已经由源码确认；列出的行为风险尚未全部运行复现。
不能把风险直接登记成已确认运行 bug，或把未触发 fallback 当作
已经证明等价。准入时冻结实际提交和构建，复查并消费之前任务的
修复，避免恢复已被移除的路径。链接指向真实源码，不固定行号。

## 必须修复或验证处置的清单

| ID | 当前点位与问题 | 目标合同与验证 |
| --- | --- | --- |
| H01 | [command_process_compat.c](../../src/ntvdm-exe/win32/command_process_compat.c) 的 `opennt_command_simple_shell_tail` 用 `strpbrk(tail, "\|&<>")` 决定 simple/composite 路由。 | 去除按字符猜测解释器的路由。以原始调用边界区分已选目标与解释器命令文本；原始 COMMAND 保留语法所有权。覆盖引号内符号、转义、普通参数、重定向、管道、built-in 与 batch，证明不同外观不意外换 shell。 |
| H02 | 同文件 `opennt_command_nested_comspec_tail` 只识别裸 `COMMAND.COM /c`。 | 不再靠裸文件名扫描重建解释器身份。验证带路径、引号、大小写、空白、重命名/同名其他映像及多层嵌套；不得为每种拼写补更多名字特判。 |
| H03 | [run16 main.c](../../src/run16-exe/main.c) 仅 DOS、basename `COMMAND.COM`、三个 argv 和 `/c` 组合重建命令行，移除外层传输引号。 | 分离 Windows argv 编码、原始 DOS command tail 与解释器语法。明确产品内部 composite 传输合同，不对任意同名程序施加特殊参数规则；验证原始尾部、含空格/引号/反斜线、嵌套与外部直接调用。 |
| H04 | run16 分类失败后统一转为 `COMSPEC /c`。 | 区分 shell 请求/允许的未找到结果、无效映像、访问失败和不支持类型；保留原错误，不通过启动另一程序伪装成功。测试坏映像、拒绝访问、缺失文件、built-in、batch、显式路径和明确 shell 调用。 |
| H05 | [Hook create_process.cpp](../../src/nthook32-dll/create_process.cpp) 的 `legacy_application` 填写 resolved；非 legacy 创建仍用该路径替换原 application。 | 确认正常 native 创建是否有明确的产品搜索覆盖合同。没有覆盖依据时保留调用者 A/W application、command、环境、目录和标志；legacy 重定向消费共同搜索且保留尾部。不能撤销已批准的 legacy 发现规则或破坏双位宽 Hook。 |
| H06 | 同文件 `command_tail` 按首个引号/空白截取 executable token，用于搜索及重定向。 | 有限 parser 不扩散为所有 CreateProcess 的替代解析器。测试显式 application 与不一致 argv[0]、未加引号的带空格路径、引号/空白、长路径及原 A/W 错误行为；按实际合同选择最小适配。 |
| H07 | [nt_thread_alert_compat.c](../../src/ntvdm-exe/win32/nt_thread_alert_compat.c) 无原 NtAlertThread 时 QueueUserAPC 并返回成功，注释承认不通用等价。 | 优先原同形调用。fallback 只在已证明的等待合同内适用；对无法等价的边界明确失败。验证 pending alert、alertable/non-alertable 等待、返回原因、无导出分支及生命周期，不能以 APC 入队成功代表 NT alert 完成。 |
| H08 | [wow_public_user_facade.c](../../src/wow32-dll/source/wow_public_user_facade.c) 字体跟踪搜索宿主 Windows/fonts 和默认 SearchPath，其他 guest 系统目录采用产品根。 | 取证区分宿主 GDI 字体与 guest 资源所有权。保留源定义的宿主字体合同或修复实际混用；不能机械替换 GetWindowsDirectory。验证相对/绝对/远程字体、同名冲突和 task cleanup。与后续 WOW 字体包共享唯一修复归属。 |
| H09 | run16/Hook 的 MAX_PATH token/result 与 common 较大搜索缓冲区容量不一致。 | 明确 DOS 原始容量与 native 容量，避免 DOS 限制无依据扩散到 native。验证长度边界、截断、短名不可用、Unicode/ANSI 不可表示错误，不修改历史 guest 结构来扩大容量。 |
| H10 | [console_text.c](../../src/ntvdm-exe/win32/console_text.c) 将发布 ERROR_NOT_READY 转成成功，以忽略 handoff 后旧 owner 画面。 | 证明允许忽略的 stale/inactive 发布与真正未就绪故障能够区分；必要时用明确状态/结果表示。验证旧画面不覆盖新 owner、真实故障不被吞掉、下次完整刷新与关闭；不能恢复 receiver 帧过滤或移除屏障。 |

H08–H10 是待合同取证项。其退出条件是有复现和原始依据的修复，或
精确的保留理由及正负测试；不能强行将合法边界改成所谓 generic。
所有 H 行都需唯一实现 owner、生产 caller/provider、错误与资源
合同、原始来源、实际测试和最终移除/保留处置。

## 明确保留的正常边界

- own-image 派生产品根与 `system32` 内部资源布局；固定服务/worker
  EXE 和 WOW32/VDMREDIR provider 身份。调查未发现这部分项目路径
  硬编码到 O:/winnt；不存在的缺陷不能列入修复。
- SYSTEM.INI 位于产品根及其他系统文件的原始位置、固定 ROM 资源
  映射、CONIN$/CONOUT$ 设备接口、Registry 访问 allow-list。
- 真实 DOS/PIF/NE/PE/subsystem 和位宽分类、正常协议/画面格式验证。
- 已验证的原始 OpenNT 应用兼容规则与 imported 名称；必须通过
  provenance 区分原始兼容行为和后来加入的项目补丁。

没有发现 EDIT.COM/WINMINE.EXE/WRITE.EXE 专用自主执行分支，不把测试
名字或历史 mirror 中的字符串当成此类项目 hack。同类扫描仍覆盖
可达项目补充代码，不能仅凭 grep 命中宣称缺陷或生产可达。

## T443 分 S 任务编排与验收

每条已确认的剩余规划单独成为一个 S；只有共享同一不可分合同的
H01–H03 合并，避免把同一条 COMMAND 解释器边界拆成表面独立、实际
互相破坏的三次修改。S1 已完成只读账本，S2 已完成 Hook 原始请求
修复；S3 起按下列顺序准入。任何 S 的“保留”结论必须有
明确源码依据、正负证据和 ledger 处置，不是以未复现为由跳过。

### S1 — 特判合同账本与边界冻结（已收口）

范围为 H01–H10 的 owner、生产链、原始来源与证据等级。产物是
`m0-t443-s1-special-case-contract-ledger.md`；它不作生产修改，也不把
扫描命中误写成运行故障。

### S2 — Hook 解析身份与原始请求传递（已完成）

范围为 H05：共同 resolver 仅用于分类，native `CreateProcess` 保留
调用者 application/command；legacy 重定向交回调用者原始请求，让
run16 按自己的共同搜索合同解析。验收包括 x86/x64 Hook、A/W、环境、
目录、标准流、属性、失败回滚与 nested CMD；不借此扩张为通用 argv
parser。

### S3 — COMMAND 解释器/目标/tail 的单一合同（H01–H03）

先冻结原始 COMMAND、run16 及 NTVDM 三方之间“已选目标”和“仍属
COMMAND 的原始 tail”的边界，再删除或收敛 `|&<>`、裸
`COMMAND.COM /c` 及三 argv 引号重建这三处耦合启发式。必须覆盖：
引号或转义中的元字符、普通参数、重定向、管道、built-in、batch、带
路径/大小写/空白的 COMMAND、直接调用与多层 DOS/native 嵌套。不得
通过更多文件名拼写、另造 shell 或篡改任意同名 native 程序来通过。

### S4 — NTVWM 原生文本采样/发布合同（已收口）

共享 publisher 只负责异步、限速和最新状态转交；它不应把 NTVWM 对隐藏
Console 的周期性采样变成显示事件。恢复 NTVWM 生产者自身对已观察状态
的比较：未变化的采样不发 cursor operation、文字帧或 publication
transaction；真实 Unicode 单元、viewport、光标形状/显隐/位置、字体、
调色板、软件鼠标或标题变化仍完整到达前端。显式
`ntvwm_presentation_text` 调用继续无过滤。不得把比较重新放入
worker-base 或 NTCON，不能以 Terminal 特判、额外 sleep 或 guest/mirror
修改修复闪烁。

已恢复 NTVWM source-local 观察样本比较；focused named-pipe 覆盖与十组件
发布哈希核验完成，所有者已在 Windows Terminal 验收。详见
[S4 evidence](../etc/evidence/m0-t443-s4-ntvwm-native-text-sampling.md)。

### S5 — 分类失败与 COMSPEC fallback 的错误合同（H04）

为 run16 分类/搜索失败建立最小错误分类：明确 shell 请求可进入
COMSPEC，缺失、无效映像、拒绝访问和不支持类型保留真实错误。验收
必须区分显式路径、built-in、batch、缺失、坏映像和访问拒绝，证明不
会启动另一程序后伪装成功。该 S 不顺带改动 S3 的 COMMAND tail
语义。

### S6 — Hook 有限 command-tail 与容量边界（H06、H09）

H06 的 token 提取和 H09 的容量不一致共用同一输入边界，故同 S
处理：限定 Hook 只在 legacy 分类所需的范围提取目标 token，调用者
显式 application 优先；对 native 保留原 A/W 错误和 argv 行为。
验证不一致 argv[0]、未加引号的空格路径、引号/空白、长路径、
Unicode/ANSI 不可表示、截断与短名不可用。不得将 DOS 的历史容量
无依据扩张到 guest，亦不得创造全局 CreateProcess parser。

### S7 — NtAlertThread fallback 的等价范围（H07）

审计无导出时 `QueueUserAPC` fallback 是否只覆盖已证明的 alertable
等待合同；分别验证 pending alert、alertable/non-alertable 等待、
返回原因、无导出分支与 worker 生命周期。若不能等价，明确失败或
保留经证明的有限边界，不能把 APC 入队成功当成 NT alert 已完成。

### S8 — WOW 字体资源所有权与搜索边界（H08）

确定宿主 GDI 字体、guest 资源、产品根和默认 `SearchPath` 各自的
owner；测试相对/绝对/远程字体、同名冲突和 task cleanup。若根因归属
后续 WOW 字体包，输出精确转交接口与保留理由；若在本 T 可独立修复，
仅改真实 provider，不能机械替换 `GetWindowsDirectory`。

### S9 — stale `ERROR_NOT_READY` 的发布结果边界（H10）

以 handoff 状态证明何时旧 owner 的画面允许被忽略，何时接收端未就绪
必须向上报错。验证旧帧不覆盖新 owner、真实故障不被吞掉、下一完整
刷新和关闭；不得恢复 NTCON 接收侧去重/帧过滤，也不得拆除既有确认
屏障。

### S10 — T443 汇总收口

复核 H01–H10 均已有“已修复”或“有证据保留/转交”结论，删除被替代
的重复绕行，运行相应 focused 负测及一致产品回归，更新 ledger、
CURRENT、证据与发布记录。S9 不新增另一项特判修复；它只消费前序
S 的结果并按规则提交所有者验收。

实现以准入时正式混合位宽包为准，NTVDM 继续 CCPU40/x86，其他组件
遵循已交付位宽合同。每个代码交付保持真实生产调用、focused 正负
测试、生命周期/RPC/版本/WOW 和 Console/Window、输入连续性、
COMMAND/MEM/EDIT、重定向与嵌套返回回归，遵循当时一致运行包发布
及可恢复基线要求。静态扫描、编译成功或有限快乐路径不能单独关闭
一条语义缺口。

原始 guest/固件不可变，遵守[来源政策](../etc/operations/policy/source-policy.md)。
项目控制与错误处理留在其真实 owner，不新增通用 shell/parser、
第二 scheduler、helper 或兼容框架，不让观察改变启动/完成。不得
以文件名白名单、环境缩减、伪造成功或 guest patch 修复回归。原始
mirror 和已批准例外仅按源码/依赖合同处理；新边界按执行规则准入。
本次是候选规划，不宣称生产修复或运行验证已经完成。
