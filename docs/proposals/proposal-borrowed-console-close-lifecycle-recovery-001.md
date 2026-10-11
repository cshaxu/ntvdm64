# Proposal — 借用可见 Console 关闭后的生命周期回收

## 状态

这是未准入的候选包，置于队列首位；不分配 T/S 编号，也不授权实现。

现场验收已确认一个与先前 T442 S1 证据相矛盾的残留：从可见宿主
Console 启动 `run16` 的 DOS 会话、DOS 任务返回后，用户关闭该宿主
Console，NTMON 仍可见对应的 NTCON 和 worker。该事实不能由 T442 的
隔离 fixture 通过结果覆盖。它需要以真实宿主关闭为对象重新审计。

## 目标

找出并修复“借用的可见 Console 已由用户关闭、而对应 NTCON/worker
仍驻留”的完整生命周期链。NTSRV 保持生命周期的唯一裁决者：它必须
基于可靠的外部宿主消失证据，决定前端关闭和相应 worker 的退场；
NTCON 与 worker 的直接 I/O 管道不能自行成为关联或生死权威。

## 已知依据

- [T442 S1 借用 Console 丢失证据](../etc/evidence/m0-t442-s1-borrowed-console-loss.md)
  已证明一个外部进程退出的隔离服务路径能通知关闭，但保留真实可见
  Console 关闭作为未完成的 owner 验收。
- [S34 根 Console 生命周期证据](../etc/evidence/m0-t423-s34-console-projection-root-lifetime.md)
  记录过根 Console、前端和 worker 的边界。
- 当前现场报告表明上述真实关闭验收仍失败；这是候选的输入事实，不
  预设它是 NTSRV 等待、Console 成员快照、Terminal/ConPTY 语义或
  NTCON 句柄管理中的哪一处错误。

## 必须回答的问题

1. `run16` 借用宿主 Console 时，NTSRV 实际保存哪些外部身份/句柄；
   它们在 conhost、Windows Terminal/ConPTY 及普通 Win32 文本父进程
   的关闭中分别何时被信号化？
2. 关闭窗口/标签后，NTCON 保留的 Console 附着、NTSRV 的前端记录、
   worker I/O 管道与 worker 进程各自处于何种状态；哪一个状态转换
   没有抵达 NTSRV？
3. 如何区别真正的借用宿主消失、正常 DOS/Win32 任务完成、内层 shell
   退出、以及独占/自建 Console，避免关闭错误的前端或共享 worker？
4. NTSRV 发出的关闭指令如何让 NTCON 和所有受该前端绑定的 worker
   依照既有控制协议清理，而不让任一端以轮询、I/O 断连或 worker 类型
   特判自行裁决生命周期？

## 约束

- 不新增进程、helper、独立注册表或第二套生命周期仲裁。
- 不以固定轮询、窗口标题、仅剩 NTCON 一个 Console 成员等脆弱启发式
  取代经认证的外部身份；若系统提供可靠等待/断连事件，应优先采用。
- 不把 Windows Terminal、conhost、CMD、PowerShell 或某个 worker 类型
  写成关闭策略特判；实现可因 Windows API 的真实语义而分支，但必须
  保持相同的 NTSRV 裁决合同。
- 保留正常共享 worker 驻留、任务完成收据、十秒服务空闲规则和
  前端—worker 仅 I/O 直连的既有架构。

## 计划与验收

1. 先以只读、可复现的真实宿主会话记录 root/launcher、NTSRV frontend
   record、NTCON、worker、外部身份等待和 I/O 状态，在关闭前后形成
   时间线；隔离 fixture 仅作对照。
2. 只在已证明的丢失通知或裁决缺口处做最小修复，并为 conhost 与
   Windows Terminal/ConPTY 分别验证，不能以一边通过代表另一边。
3. 正测：从可见 Console 运行 DOS/文本工作负载后关闭宿主，NTMON 不再
   保留该前端及其仅依附 worker；NTSRV 依既有规则随后空闲退场。
4. 负测：普通任务退出、内层 `COMMAND` 返回、共享 worker 驻留、自建
   Console 和其他仍存活的前端不得被误关闭；不相关 frontend/worker
   必须保持。
5. 记录真实宿主边界、不可自动化部分、源码根因及发布后 owner 验收；
   若无法获得可靠关闭证据，不以周期性杀进程替代正确语义。
