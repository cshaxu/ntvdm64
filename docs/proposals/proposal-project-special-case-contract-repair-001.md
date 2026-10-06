# Proposal — 项目自主实现特判与契约绕行专项修复

## 状态与目标

所有者于 2026-10-06 要求，将自主实现代码中的 hacky 特判调查作为
独立专项修复候选 T 插入队列第 3 条，原第 3 条及以后顺延。本包未
分配数字 T，不修改当前活动任务或提前实施生产修复；准入按
[执行规则](../rules/EXECUTION.md)在 [CURRENT](../states/CURRENT.md)进行。

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

## 建议有界阶段与验收

1. 冻结逐 H 合同、实际生产链和测试矩阵；复现反例并复用原始
   COMMAND、已交付路径隔离、Hook 与控制生命周期证据。
2. 完整修复 H01–H04 的解释器/目标/command-tail 边界，移除替代
   启发式路径，覆盖直接与多层 DOS/native 及 streams/error 回归。
3. 修复 H05–H06 和适用的 H09，两种 Hook 位宽保留正常 native 创建
   行为、受控 legacy 重定向与实际 child 身份/挂起合同。
4. 完成 H07–H10 的唤醒、字体、容量及发布错误处置，修复已证实
   缺陷并保留有原始依据的边界；进行相邻同类有限扫描。
5. 全部 H 行复核、重复/绕行路径移除、实际负例与全产品回归，
   形成可审核修复和诚实限制，按规则发布并提交所有者验收。

这是建议阶段，不是当前 S 分配。实现以准入时正式混合位宽包为准，
NTVDM 继续 CCPU40/x86，其他组件遵循已交付位宽合同。每个代码交付
保持真实生产调用、focused 正负测试、生命周期/RPC/版本/WOW 和
Console/Window、输入连续性、COMMAND/MEM/EDIT、重定向与嵌套返回
回归，遵循当时一致运行包发布及可恢复基线要求。静态扫描、编译
成功或有限快乐路径不能单独关闭一条语义缺口。

原始 guest/固件不可变，遵守[来源政策](../etc/operations/policy/source-policy.md)。
项目控制与错误处理留在其真实 owner，不新增通用 shell/parser、
第二 scheduler、helper 或兼容框架，不让观察改变启动/完成。不得
以文件名白名单、环境缩减、伪造成功或 guest patch 修复回归。原始
mirror 和已批准例外仅按源码/依赖合同处理；新边界按执行规则准入。
本次是候选规划，不宣称生产修复或运行验证已经完成。
