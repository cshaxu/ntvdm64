# Console／Window 统一呈现与原生字符执行

## Owner 最新重启方案

本 proposal 是 T423 的最新目标与新 S1–S6 规划，替代此前重启方案的
S1–S4 切分及 native 子程序强制退回 Console 方案。旧实现已保存到本地
`codex/t423-original-reference-20260925`，快照 `286d54a30`。本地 main
已回退 T422 收口点 `3821036ae`，仅带回最新规划和实施规则，新 S1 已准入。
远端历史未改写，O:/winnt、旧 build 缓存和日志未改动，不作为新 S1 验收。
唯一活动 packet 仍由 [CURRENT](../states/CURRENT.md) 指定。

Owner 要求先将本 T 全部成果保存为本地参考分支，再从上一 T 收口点
重做 main。执行次序不可颠倒：

1. 核实上一 T 收口的准确 commit、当前 T commit 范围、本地/远端 main
   与所有未提交文件；不能仅按日期或标题猜基线。
2. 在具名本地参考分支提交本 T 全部已跟踪及经审查的未跟踪源码、
   文档和测试。忽略的 build 产物、外部运行包与必要原始日志另建恢复
   清单并按需保存在 repo build 下，不盲目纳入 Git 或遗漏唯一证据。
3. 验证参考分支包含当前源码/文档状态，记录 commit 和基线；恢复必须
   不依赖 stash。保存新的本 proposal，确保回退后仍可恢复最新规划。
4. 备份验证后，将本地 main 回到上一 T 收口点，移除本 T 的提交；
   参考分支不得删除。远端历史重写须有明确授权并检查远端 tip，
   使用受保护的 lease，不以既有普通 push 授权代替。
5. 仅恢复最新规划和必要治理记录，在干净基线上登记本 T 的新 S1。
   旧 S 编号/证据明确标记参考分支历史，不能混作新 S 的通过记录。
6. O:/winnt 保留完整可用基线；每个新生产 P 经验证后成套更新。

参考分支仅用于逐文件研究及选择性复用；禁止将原型整包搬回。
旧理想态审计、S1 设计、S2 证据和原型代码保存在上述参考分支，可用
`git show codex/t423-original-reference-20260925:<仓库相对路径>` 读取。
其布局、来源例外和验收结论不能自动带回主线。

## 固定产品契约

Owner 最新确认：run16 拥有全部用户前端及受托启动的 Win32 子进程；
ntvdm 保留原始 DOS 执行、DOS/native 交接及 re-entry 策略。根 run16
是唯一前端，嵌套 run16 只提交/等待，不另建竞争的输入读取者。
下表是 S2–S5 逐步完成后的目标；新 S1 只有可见 Console，Win32 字符
子进程直接继承该 Console，不经过隐藏后端。

统一显示与交互归属，不强迫 DOS 与 Win32 使用同一种执行后端。
DOS 保留原始 MVDM/CCPU40 的执行、文本/图形帧及键鼠语义；
Win32 字符程序由 Windows 的真实隐藏 Console 承担 Console API、
cooked/raw、缓冲区与光标语义。Win32 GUI 正常打开自身窗口。

| 当前执行者 | display=console | display=window |
| --- | --- | --- |
| DOS 文本 | ntvdm 输出由 run16 呈现到可见 Console，保留流式输出与滚屏 | 原始文本帧经 run16 交 kvm-window |
| DOS 图形 | 自动打开 Window，停用可见 Console 程序交互 | 原始图形帧直接交 kvm-window |
| Win32 字符 | 隐藏 Console 与可见 Console 双向转接 | 隐藏 Console 与 kvm-window 双向转接 |

Win32 字符程序无论由外部 run16 还是 DOS COMMAND 发起，都使用稳定的
隐藏 Console 后端，不随 display 切换迁移已打开的 Console 句柄。
正常原生子进程继承同组 Console，不按 EXE 数量创建后端；显式要求新
Console/脱离 Console 的程序不得被静默改写为默认继承行为。

display 由根 run16 前端持有，与所属 worker 会话唯一关联，不放入
原始 MVDM 调度；纯原生前端不为空的 native 任务创建 DOS worker。
每个对应会话唯一 display flag，初始化为 console，嵌套/返回/窗口重建
不重置。Console CAF 设为 window；Console Alt+Enter 不作为产品热键。
Window CAF、Alt+Enter、X 都设为 console，不退出、不 pause/resume。
一个 worker 至多一个呈现 Window；全屏指客户区承载完整 guest 屏幕。
console 策略下只在明确恢复文本模式/有效文本帧后关闭图形 Window；
不能因静态图形暂无新帧而退窗。宿主文本栅格转像素不算 DOS 图形模式。

“冻结 Console”仅停用它的程序画面更新和交互输入，不挂起 guest，
不释放给等待中的父 shell，不改变文件/管道标准流重定向。
Win32 隐藏后端的可见 Console 前端只转接输入，不重复 cooked 行编辑
或回显。DOS 文本和图形输入都进入原始 MVDM，不先绕隐藏 Console。

原始 COMMAND/BaseSrv 决定执行、等待、re-entry 和退出码；
呈现层仅跟随其交接，记录当前输入/画面端点和必要返回关联，不重建
READY/BUSY 调度器。DOS→CMD→run16 DOS→native 的嵌套必须逐层恢复。
跨 Console 关联必须由认证连接及已验证进程能力建立，不信任裸 worker
ID、PID 或环境变量。通信失败不能遗留抢输入线程、捕获或阻塞的等待者。

## 建议代码目录与职责

以下是设计目标，不是已创建的文件清单；实现前复用基线既有 owner 和
同义文件，不为满足树形图增加空壳或通用框架。未列出的组件保持不动。

```text
src/
  run16-exe/
    main.c                         # 保留实际入口命名，程序分类/启动
    presentation/
      controller.c                 # S4：display 与实际呈现目标
      console.c                    # S2：可见 Console 输入/输出与恢复
      window.c                     # S4：唯一 Window、焦点与生命周期
      input.c                      # 前端输入、快捷键及已确认端点路由
      dos_view.c                   # DOS 流式输出/文本帧；S4 加图形帧
      native_view.c                # S3：原生 Console 快照呈现
      lib/                         # S4 从最新 nxvm 导入
        types/
        base/
        kvm-base/
        kvm-window/
    native-console/
      launch.c                     # 原生 CUI 启动、标准流和后端存活
      host.c                       # run16 内部 helper：隐藏 Console 本地 API
      capture.c                    # 活动屏幕、字符/属性、光标、尺寸
      input.c                      # INPUT_RECORD 投递、控制事件边界
      channel.c                    # 有界快照/事件通道与断连清理
      console_frontend.c           # 可见 Console 呈现/输入转接和状态恢复
      protocol.h                   # 此专用通道的版本化无指针记录
      client.h                     # 有限客户端接口，不暴露 helper 私有状态
  ntvdm-exe/
    command/
      native_handoff.c             # 原始 EXEC/re-entry 的最小产品绑定
    presentation/
      execution_binding.c          # 原始交接的确认/返回，不重做调度
      channel.c                    # DOS 输入/输出通道、断连与任务身份
      input.c                      # 前端事件→原始 guest 输入
      dos_text.c                   # 原始文本/流式输出→前端，含滚屏
      dos_graphics.c               # S4：原始图形帧/调色板→前端
  basesrv-exe/
    transport/                     # 有限认证关联、失效/撤销；无帧/输入
  mvdm/                            # 原始 DOS/视频/输入 owner 的最小登记 hook
  opennt-host/                     # 原始 BaseClient/BaseSrv 策略，不重写
  product-abi/                      # 既有共享版本定义；不放运行态机制
  product-package/                  # 包路径；不硬编码 O:/winnt
tests/
  native-console/                   # S1/S2 Console/API/嵌套测试
  ntvdm-presentation/               # DOS 通道、S3/S4 显示和生命周期
```

所有目录创建仍遵守当前“仅 build 下新建目录”限制；上述新子目录是
待实施布局提案，不因写入本 proposal 获得例外。未获得目录例外前，
可将同名具名文件平铺于已有 owner/tests 目录，不改变职责，不擅自 mkdir。

native-console 是 run16 拥有的专用组件，不新增 EXE 或泛用 common。
可见/隐藏 Console 转接只在 run16 一侧执行，不由 ntvdm 链接另一份
前端。ntvdm 保留原始交接策略，只通过有限接口请求启动并接收结果。
helper 使用 run16 的私有内部入口，不增加公开产品参数；隐藏 Console
句柄只在附着进程内使用，不传入 broker 普通记录。

外部直接 run16 native 时由 run16 驱动可见 Console 前端，不为空的
原生命令创建 NTVDM worker；worker 内的 native 也由同一根 run16
前端承接。在新的 S1 审计中固定实际进程/Console 附着图、
创建与继承 flags、标准句柄、输入读取 owner 和失败回收顺序后编码。
根前端生命周期与一次子任务完成分离：不能等待根 run16 退出才能
恢复 DOS。嵌套请求关联现有前端，不移交可见 Console 控制权给 worker。

S1 只修生命周期，S2 迁移宿主前端与 DOS I/O，S3 增加隐藏 Console。
S4 四库来自实施时最新 nxvm，
固定 revision、文件 hash、许可证/notice、依赖闭包并登记来源政策；
不使用当前本项目的库副本充当新来源，不导入 test lib/额外组件，
不修改其他项目。先核对最新版本的 43/50 行容量和接口；旧 SoftPC
容量补丁不能自动重用或被视为 nxvm 字节一致。镜像中不放新库/私有文件。

## 新 S 切分与退出标准

### S1：三类程序生命周期

S1 保留基线 Console I/O，先修生命周期；新 I/O 协议和前端迁移归 S2。
Win32 GUI：创建并等待目标进程，返回其退出码，不接管窗口。
Win32 CUI：子进程直接使用继承的可见 Console，run16 等待并返回其退出码。
DOS：run16 按需启动/连接 basesrv、创建/关联合适 worker，提交 guest
命令，等待该任务完成并返回其退出码；此时 ntvdm 仍直接处理 DOS I/O。
后台服务/worker 不按 launcher 普通子任务连带终止，创建父 PID 不等于
生命周期从属；服务存活不能阻止任务完成后的 run16 返回。

#### S1 前置：run16 无法退出与最小生命周期契约

Owner 报告当前存在 run16 启动后无法退出的问题。这是尚待定位的真实
缺陷，不预判其来自 broker、worker 或转接线程；纳入 S1 必修及退出
门槛，不得留到 S6，也不能以新实现覆盖现场而丢失根因证据。
重启 main 前在参考分支保存可复现步骤、准确源码/产物身份和等待链；
在干净基线检查是否同样存在。若只属于旧原型，也须证明新路径不再出现。

实施顺序：先定位具体阻塞的进程、线程、等待对象及其预期唤醒方；
再固定启动、交出交互、子任务完成、恢复父任务和异常清理契约；
先修原有路径；S2 再接通可见 Console 前后端。不能仅以 CPU 空闲或进程仍在推断根因。

必须分别排查“run16 尚未退出”和“run16 已退出但其他进程仍占住
Console”。记录 DOS 完成通知、run16 等待对象/退出时刻，以及 Console
附着进程；当前窗口残留原因未被证明，不能提前归因。

- [ ] CMD 启动：DOS 完成后 run16 取得任务退出码、停止 I/O 转接并退出，
      恢复原 CMD 提示符；原 Console 窗口保留且能继续输入。
- [ ] Explorer 启动：DOS 完成后 run16 返回退出码并退出，本次自动创建
      的 Console 正常关闭；basesrv/worker 不因附着而维持残留窗口。
- [ ] 证明任务完成不依赖 worker/basesrv 退出；按原始策略可存活的后台
      进程仍存活时，run16 也能完成退出。不得靠强杀服务伪造通过。

- 显示策略只决定呈现/输入路由，不改变任务生命周期或终止任务。
- run16 等待自己所启动任务的原始完成关系，不因 broker/worker 仍存活
  就继续等待；保留原始任务完成与退出码语义，不重建第二套调度器。
- 每个阻塞操作登记正常完成条件、必要对端死亡/断连解除条件、取消
  与资源 owner。输入读取、通道等待及线程 join 必须可解除；不能靠
  任意超时杀死仍正常运行的交互程序来伪装修复。
- 子任务完成后停用该层转接并释放其资源，恢复仍存活的父方；不得
  因父方保留共享 Console 就无限等待，也不得销毁父方仍使用的后端。
- 必要参与方异常退出必须唤醒等待者并传播明确失败；无残留抢输入
  线程、捕获、孤儿 helper 或永不返回的 run16。
- 隐藏 Console 只承接原生输入输出，不成为独立会话管理器。按必要
  的原始嵌套关系记录返回关联，不重复维护 READY/BUSY/任务状态机。

#### 启动来源、目标和嵌套矩阵

| 来源 | Console 与交互归属的验证要求 |
| --- | --- |
| 宿主 CMD 启动 run16 | S1 保留共享 Console：DOS 由 ntvdm 直接交互，native 子进程直接继承；结束后父 CMD 恢复。 |
| Explorer 启动 run16 | 核对 Windows 创建的 Console、标准句柄及完成后的窗口/进程清理；不是另一套任务调度。 |
| DOS COMMAND/程序发起 native 或 run16 | S1 保留原始阻塞/恢复及 EXEC/re-entry；验证逐层任务完成，不改为新调度器。 |
| worker 所属 native CMD 中启动 run16 | 认证关联回原 worker/根前端，不信任裸 PID/worker ID；S3 再覆盖隐藏 Console 归属。 |
| 外部 run16 native CMD 再启动 DOS | S1 按原共享 Console 路径验证，native 等待期间不得抢输入；DOS 完成后恢复 native；S2 再迁移 I/O。 |

每种适用来源覆盖 DOS、Win32 CUI、Win32 GUI 三类目标：GUI 正常开窗，
不创建字符后端/接管字符交互；显式 Console 创建 flags 和重定向另列。
若某组合无合法原始调用者，提供调用路径依据，不用缺测代替不适用。
同一时刻只有一个前端协调者；代码复用与运行时资源归属分开记录。

- [ ] 定位并修复/排除旧原型 run16 无法退出的根因，保留等待链及负向对照。
- [ ] 直接 DOS、直接 native CMD、DOS→CMD、CMD→DOS、多层嵌套逐层 exit
      均返回正确调用者，并验证实际文本、原始退出码和进程/线程清理。
- [ ] 用户关闭可见 Console、子进程异常退出、broker/worker/helper 死亡
      后等待正确解除，不留下挂起 run16；不终止无关用户进程。
- [ ] S2–S5 复验同一生命周期矩阵；kvm-window 的 X 是显示切换，不得与
      用户关闭宿主 Console 的终止事件混淆。

- [ ] 直接 run16 启动原生 CMD/DTMGR，以及 DOS COMMAND 启动原生程序。
- [ ] DOS→native→run16 DOS→native 多层嵌套逐层返回；原始命令/退出码保留。
- [ ] cooked 编辑/回显、raw 键鼠、Unicode/宽字符、活动缓冲区、滚屏/尺寸。
- [ ] Ctrl+C/Break 单独验证，不能以 WriteConsoleInput 的普通按键证明控制事件。
- [ ] 标准流重定向/别名/EOF，快速退出、创建失败、broker/worker/helper 死亡。
- [ ] 原生子进程 Console 继承、无双重输入、前端退出恢复宿主模式。
- [ ] 正式 x86、DOS17、三 WOW 应用非回退及 coherent package 发布门槛通过。

退出：无需 Window，即可从可见 Console 完整操作 DOS/native 及其嵌套；
测试读取实际文本/行为，不以退出码或局部 mock 代替。

### S2：run16–ntvdm I/O 协议与 DOS 宿主前端迁移

从 ntvdm 剥离宿主 KVM 前端，不剥离 guest 键鼠设备/中断、显存、
视频模式/帧生成和原始执行交接。ntvdm 的角色类似 SoftPC common/machine，
run16 类似 common/ui；这是职责类比，不授权导入这两个组件。

run16 独占用户可见 Console 的 DOS 输入/呈现；ntvdm 消费协议输入队列、
发布输出事件。原生子程序仍直接使用同一可见 Console，不引入隐藏后端。
根 run16 是唯一前端，嵌套 run16 只提交/等待，原始 MVDM/BaseSrv 决定
执行顺序与 re-entry，前端跟随确认的交接而不重建调度器。

- [ ] 原始 Console 调用点→协议→两端实现→测试断言的逐项账本。
- [ ] 键盘/字符、鼠标坐标单位与按键配对、焦点丢失及释放事件。
- [ ] 文本快照、流式输出/滚屏、字体/颜色/光标；Console 鼠标真实可用。
- [ ] 明确视频模式、图形帧及调色板契约和边界测试；实际显示接通归 S4。
- [ ] 身份/版本、代次、背压、交接确认、取消/断连及最终输出排空界限。
- [ ] 输入释放、模式、滚屏与控制有序不可丢；过时图形帧可以合并。
- [ ] display/CAF/AE/X 不发给 guest；任务完成仍走 BaseSrv，不以断流推断成功。
- [ ] COMMAND/MEM/EDIT 实际输出、滚屏、键鼠、标准流及完整 S1 生命周期不回退。

退出：真实 DOS 文本 I/O 已经经 run16 接通；不止定义头文件或局部 mock。
生产 P 全部回归、部署及原始语义门槛不降低。

### S3：隐藏 Console 后端与可见 Console 转接

以 S2 为基线，原生 CUI 无论直接或 DOS 内启动，都改用 run16 管理的
隐藏 Console。DOS 通道及原始执行交接不迁移；仍不引入 display/Window。

- [ ] 正式原生启动、后端认证、屏幕/输入双向转接、活动缓冲区与滚屏。
- [ ] cooked/raw、字符/鼠标、控制事件、尺寸、Unicode、标准流/EOF。
- [ ] 原生子进程共享 Console、DOS/native 多层嵌套与逐层恢复。
- [ ] 任务完成、断连、异常取消不留等待者或隐藏后端；复验 S1 全矩阵。
- [ ] 完整生产 P 构建、DOS17、WOW 非回退及一致部署门槛。

退出：隐藏后端正式接通且可见 Console 交互与 S1 一致，不以 fixture
代替真实程序，不将生命周期和退出缺陷留给后续显示任务。

### S4：display、四组件与文本/图形 Window（暂不支持 Window 鼠标）

以 S3 稳定后端为基础，run16 导入最新 nxvm 四组件并持有 display。

- [ ] DOS 原始文本帧和 native Console 快照分别接入同一个 Window。
- [ ] Console CAF、Window CAF/AE/X，前台焦点、重复/释放/失焦无粘键。
- [ ] native 后端不随显示切换重建；DOS/native 嵌套保持 display 并恢复正确画面。
- [ ] COMMAND/MEM/EDIT/CMD/DTMGR 双前端交互，43/50 行和当前字体/光标。
- [ ] 多 worker 隔离、重复开关窗口、异常清理；完整既有生产 P 门槛。

本 S 同时完成以下 DOS 图形显示；只将 Window 鼠标明确留给 S5，
Console 模式下 DOS 与 Win32 字符程序鼠标均必须保持可用。

- [ ] 原始视频帧、调色板、尺寸、刷新接入，不新增第二视频模拟器。
- [ ] DOS 图形键盘操作、图文往返与有效模式确认；Window 鼠标留给 S5。
- [ ] console 策略自动开/关窗，window 策略保持窗口；静止画面不误退窗。
- [ ] 图形期间 Console 无双重交互，CAF/AE/X 只改策略；完整生产 P 门槛。

退出：真实 DOS 图形程序可显示、操作、退出并恢复文本，不用测试图案替代。

### S5：Window 鼠标支持

- [ ] DOS 文本/图形与 Win32 字符程序的 Window 鼠标分别接通真实消费者。
- [ ] 坐标/缩放/行列与相对位移契约、捕获、按键配对、失焦释放。
- [ ] Window/Console 往返、DOS/native 嵌套与退出无假点击/粘键/重复投递。
- [ ] Console 既有鼠标及 S1–S4 全部能力不回退；完整生产 P 门槛。

退出：三类 Window 内容鼠标均可操作并正确清理，不用输入命中替代行为验证。

### S6：剩余缺口清扫与代码质量收口审计

- [ ] 逐项核对 S1–S5 ledger，正常/负向/嵌套/生命周期/多 worker 全覆盖。
- [ ] 清除旧原型、诊断、重复状态与无调用层；核算两镜像 diff、
      导入库行数、自主实现行数，分别列增减及保留理由。
- [ ] x86 构建、DOS17、WINMINE/SOL/WRITE 各自既有最深程度不回退。
- [ ] O:/winnt 六 binary 与被测 manifest 一致，所需 guest/配置正确，
      提交推送且工作区干净；停下等待 owner 审计，不自行关闭 T。

依赖 S1→S2→S3→S4→S5→S6。S6 不承接前面未闭环的核心功能。每 S 维护自己的 ledger/checklist：
生产调用点、测试入口/参数、断言、代码及产物身份、运行证据、失败/清理。
当期能接通的全部接通；研究/接口/mock 不能代替生产闭环。
所有生产 P 遵守 [实施规范](../rules/EXECUTION.md) 的增量构建、DOS17、
WOW 三应用非回退、六 binary 发布规则；纯文档提交不触发编译部署。
guest 不修改，系统注册表不写，原始调度不重做。
