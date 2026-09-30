# Console／Window 统一呈现与原生字符执行

## S16：零延迟 DOS／Win32 文本输入交接

S15 已由 owner 收口，其补充 `dos-native-typeahead` 测试一次通过、一次把
末尾 `exit` 变成 `eexit` 后超时；此前 S14 基线也出现相同现象。S16 单独
审计并修复：先对照 observer 投递、NTKVM 输入队列、NTCON 隐藏 Console、
NTVDM 原始输入读取和所有权交接，逐键证明来源与去向；区分观察器时序
假象与产品丢键／重键。若是测试缺陷，保留严格的真实输入顺序断言后修复
观察器；若是产品缺陷，在已确认的边界做最小修复。不得以固定 Sleep、
放宽输出断言或重试掩盖故障。覆盖 Console／Window、双向交接、嵌套、
断连及退出；生产改动须按既定八文件构建、回归和发布门槛交付。

## S17：Window 退场后的外层 CMD 输入恢复

已由 owner 于 2026-09-30 在 P1 `0b1bc30f3` 后收口；证据见
[S17 recovery ledger](../etc/evidence/m0-t423-s17-window-exit-input-recovery.md)。
后续彻底移除既有 creator/`ERROR_BUSY` 轮询的跨 NTSRV 协议工作已转入
[TODO](../states/TODO.md)，不属于本 S 的恢复屏障。

S16 已由 owner 于 2026-09-30 收口。S17 调查并修复可复现的 Window
退场回归：在 CMD 执行 `run16 command`、按 Ctrl+Alt+F 进入 Window、再在
DOS COMMAND 执行 `exit` 后，Window 消失并回到外层 CMD 提示符，但下一次
可见行输入／回显偶尔失效；盲打 `echo CHECK123` 后，CMD 执行该命令并恢复。

先用低扰动时间戳和状态快照核对 `ntkvm` 的临时 Console buffer 恢复、输入
源／模式恢复、`close_window()`／闲置退场，以及 `run16` 向外层 CMD 返回的
实际先后。不得以 Sleep、强制重绘、注入按键或修改 OpenNT guest 掩盖问题。
若证实退场竞态，建立只覆盖该 root run16 任务的退场确认屏障：外层 CMD
重新取得控制前，须确认原 buffer、输入源和输入模式已经恢复；若仍有合法
frontend 使用者，不能把“确认此任务已归还”扩大成等待整个 `ntkvm` 退出。
核查边界至少包括 `native_console_frontend.c::window_route`／
`run16_native_frontend_destroy`、`window_controller.c::close_window`、
`session_service.c` 的闲置退场和通道停止顺序，以及 `run16-exe` 的完成／
返回路径。验收包括重复真实 CMD→DOS→Window→exit 回归、非 Window 对照、
合法共享前端使用者、异常关闭与完整既有生产回归。

## S18：原生根任务的同一退场确认

S17 的 `retire -> restored` 屏障已修复 DOS 根任务；owner 随后确认原生
字符根任务仍遗漏同一规则：`cmd.exe -> run16 cmd -> CAF -> exit` 能在
NTCON 的 final-presentation 完成后立即让 root `run16` 返回，NTKVM 则只能
在 creator 已退出后开始恢复外层 CMD 的原 buffer/input mode。S18 以 S17
的已验证机制修复该不对称，而不是引入第二条延时或重绘路径。

- root native text 在其直接 target 的实际退出码与既有 final-presentation
  fence 都完成后，发出既有 root retirement；随后等待既有 `restored`。
- NTKVM 的既有 pending/task/member 保护保持不变；不把 target 完成扩大成
  worker/后代终止，也不等待无关 frontend 用户。
- 内层 run16 不持有 root lifecycle，不得发送或等待 root retirement。
- 复验 DOS、原生、shell-fallback native 三种根返回，以及 Window/Console、
  成功/失败 final fence 与既有退出码；生产 P 仍执行 x86、DOS/Window、WOW
  非回退和八文件发布门槛。

## S19：PID-first worker 管理投影与 NTMON 任务显示

S18 完成后，下一项 S 将统一 NTVDM/NTCON 向 NTSRV/NTMON 提供的 worker
管理投影。NTMON 对用户只显示并操作真实 Windows PID；不再显示或传递
BaseSrv 私有 sequence 或 management epoch。NTSRV 按当前已认证、存活的
注册 worker PID 解析 Delete；PID 复用于后来注册的 worker 时，该当前
worker 即为 PID 操作目标。私有连接 generation、路由及 sequence 可留在
BaseSrv 内部，但不得跨 NTMON 管理边界。

表头固定为 `PID  KIND  ELAPSED  STACK  TASK`。NTCON 行显示其 worker PID、
`WIN32`、`MEMBERS=<n>` 和当前执行的 Win32 target 完整路径；空闲时 TASK
为 `<EMPTY>`。NTVDM 行显示其 worker PID，并保留源定义的 DOS/Win16/WOW16
kind、任务深度和任务标签。不得将 PID、成员、状态或起始时间拼接为
`TASK / DETAILS` 长行。

`interface` 定义版本化复制管理契约；`worker-base` 仅提供 NTVDM/NTCON
复用的 worker 端上报客户端、验证和序列化；NTSRV 是唯一认证和管理投影点；
NTMON 只渲染投影并发起 PID 请求。NTVDM 从原 DOS/WOW 记录产生 task label，
NTCON 从实际启动的 target 产生/清除完整路径。不得新增第二个通用任务注册表、
进程树枚举或任意进程控制；NTCON Delete 仍须完成其拥有的 Console 会话关闭
确认，不能仅终止 carrier。

验收包括 PID-only snapshot/terminate 契约、当前 PID 注册查找、NTCON idle/
active target 与 MEMBERS 投影、NTVDM 原有标签/深度保留、五列表格断言，以及
真实 NTCON 会话关闭和适用 x86/DOS/Window/WOW 回归。

## S20：移除 creator/ERROR_BUSY 生命周期轮询

S20 专门完成 [TODO](../states/TODO.md) 中保留的 NTKVM 100ms
creator/`ERROR_BUSY` 生命周期轮询消除，不与 S17 已交付的
`retire -> restored` 恢复屏障混算。新增一项经过认证的
`frontend_state_changed` 等待能力（不得复用 request-ready 事件），并在每次
改变 `FrontendUsage()` 或 retirement eligibility 的服务端状态变更后发信号。
NTKVM 将 creator 句柄直接加入 wait-set；`RetireFrontend()` 返回
`ERROR_BUSY` 后只能等待这项状态事件再重试，绝不使用定时超时或 sleep。

验收覆盖 lost-wake、`ERROR_BUSY`、并发 admission、正常/异常 root 退出、
嵌套/native membership、broker loss 和“无计时轮询”检查。该工作只修复项目
新增的前端生命周期边界；不改变 guest、原 DOS/WOW 调度或 S17 的恢复语义。

## S21：NTVDM/NTCON 生命周期语义审计与收束

S21 是 T423 的最后一个 S。基于 S19 的统一 worker 管理投影及 S20 的无轮询
前端等待，逐项审计
NTVDM 与 NTCON 的登记、就绪/占用、接单、嵌套、target/task 完成、前端断连、
空闲、显式管理关闭、异常退出和资源回收语义。识别仍有可证明同形的状态、
协议、验证、复制记录、关闭确认或测试边界，并在不改变原 DOS/WOW record
所有权、NTCON target 执行所有权和既定 native Console 会话关闭语义的前提下
统一实现。

审计不得以抽象“common”层替代真实 owner，也不得为了合并而合并。每个候选
合并点须记录两侧原语义、调用者、失败/清理顺序、共享边界和回归证明；不同
语义必须保留在其实际执行 owner 中。完成同形机制的去重、镜像/adapter
diff accounting、完整生命周期矩阵与生产发布门槛后，T423 交由 owner 验收，
不得自行关闭。

## 最新批准：文本区域尺寸交接修复

Owner 于 2026-09-29 批准新增尺寸交接修复 S；CURRENT 登记为 S13。
S14 已经 owner 验收收口；随后 owner 准入 S15 修复两个实测鼠标问题。
S15 限于 NTKVM 对 Window 捕获与原始 DOS Console `ClipCursor` 请求的
所有权隔离，以及 RDP 绝对输入与 NTCON 文本方块光标的位置对齐；不得
更改原始 MVDM/guest，不能用程序名猜测是否需要鼠标。须以原有 S14
完整回归为基线，完成 x86 编译、定向和全量测试、八文件一致发布，
提交推送后等待 owner 实测，不自动收口 T423。
原产品体验/组件生命周期 S13 顺延为 S14，范围不变，不随本次准入。
本节取代下方旧 S12 active / S13 待准入文字；S12 已交付，当前状态只由
[CURRENT](../states/CURRENT.md) 管理，T423 不自动收口。

- NTKVM 保存当前会话权威逻辑文本区域、文字、光标、字体和交接版本；
  可见窗口像素大小、隐藏 Console 物理视口、滚屏缓冲区容量不能替代逻辑尺寸。
- NTKVM 验证公共协议；NTCON/NTVDM 验证并真实应用各自后端尺寸。
  请求、应用、确认后才完成交接并恢复输入，禁止仅修改帧头冒充接通。
- 首次原生文本初始化采用 DOS 支持的尺寸；从 DOS 进入必须继承实际 DOS
  文本区域。已有 Win32 链启动/返回继承当前区域，不恢复初始默认。
- Win32 程序主动改尺寸允许应用并向后续启动/返回传播；宿主物理窗口限制
  不是程序改尺寸意图。须核实真实 API 可观测性，不能凭 srWindow 变化猜测。
- 返回 DOS：有实际验证可用的原始视频模式接入路径则原样采用；否则恢复
  该会话最后一次有效 DOS 文本模式，无记录时用原始启动默认。不硬编码
  普遍 80x25，不伪造 BIOS/显存状态，不修改 guest。
- 转换不重排文字，列左对齐；缩短时选择尽量保留顶部且包含光标的连续行；
  超出目标列的内容不进入 DOS 画面，增长处补空格，光标转换/限界。
  原生滚屏历史不清空。兼容交接不跳变；不兼容模式转换允许一次明确变化。

验收包括窄桌面双向交接、连续/嵌套原生程序、真实主动 resize、DOS 支持及
不支持模式、右/下边缘标记、滚屏/光标/字体/鼠标坐标、失败确认和独立会话。
保留既有 DOS 路径和 WOW 深度；x86 构建及完整回归后才发布 O:/winnt 八文件
并提交推送。模拟测试不替代真实交接。


### S13 补充批准：NTCON 自有文本鼠标光标

Owner 明确选择文本反色方块，保持现有文本帧协议和共享库，不保留前端
专用像素箭头。NTCON 接收前端事件、维护逻辑位置及按钮、生成隐藏 Console
的原生鼠标记录，并仅在输出文本帧副本合成软件光标。不得修改隐藏 Console
真实字符、文本插入光标或用于后端交接的原始屏幕；NTVDM 原始鼠标路径不动。
NTKVM 删除原生鼠标位置/坐标策略与箭头合成，只保留事件路由和共同呈现。
输入协议允许显式携带相对位移及修饰键，不把前端物理尺寸强加给 NTCON；
输入扩展须版本化并验证，不改变文本帧形状。交接、释放捕获、断连及退出时
不得遗留按钮按下或将方块烙入屏幕。80x50/font16 继续走直接文本帧，不为
光标转成受768行限制的 DIB，也不扩大共享库容量。

### S13 画面竞争的同步要求

同一 NTKVM 前端内，DOS 写屏与 NTCON 获取前端整幅画面必须争用同一把
I/O 锁。NTCON 跨多次 RPC 读取时使用明确的 begin/end 事务；先得到锁的
一方完成该段工作后才允许另一方继续。断连、取消或协议错误必须释放锁；
不得以定时轮询代替可控制的生产者/消费者同步。隐藏 Console 的 Win32
目标不遵守项目锁，NTCON 对其快照只能进行前后几何一致性验证，发现变动
丢弃整帧并有限重试，不能发表混合尺寸的帧或把永久错误吞成成功。

## 最新批准：NTCON 独立原生文本后端

Owner 追加批准统一文本协议最小扩展：原有 glyph/attribute 两字节 cell
保持有效；需要逐字符样式时，两种 worker 共用可选第三字节 style。
DOS 默认不设置，NTCON 用它保留下划线；不得借用颜色亮度位表示样式。
NTKVM 仅有一个公共文本解码/渲染入口，不保留 native 字形映射或专用
渲染器，不修改 guest 或共享 lib。协议版本检查及未知样式拒绝均须验证。

### 最新后端选择：普通隐藏 Console，撤销 ConPTY 和 helper

Owner 批准保留独立 NTCON worker 架构，后端由 ConPTY 改为 NTCON 自有、
自附着的普通隐藏 Console。本节优先于下方所有 ConPTY 迁移计划。
不增加私有 helper（包括短期 bootstrap），NTSRV 不持有或管理 Console/
ConPTY，NTKVM 不承担后端职责。通过启动属性或 NTCON 本地初始化建立
隐藏 Console；不得依赖事后寻找用户程序 PID 附着。worker 从创建开始就能
使用原生 Console API 读写屏幕、输入队列和查询真实附着成员。

S12 仍完整承接独立登记、接单/嵌套重入、直接目标结果、生命周期、管理、
统一 NTVDM 文本帧/字形、连续屏幕及输入交接和既有全部回归。复用现有
Console state/launch/control 测试；ConPTY/VT 泵及前端关闭代理在生产切换
后删除，保留研究证据不代表保留双后端。禁止把新 Console 抢占前台或
仅链接成功算作验收。公开 API 的 Console 会话关闭及后代存活必须实测。

下面 S12 清单中的 ConPTY 所有权/解析项改验普通隐藏 Console 的实际拥有、
状态读取、输入投递、成员识别和会话关闭；其他退出标准不减少。

### 最新优先契约：NTCON 与 NTVDM 并列 worker

Owner 追加 `src/interface`：统一拥有所有跨组件协议声明，包括 KVM 文本/
图形帧、键盘鼠标事件、NTSRV 与 launcher/worker/monitor 的消息、控制和
对应 IDL。只放契约，不放认证、收发、生命周期或调度实现。迁移旧定义而非
复制，所有消费者引用唯一声明；生成的 RPC 文件仍在 build。

共享组件采用 owner 指定名称 `worker-base`：`src/worker-base/` 和
`worker-base.lib`。Owner 最新澄清：承载双方语义一致的项目新增 worker 机制，供
`ntvdm-exe`、`ntcon-exe` 复用。`run16-exe`、`ntsrv-exe`、`ntkvm-exe`、
`ntmon-exe` 各自内部共用处理两类 worker 的主路径，仅在必要处按类型分支，
不将这些调用者的职责搬入 worker-base。不增加进程或通用 scheduler。

共享审计覆盖连接、传输/校验、取消/断连、帧与通用输入、交接确认、完成通知、
释放/回滚和生命周期，包括镜像文件内的项目新增内容。原始 OpenNT/MVDM
执行、调度、任务完成、阻塞/恢复和清理不得搬出再反向调用。公共 worker 客户端
归 worker-base，协议声明归 interface，NTKVM 服务/渲染/前端所有权不迁移。
按完整正常、失败、嵌套、断连与退出契约判断；保留真实后端差异，不搭通用框架。

同形生命周期必须尽量共用实际代码，不准复制成两套相似流程。run16 内共用
挂起创建/Prepare/启动期回滚，NTSRV 共用认证、reservation、worker watch，
NTMON 共用管理入口，NTKVM 共用帧/输入连接；后端差异只留在真实执行边界。
原始 DOS/WOW record、GetNextVDMCommand/BOP 重入不得改成自主通用调度器；
Win32 的进程完成也不得伪装成 DOS record。S12 退出须审计重复路径并删除
旧 frontend-owned NTCON 启动/登记/关闭实现。提取出的函数必须被真实调用，
不能仅增加一个未使用的“公共”实现。

Owner 明确 NTCON 是 Win32 文本程序的 worker，不是 NTKVM 的附属服务，
存活期不限定为一个 NTKVM 的存活期。此节取代下文按 frontend 生命周期
创建、唯一登记、关闭 NTCON 的假设；既有候选不因此自动成为正确实现。

- run16 对两类 worker 采用同形的发现/启动、认证登记、提交、直接任务等待
  和结果返回流程；不得把 native 启动请求绕到 NTKVM 去执行。
- NTSRV 管理独立的 worker 身份、就绪/占用/等待重入/退出、任务请求与完成、
  故障及 rundown；前端关联是单独的 I/O 绑定，不是 worker 的主身份或租期。
- NTCON 常驻接单，在外层任务等待期间仍能接收嵌套请求。启动、挂起/交接、
  重入、正常退出、异常退出均须对照 NTVDM 现有原始语义逐项实现和测试。
  挂起不得擅自解释为 SuspendProcess，也不得引入新 scheduler。
- NTKVM 对两类 worker 采用统一的认证连接、输入投递、文本帧接收和断开
  处理；它只拥有可见 Console/Window 和 display，不拥有 worker 或 ConPTY。
- NTMON 统一枚举、状态呈现和结束操作；后端类型区分执行实现，而不是两套
  管理生命周期。结束一项任务与结束 worker 必须保留相同的明确区分。
- 同形指外部接口和可观察生命周期，不把 Win32 task 塞入原始 DOS/WOW
  guest record，也不复制模拟器内部状态。必须先复用已选原始 worker 管理
  路径，再保留最小的原生执行差异。前端断开、Console 显式关闭和 worker
  故障分别对照 NTVDM 处理，不能只因 frontend 连接结束就自行回收 NTCON。

新增验收：两种 worker 的管理操作对照矩阵、前端与 worker 独立存活、前端
失效/重连、嵌套重入、每层独立结果、显式关闭及异常故障。先审计 NTVDM 的
实际调用链与退出规则，再迁移 NTCON；不以“永久不退出”冒充同等生命周期。
先前提出的一次性初始化副本仍未获批准；本次生命周期变更不是该机制授权。

### Owner 澄清：NTKVM 仅拥有可见前端

Owner 明确：ConPTY 由 NTCON 创建、管理和关闭；NTKVM 只管可见 Console、
Window 及其切换。这取代本候选及历史章节中“NTKVM 持有 ConPTY／终端解析器”
的所有权安排。S12 仍然 active，已有代码与测试保留为迁移输入，不算收口。

- NTCON 拥有 ConPTY、原生输入编码/投递、VT 解析、原生屏幕/历史/模式、
  实际成员、原生启动及会话关闭。它输出完整且有一致版本的画面，不让前端
  拼合 VT 状态和另一份 Console 快照。
- NTVDM 继续拥有 DOS 执行、设备、输入处理及文本/图形帧生成；不迁入
  原生 Console 职责，不改 guest 或原始 DOS/WOW 调度。
- NTKVM 仅采集可见界面输入、选择活动后端、接收完整帧，进行可见 Console/
  Window 呈现和 display 切换。它不创建、附着、读取或关闭后端 ConPTY/
  隐藏 Console，不维护 VT 解析器或原生后端执行/成员状态机。
- NTCON 与 NTVDM 的文本帧使用严格相同的现行 `product-abi/console_video.h`
  契约：`console_video_description`、`console_text_style`、字符/属性字节对；
  字段布局、字体银行、调色板、光标、分块提交和完整帧可见规则均一致。
  NTCON 只发布 `CONSOLE_VIDEO_TEXT_FRAME`，不得把文本转换成图形帧发送。
  NTCON 后端负责 Unicode 到已批准 PC 字形范围的转换；NTKVM 不保留另一套
  native 字形映射或 VT 解析/绘制路径。默认点阵来源与 NTVDM 一致，并验证
  DOS 下载字体及双字体银行在交接中的状态，不能只核对一份默认 ROM 表。
  现行字节字符帧不承诺任意 Unicode 字形；后端可保留原始 Unicode 状态，
  不得暗自扩充 native 专用帧格式。原有 Console Unicode 回归另行明确核对，
  不把窄字形帧测试冒充该能力的证明。
- NTSRV 继续认证登记和实例管理；NTMON 的结束请求由 NTCON 执行真实会话
  关闭并确认，不由 NTKVM 持有 HPCON 代办；run16 不承担终端泵。

迁移依次完成：登记并保全当前证据 → 验证 NTCON 单进程资源建立/附着机制
及失败清理 → 迁移已有 ConPTY/解析/状态和输入代码 → 统一画面/输入交接 →
清除 NTKVM 越界链接及管理代理 → 原有完整 S12 验收和八文件交付。
不得因一种附着调用失败而把资源放回 NTKVM，也不得默默增加 helper、观察
进程、Job 或另一个 scheduler；需要扩大边界时带证据报告。

S11 鼠标范围已完成编译、生产路径回归及 O:/winnt 七文件发布；详见
[S11 收口证据](../etc/evidence/m0-t423-s11-interaction-retirement.md)。
Owner 已于 2026-09-28 验收通过并准入 S12；CURRENT 登记唯一 active packet。
S11 已推送提交为 965083eec，T423 保持打开，S12 完成后另行交付验收。

本节取代下文 S9/S11 的无 helper、每分支 PTY 约束，不改变已发布事实。
S11 必须先完成鼠标修复及压力验收，不能把鼠标尾项转给 S12。保全研究、
候选和测试，仅将屏幕交接/退场重组移交新增 S12 NTCON，不宣称这些缺陷
通过。Owner 确认 RDP 鼠标问题已经解决，取消原 RDP S13 计划。最新指令将产品体验修复从候选 T 队列移入本 T 的最后一个 S13，范围是组件生命周期及启动/使用/退出体验，不恢复 RDP 待办。S12 仍是唯一 active packet，S13 待 S12 完成后单独准入；CURRENT 是唯一状态权威。

- `src/ntcon-exe/ -> ntcon.exe`：一个字符前端会话共用一个原生文本后端，
  附着真实 Console，提供屏幕/光标/输入/模式及实际成员操作。优先保留
  ConPTY 传输，不能仅包装管道后声称可读写远端 Console 状态。
- `ntkvm`：可见 Console、Window、display、输入路由与连续呈现的唯一 owner。
  后端控制/屏幕交接参照现有 NTVDM 认证连接及复制式协议，不经 NTSRV
  转发画面。DOS 图形仍走 NTVDM 原始帧路径。
- `run16`：分类、启动/提交和直接目标结果，不承担终端泵。DOS/native
  文本同步等待；Win32 GUI 默认创建后返回，Win16 默认 InitTask 通知后
  返回，保留 --wait。NTCON 的存活不延长 launcher 的直接目标等待。
- `ntsrv`：认证 NTCON 实例、前端归属、版本、端点和生命周期；复用现有
  资源转交模式。原生后端记录不能进入原始 DOS/WOW records，不新增调度器。
- `ntmon`：列出已登记 NTCON 的类型、实例/PID、时间、状态及成员；结束
  操作关闭该原生 Console 会话，必须确认结果，不能只杀 NTCON 外壳。
  不递归杀进程树，不影响脱离 Console 的 GUI、其他会话或 DOS worker。

`DDWWDDWW` 同步嵌套链共享一个 NTKVM、一个 NTCON 及原始执行关联下的
一个 NTVDM；跨 DOS 段不拆 NTCON。GUI 分隔的两段字符链仍建立两个前端。
普通 native 子进程依 Windows 规则继承 Console；显式新建/脱离仍保留。
直接目标退出不代表所有 Console 成员退出；后端自身不能计为业务使用者。

### S12 实施与验收清单

- [x] 审计复用 S8 真实 Console 操作、S9 ConPTY、S11 反例与鼠标候选，
      逐项登记保留/迁移/删除；不重建已验证算法或恢复旧 frontend owner。
- [x] NTCON x86 /MT 正式组件/构建目标；共享 APP_VERSION 和版本拒绝。
- [x] NTCON 实际拥有普通隐藏 Console/状态/输入/关闭；NTKVM 源码与链接图无
      后端 ConPTY API、VT 状态或原生会话终止实现，不能以移动文件名代替。
- [x] 两后端文本帧逐字段/逐字节契约一致，NTCON 无图形帧发布；默认点阵、
      DOS 字体交接和缺字行为有测试，NTKVM 使用同一文本帧接收/呈现路径。
- [x] 认证登记、原子复用/并发启动、rundown、跨会话拒绝与失效实例拒绝。
- [x] run16 原生提交与实际直接目标结果，launcher 提前死亡不杀 target。
- [x] NTKVM/NTCON 真实字符、属性、光标和模式交接；固定位置写入、继续
      当前光标、清屏、滚屏、背景输出、输入边沿不重复及重定向不受影响。
- [x] NTMON 列表、正常/异常消失、整个 Console 会话结束、独立会话隔离。
- [x] 真实成员保留/退休、启动与关闭竞争、broker/frontend/backend 故障；
      不以 ConPTY 或 carrier 存活冒充业务成员，也不把 I/O 失败当任务成功。
- [x] 回归 S11 已验收鼠标能力；鼠标压力失败必须由 S11 先修复；RDP 鼠标问题已由 owner 确认解决。
- [x] DDWWDDWW 和既有两条十二目标链、Console17/Window17、fault、monitor、
      三个独立 WOW headless 前沿；生产调用者证明，不以 fixture 代替。
- [x] 一致八文件构建/验收/备份/发布至 O:/winnt：run16、ntsrv、ntvdm、
      ntkvm、ntcon、ntmon、WOW32.DLL、VDMREDIR.DLL；文档治理、提交推送。

完成后等待 owner 实测；不自行收口 T。未验证候选不发布；guest/lib 不改。
新组件目录是 owner 本次显式授权的源码目录例外；中间产物仍仅进 build。
具体迁移与证据见 [S12 ledger](../etc/evidence/m0-t423-s12-ntcon-backend.md)。

## 当前方案：独立 frontend.exe（取代根 run16 前端）

迁移报告后 owner 已批准按此方案开始。S3 以成果保全/重规划结论结束，
不宣称隐藏 Console 功能验收通过；S4 正式准入，复用现有实现和测试。
下文“先报告再编码”的前置步骤已完成，不再阻止本次批准的迁移。

迁移和七文件发布证据已形成 S4 P1 `9c27b5fd2` 并推送 main。
S5 按 owner 的自动顺序准入授权继续隐藏后端扩展验收；当前唯一 active
packet 仍以 CURRENT 为准。S4 不宣称完成后续 Window、鼠标或整个 T。

Owner 最新要求取代下文旧方案。保留 main、当前工作区、已验证代码和测试，
不回退、不丢弃，不为保存快照而将未完成验证的候选发布到 O:/winnt。
当前 S3 停止扩展旧所有权，改做快照、迁移审计和规划；旧隐藏 Console
整包目标明确为重规划，未功能收口。先报告此方案，再继续生产编码。
下文“Owner 旧重启方案”及其 S3–S6 仅为历史设计和验收来源，不再准入执行。

### 最终所有权

Owner 于 2026-09-27 追加 S9 ConPTY 迁移：下述隐藏 Console/helper 是
已由 S4 实现、在 S9 前继续使用的过渡后端，不表示 S8 已完成，也不是
最终保留方案。S9 由 frontend 内 ConPTY
适配器替换并删除自建隐藏 Console/helper 路径；frontend 的可见前端、
display 所有权和 run16/ntvdm 执行边界保持不变。详细契约以下方 S9 为准。

Owner 显示范围澄清：Console 保持字体及完整缓冲区，通过原生滚动访问；
Window 保持共享库的按帧尺寸显示及超屏适配，不新增 Window 滚动条。
200 列等额外 Win32 视口压力测试不是已证明的 OpenNT 要求，不得据此
扩展共享库或新增收口门槛。保留超容量拒绝的边界证据，不算功能通过；
真实已支持路径的回归仍须修复。四组件仅按批准的上游版本导入。

- `src/frontend-exe/ → frontend.exe`：一个字符交互会话的唯一前端。
  独立持有可见 Console、Window、隐藏 Console/helper、输入/呈现线程、
  display 和 I/O 端点交接。helper 是 frontend.exe 的私有角色，不再借用
  run16.exe 的入口。不得拥有 DOS/WOW 调度或替 launcher 推导退出码。
- `src/run16-exe/ → run16.exe`：分类、发现/启动并认证加入 frontend，
  启动/提交目标，等待直接目标，返回实际结果。没有根/内层 UI owner
  分支；所有 launcher 都是客户端。不持有画面、输入泵或 helper。
- `ntvdm-exe`：原始 guest 执行、设备、帧生成、命令交接和 re-entry；
  通过原有直接协议与 frontend 通信，不访问 Windows Console 前端。
- `basesrv-exe`：保留原始 DOS/WOW records 和认证/资源转交；前端登记的
  存活对象改为 frontend.exe，执行 Console capability 仍与前端能力分开。
  不传输帧/输入，不引入新 scheduler。
- `product-abi` 只保留复制式版本/记录；共享协议客户端按命名 owner
  静态链接，不新增 common/compat 框架。前端公共客户端归 frontend-exe，
  run16 与 worker 可链接所需有限子集，不链接呈现实现。

### 字符段与图形段

普通链条 `GGG → CCC → GGG → CCC` 有两个 frontend：每个连续 C 段一个，
G 段没有 frontend 所有权/成员资格。G 指 Win16/Win32 窗口应用，C 指
DOS 或 Win32 字符应用；**DOS 图形模式仍属于其原字符会话**，不是 G。
同一 C 段的 DOS/native 嵌套共享 frontend 和 display，不因每次 EXEC
重置。C→G 不把 frontend/execution 加入能力传给 G；G→C 建立新会话。
旧 C 段等待 G 时仍可保留自己的 frontend；不能合并两个 C 段或连带关闭。
纯 G 链不启动 frontend，也不为它创建隐藏 Console。

原版依据：OpenNT `base/win32/client/support.c:975–991` 根据创建 flags
选择 Console 继承；`windows/core/ntcon/client/dllinit.c:326–353` 对非
Console 应用清空 ConsoleHandle，无 Console 的字符应用请求创建。
这是普通启动基线，不涵盖应用显式 AllocConsole/新 Console/脱离 Console，
也不授权 hook 任意 Windows 进程。项目可控启动点负责能力传递；无法
观察的任意第三方创建，不得用猜测的进程树或裸 PID 补认证。
Win16 共享 WOW worker 不等于共享字符前端，成员资格须按任务/请求而非
整个 WOW 进程传播；这条边界有专门负向测试，不能靠全局环境变量处理。

#### S4 完整 12 层启动链验收（owner 澄清）

`GGGCCCGGGCCC` 是十二个实际目标程序逐层启动并等待的链，不是四个
抽象类型节点，也不是单个 GUI 跳转测试。该矩阵中每个 G 都是 Win32
GUI 子系统窗口程序；D 是 DOS 字符程序，W 是 Win32 字符程序。
run16/helper/frontend 和测试观测器是基础设施，不计入十二个目标层。
DOS 采用实际 guest 执行，不以 native fixture 冒充；GUI fixture 必须
执行 GUI 生命周期，不能仅以一个 Console 程序改标签代替。

Owner 随后将范围明确缩减为两组典型正常路径，不执行 64 组穷举：
`GGGWDWGGGDWD` 与 `GGGDDWGGGWWD`。这两组覆盖 owner 指定的
`DWD/WDW/DDW/WWD` 组内及跨组组合。保持十二个主目标的实际父子启动、
等待和返回顺序；辅助观测不得被计作目标层或改变其前端关联。

每例的 checked-in test 和结果账本须断言：

- 十二个目标的进入、直接子层完成和返回证据完整；记录实际 PID、
  目标类型及对应 launcher/DOS record，不能把命令回显当执行成功。
- 第一组 C1/C2/C3 的认证 frontend 身份相同；第二组 C4/C5/C6 的身份
  相同；两组身份不同。在第二组执行时，仍等待的第一组前端保持存活。
- 六个 G 都不加入或转交 character frontend/execution capability，
  纯 G 前缀不创建前端；两段之间三个 G 不把第一组身份带给第二组。
- 返回时恢复各自 C 段的 I/O 和身份；执行可区分的真实文本/输入见证，
  逐层传播直接目标实际结果，遵循原始 DOS COMMAND 的退出码语义，
  不强制把 native 退出码规则套给 DOS。
- 最后使用者结束后，两组 frontend/helper 各自正常退休，相关 DOS
  record/worker 按既有规则清理；不是测试控制器强杀后才判成功。
- 失败、超时、缺见证或未执行的案例单列，不计通过。旧 native
  GUI-segments 与 A/B 测试继续保留，但不能抵扣这两个十二层案例。

使用不切换的隔离桌面，仍不操作 owner 桌面、不修改 guest 介质、不为
测试引入产品 scheduler。该正常路径矩阵归 S4 所有权迁移验收；S5 的
expanded hidden-backend/fault 矩阵仍独立承担故障与设备交互组合。

### 生命周期和交接

run16 的完成对象始终独立：native 取实际进程结果，DOS 取对应 record。
成功交接后的 run16 死亡不等于 frontend 死亡，不结束 frontend、target
或 worker；已完成结果不被最后一帧/断流覆盖。frontend 正常会话结束或
真实 Console close 才进入现有原始 VDM close 路径；worker 监测对象从
run16 改为认证 frontend。helper 故障仍仅是 I/O 故障，不是 target 完成。
不得递归杀 native 后代或另一字符段；启动未交接的回滚照旧保留。

前端退休依据真实 I/O 使用者/已交接端点的结束，不以某个 launcher
退出作为条件。迁移验收必须覆盖 launcher 已死而 target/子层仍使用
前端，以及最后使用者退出时 helper/frontend 能解除阻塞并退场。
可见 Console 的初次接管保留 CMD 既有窗口，Explorer 路径不留下空壳；
run16 仅传递经认证的启动上下文，不成为 Console 资源 owner。
这需要验证 Console 附着、模式恢复和 shell 返回顺序，不是单纯改名。

### 新 S 序列与退出标准

独立 frontend 方案下，下方历史显示表的 root-run16 owner 全部由
frontend.exe 接替；不把 UI 迁回 launcher。S6 的 display 是字符会话
持久状态，初值 console，嵌套目标切换不重置。Console CAF 设 window；
Window CAF/Alt+Enter/X 设 console，不结束任务、不暂停。console 策略下
实际 DOS 图形仍使用 Window，只有明确文本模式/文本帧才回到 Console；
静态图形没产生新帧不算文本模式。DOS 输入仍进原始 guest 设备，native
字符输入仍进稳定隐藏 Console，文件/管道重定向不被呈现切换改写。
S5 扩展验收已交付至 `1fb291a8f`；S6 从最新 nxvm 四组件核验开始。

| S | 工作与独立验收 |
| --- | --- |
| S1、S2 | 保留已交付历史结论；旧 root 生命周期实现需迁移，不能直接视作新架构通过。 |
| S3（已完成保全/重规划） | 已保存 WIP 和测试、登记未通过项、完成逐文件迁移账本并报告；不宣称隐藏 Console 功能收口。Owner 已另行批准 S4。 |
| S4 | 独立 frontend 所有权闭环：迁移现有 Console/输入/呈现和 helper，统一 run16 客户端，认证身份/关闭对象切换；实际 DOS/native 直接与嵌套可用，两个 C 段隔离、纯 G 无前端；原生退出码、launcher/frontend/worker 各自故障、CMD/Explorer 清理通过。不发布仅能编译的空壳。 |
| S5 | 继承旧 S3 隐藏后端剩余整包验收：A/B 四层链、mixed fault、输入归还、控制通知、尺寸/滚屏、Unicode、raw/cooked、键鼠、流别名/EOF、最终输出和 helper 取消；复用已有测试，不重做已证实算法。 |
| S6 | frontend 内 display 和最新 nxvm 四组件：DOS 文本/图形、native 快照，CAF/AE/X、图文往返、会话隔离。Console 鼠标保留，Window 鼠标由 S7 完成。 |
| S7 | Window DOS 文本/图形与 native 文本鼠标闭环，缩放/坐标/捕获/释放和切换回归。 |
| S8 | GUI 启动与等待语义：统一 Win16/Win32 GUI 的异步启动与显式等待契约，修复交互命令被 launcher 的全生命周期等待占住的问题；验证可靠启动交接、失败反馈、同步结果及嵌套返回。不修改原始 COMMAND 等待或 BaseSrv/WOW 调度来绕过问题。 |
| S9 | frontend 迁移到 ConPTY，移除自建隐藏 Console/helper 后端；Console 模式在 conhost/Terminal 保持原有交互，Window 模式让 DOS/native 文本共用字体位图呈现，完成输入、嵌套、切换与生命周期闭环。 |
| S10 | 删除已替代的旧 root owner/重复分支，核算镜像 diff 与自主代码，完整回归/一致发布，等待 owner 验收，不自行关闭 T。 |
| S11 | 完成 Window EDIT 鼠标输入批处理、压力与取消/交接修复；编译、回归、发布、提交推送后停下等 owner 验证。 |
| S12 | 实施开头已批准的 NTCON 独立原生文本后端；承接原生屏幕连续性、真实成员及退场，保持 S11 鼠标回归。Owner 已验收 S11 并批准准入，实施状态见 CURRENT。 |
| S13 | 文本区域尺寸交接修复：按最新契约统一 NTKVM 权威状态、两端真实应用/确认、原生 resize 传播和 DOS 不兼容尺寸转换。 |
| S14（已验收收口） | 产品体验与组件生命周期修复；已发布并推送 P1 `631206f9e`，owner 于 2026-09-29 报告验证通过并要求收口。自动测试与 owner 验收边界见[S14 证据](../etc/evidence/m0-t423-s14-product-experience.md)。T423 仍保持打开，等待后续指令。 |
| S15–S18 | 后续已准入的鼠标所有权、零延迟输入交接、Window 退场恢复和原生根任务退场确认；S18 是当前活动包，具体状态只由 CURRENT 管理。 |
| S19（S18 后下一项） | PID-first worker 管理投影与 NTMON 任务显示：统一 NTVDM/NTCON 的 NTSRV/NTMON 管理契约、真实 PID 选择及 NTCON active target 标签。 |
| S20 | 消除 NTKVM retained creator/`ERROR_BUSY` 100ms 轮询：以认证 state-change wait-set 取代 timer polling，覆盖丢失唤醒、并发 admission、断连与异常退出。 |
| S21（最后一个 S） | NTVDM/NTCON 生命周期语义审计：以 S19/S20 的共同投影和无轮询等待为基础，审计并完成仍可证明同形的 lifecycle/management 机制统一，保留不同 owner 的实际语义。 |

### Owner 增补：清理交付与实测修复分离

以下是此前准入记录，已由开头 NTCON 规划及上表取代，不是当前 S11 退出标准。
当时 owner 指令：先完成 S10 清理收口及提交推送，再准入 S11 实施两项
修复；RDP 指针问题留给 S12。取代此前“先准入 S11 然后等待”的指令。
S10 的已完成清理与已通过证据保留；未通过的 Window 文本连续性检查、
实测缺陷不得写成通过。未完整验证的候选不为收口而覆盖 O:/winnt。
本 T 保持打开，不能以 S10 清理收口代替产品验收。

历史 S11 checklist（保留未通过项；原生部分重规划至 S12，不勾选为通过）：

- 已准入 ntkvm 独占连续屏幕的有界原型：各 ConPTY 保留自身解析状态，
  DOS/native 更新统一可见屏幕。不改 run16、不加 helper；验证嵌套返回、
  滚屏、长行/退格/历史重绘、清屏、定位绘制与鼠标反向坐标。
  不依靠程序名、提示符或猜测应用意图修补。原生后台屏幕读取差异单列；
  不能把原型通过当成正式包通过。若完整交接必须靠应用特判，停止该路线。
  原型结果：普通流式合成有通过证据，但真实 ConPTY 对照证明“继续当前光标”
  与“固定位置写入”可产生完全相同的输出流，在原始共享屏幕下却需不同位置。
  因而通用原语义目标未通过；保留反例，不扩大程序特判、不发布该原型。

- 返回外层时保留同一屏幕的连续输出；禁止恢复各 PTY 的旧屏幕。后端资源独立不代表用户页面独立。必须验证光标、绝对定位输出、滚屏与嵌套返回，不得仅复制前端缓存后宣称完成。

- [ ] EDIT 鼠标迟缓：核对原始 OpenNT、备份分支与当前输入链。owner 提供
      nt_event.c 单条读取、Sleep(10) 和 IPC 额外开销的初步证据；先测量
      队列积压、投递和 guest 消费延迟，不将它直接认定为唯一根因。
      最小修复不得丢失按键边沿、制造假点击或饿死键盘；不恢复 worker UI。
- [ ] Owner 最新批准取代整个 frontend 永久复用一个 ConPTY：每个独立
      Win32 文本启动分支创建自己的 ConPTY；普通 CMD 直接启动 CMD 等子进程
      通过 Windows 继承原 Console。Win32-A -> DOS -> Win32-B 同时保留 A/B
      两个后端，但 ntkvm 唯一可见前端只选择一个当前交互对象。DOS 仍走
      worker 原始 I/O；不得销毁仍等待 DOS 返回的外层 PTY。
- [ ] 首个客户端成功附着后释放对应 HPCON 保活引用；不再通过已释放的
      HPCON 启动独立分支。Windows 在最后一个附着客户端离开后产生 EOF；
      保留输出读取直到排空，最后 ClosePseudoConsole。API 缺失明确失败，
      不回退为直接子进程计数、不加查询进程或 Job、不使用私有 Console 协议。
- [ ] 退场条件同时满足：无待处理启动、无活动 DOS task、无未结束的
      直接 Win32 请求、无其他仍附着 ConPTY 的程序；与新启动同步地停止
      接收请求，再关闭已结束的后端和 Window。I/O 故障不能冒充正常 EOF。
- [ ] 验证 W-A -> DOS -> W-B -> W-C：A/B 为独立 PTY、C 继承 B；逐层
      返回、退出码、后台输出隔离及输入归属正确。外层存活期间不强杀 A。
      分支有仍附着后代时保留，脱离 Console 的独立程序不阻止 EOF。
- [ ] 实测 COMMAND → CMD → exit → DOS → exit 正常关窗；CMD 留下仍
      使用终端的子程序时保留窗口，子程序真正退出后自动关窗。覆盖并发
      新启动、断连和独立会话不受影响。继续核对 S10 的退出画面连续性问题，
      不预设它与存活问题是同一根因，不放宽文本断言掩盖实际输出回退。
- [ ] 正式 x86 构建、受影响测试、双显示 DOS17、嵌套/故障和 headless
      WOW 前沿验证；通过后一致发布七文件，提交推送，仍不自动关闭 T。

S12 承接 RDP 指针越界的复现、捕获/裁剪/失焦/释放契约和真实环境验证。
共享 lib 不因本次规划而获准修改；现有无 helper、无 Job、前端唯一 owner
约束保持。若真实成员查询确需改变这些边界，先报告证据和最小方案，
不得偷偷新增辅助进程或借用 launcher/worker 做前端 I/O。

### S7 增补：产品命名

S7 增补（owner 2026-09-27）：产品与组件命名统一为 run16.exe / run16-exe、
ntkvm.exe / ntkvm-exe（原 frontend）、ntsrv.exe / ntsrv-exe（原 basesrv）、
ntvdm.exe / ntvdm-exe。本项在 S7 内完成，不另开 S、不改变原始 BaseSrv
函数/记录策略或 frontend 角色。原名称的历史证据保留；生产启动、构建、
身份核验、当前测试与七文件发布全部使用新名称，不保留重复旧 EXE 充当别名。
重新验证后以可恢复的整包替换发布，不能把旧名称的通过结果称为改名验收。

### S8：GUI 启动与等待语义

Owner 于 2026-09-27 批准将此项插入为 S8；追加 ConPTY 阶段后，收口审计为 S10。
当前 S7 不被中断或重新编号；本次只更新规划，不宣称 S8 已实施或验收。
最终顺序为 S7 鼠标闭环 → S8 GUI 启动/等待 → S9 ConPTY → S10 收口审计。

已核实的现状：`run16` 是 Console 子系统程序，Win16 走
`launch_vdm` 的任务完成等待，Win32 GUI 走 `launch_gui` 的进程退出等待。
因此宿主 CMD 等待 run16；DOS COMMAND 的原始 `cmdCreateProcess` 也等待
其直接子 run16。独立 WOW worker 或共享 WOW task 不会自动解除这些等待。
原 OpenNT-4.5 `nt/private/windows/cmd/cext.c` 的交互分支在启用扩展、
非批处理且非单命令执行等条件下，将 WOW 或 Win32 GUI 同步执行改为异步；
不能把原始 BaseSrv 提供完成事件误读成 shell 必须等待。

S9 追加批准：当前 conhost 10.0.26100.1 的横向滚轮，经绕过项目编码器的
原始 SGR 探针仍无横向输入记录，owner 批准登记为宿主限制并继续发布、
提交推送。保留严格失败测试和 TODO，不算能力通过；不得新增 helper 或
修改系统，也不豁免其他输入、生命周期或回归问题。

实施与独立退出标准：

Owner 于 2026-09-28 明确：同一 ntkvm 内所有 Win32 文本程序共用同一个
ConPTY，即使其间穿插 DOS 程序也不释放、不重建。HPCON 由 ntkvm 保持到
前端明确关闭；不能以直接目标结束或 display 切换调用 ReleasePseudoConsole。
不增加观察进程或 Job。无法证明最后使用者离开时，允许保留前端及 ConPTY，
不强求即时自动退休；这不允许阻塞 run16 的直接目标结果，也不允许递归杀后代。
原有“所有客户端退出后必然自动退休”的验收改为明确关闭可清理，并单独记录
保留状态，不能仍把 EOF 或进程组存活伪称为精确 Console 成员计数。

- [x] 在 run16 启动/等待层落实 GUI 异步启动与显式同步等待两种契约。
      先固定默认行为、等待选项或调用接口，以及交互、批处理、CMD /c、
      DOS COMMAND 转交的适用规则；不得仅凭 Console 存在、父进程名或
      程序窗口存在来猜测 shell 上下文。普通 GUI 启动应可恢复提示符；
      明确要求同步的调用仍等待并返回其原有完成结果。
- [x] Win32 GUI 使用实际创建结果；Win16 核实新建/复用 WOW 的可靠启动
      交接点、加载失败及任务归属。请求入队、worker 存活和首个任意窗口
      均不能冒充目标加载成功。异步返回的是启动结果，不是最终退出码；
      保留同步调用的原始结果契约，不虚构 Win16 最终退出码能力。
- [x] 核对 launcher 返回后 BaseSrv 的 pending-creation、reservation、
      rundown、等待句柄和 WOW record 生命周期，确保已交接任务继续运行，
      未成功交接的资源正确回滚；不引入后台等待代理、新 scheduler 或
      frontend 执行策略，不强制每个 Win16 应用新建 worker。
- [x] 原始 COMMAND 仍等待它直接创建的进程；异步 GUI 的 launcher 完成
      后应正常恢复原提示符。DOS/Win32 文本同步行为、重定向、独立会话及
      frontend 所有权不变。不得一律删掉 GUI 等待来破坏批处理和嵌套结果。
- [x] 增加真实调用测试：宿主交互 CMD、DOS COMMAND、批处理、CMD /c、
      显式等待及多层嵌套；覆盖 Win16/Win32 GUI 的成功、缺失/坏镜像、
      启动中 worker/broker 故障、launcher 返回后的目标存活和正常结束。
      断言提示符可继续执行、实际目标身份和结果，不仅断言进程创建。
- [x] 保留现有 GUI 返回 37 的批处理证明，以及两条十二目标链的逐层
      等待/返回验证；需要时改为明确的同步调用，不删除断言或降低门槛。
      加入独立异步案例，证明先恢复提示符、目标后退出。Win16 使用既有
      可用 WINMINE 前沿，SOL/WRITE 原有失败仍如实记录。
- [x] 完成相应生产代码的 x86 构建、DOS17、既有 frontend/嵌套/故障及
      WOW 非回退门槛、一致七文件发布和提交推送；S9 保留其契约，S10 总体审计。

本 S 不承接路径搜索修复或 ConPTY 迁移。先复用原始 shell/VDM 契约及
已有通知机制；新增绑定须给出原始 owner、缺失边界和最小 diff 依据。

逐项验证、原始限制与发布身份见 [GUI 启动证据](../etc/evidence/m0-t423-s8-gui-launch-wait.md)。

### S9：frontend ConPTY 后端与统一文本呈现

Owner 于 2026-09-27 批准追加本阶段，原 S9 收口审计顺延为 S10。
S9 P5 已完成有界实现与发布验证；下列勾选包含明确批准的宿主限制，
不是所有物理交互或横向滚轮均通过。提交推送后自动转入 S10 总体审计。

目标是以 Windows ConPTY 替代自建隐藏 Console 和 helper 子进程方案。
ntkvm.exe 独立管理 ConPTY 句柄、输入/输出流、终端屏幕状态、可见
Console、Window 和 display。删除项目自建 helper 启动入口、私有后端
RPC/快照轮询及已被替代的资源生命周期代码；不新增 helper EXE，也不
以隐藏 Console fallback 永久保留两套后端。Windows 自己的 Console
宿主进程不属于被禁止的项目 helper；ConPTY 并非没有系统 Console 会话。

| 内容 | display=console | display=window |
| --- | --- | --- |
| DOS 文本 | ntvdm 原始文本/输入契约经 frontend 接到可见 Console | ntvdm 原始文本帧经统一字体位图呈现到 kvm-window |
| Win32 文本 | 同一 ConPTY 后端经 frontend 接到可见 Console，conhost 与 Terminal 均保持现有交互 | ConPTY 流经 frontend 终端适配器形成文本屏幕状态，使用与 DOS 相同的字体位图方案到 kvm-window |
| DOS 图形 | 保留实际图形模式自动使用 Window 的既有策略 | 保留 ntvdm 原始图形帧到 kvm-window |

Win32 文本在两种 display 下都使用稳定的 ConPTY 后端，切换不重启
程序、不更换执行会话、不重新解析用户命令。DOS 不绕入 ConPTY，仍由
ntvdm 处理原始 guest 执行和设备。Win16/Win32 GUI 保留自身窗口及 S8
启动/等待规则，不被纳入终端画面或文字流。

Owner 后续明确确认以下实施边界，取代先前未获批准的临时输入探针和
强求跨后端输入追回的建议：

- ConPTY 和全部前端 I/O 只归 ntkvm；run16 不附着后端、不搬运输入，
  ntvdm 不承接宿主 Console 操作。不新增常驻或短命 helper，不把其角色
  转嫁给 launcher/worker。Windows 自己的 ConPTY Console 服务不受此禁令影响。
- ntkvm 管理尚未投递的输入，按既有执行交接通知切换接收端。成功写入
  ConPTY 的输入归该后端，不查询消费进度、不建立影子副本、不自动重放。
  Win32 -> DOS 不保证追回已投递但未消费的输入；这些输入可留在同一
  ConPTY 中，后续返回原生消费者时被延后消费。Owner 明确接受这一相对
  原版共享 Console 的差异。不得为了清空它而关闭仍有客户端的 ConPTY、
  杀目标或重建会话；错误/部分写入不得伪装成完整成功或盲目重试。
- DOS 原始 ReturnUnusedKeyEvents / ReturnBiosBufferKeys 及尚未投递输入的
  有序归还继续保留；验证 DOS -> native 的顺序和无重复。鼠标坐标、
  捕获和按键释放按原有消费者边界处理，不跨端重放旧坐标。
- Window 字符覆盖以 SoftPC 的 PC 字符映射表为界：Unicode 反查已登记
  的字形编号，再使用位图字体；未映射字符显示问号，不额外建设完整
  Unicode 字库或 native-only 系统字体 renderer。终端状态仍保留原始
  Unicode 与宽度，替代显示不得挤坏后续单元格。DOS 原始字体不改变。
  此项明确取代下文先前要求完整 Unicode 扩展字形显示的目标；Console
  模式的宿主 Unicode 呈现不因 Window 的 ROM 字形覆盖而主动降格。

实施与独立退出标准：

- [x] 先审计可复用的 ConPTY/VT 解析、输入编码和屏幕状态组件，登记
      来源、许可、x86 构建和边界；kvm-window 不是 VT 解析器。优先复用
      成熟组件及现有字体/帧/认证代码，避免从头实现通用终端或增加重复
      屏幕模型。共享 lib 改动仍须另获批准，不修改其他项目文件。
- [x] frontend 的 ConPTY 适配器在 Console 模式也持续维护必要的终端
      状态，确保切到 Window 后立即显示完整当前画面，不等下一次重绘。
      处理分段 UTF-8/VT、光标/属性、宽字符/组合字符、备用屏幕、滚动、
      尺寸变更和终端查询回复；明确唯一回复 owner，避免可见终端和本地
      解析器重复回复。不能把原始字节流直接当成文本帧。
- [x] Window 的 DOS 与 native 文本使用同一字体位图选择、字形/单元格
      几何及栅格化规则，不为 native 单独使用另一套系统字体 renderer。
      保留 guest 字体银行、代码页及 43/50 行语义；相同字符/属性/字体
      输入给出一致显示；按上述批准范围验证 PC 字符反向映射、重复映射
      的确定选择、缺字替代和宽字符占位，不声称完整 Unicode 字形覆盖，
      也不改变 DOS 图形帧。
- [x] Console 模式保留固定字体、原生滚动和完整可访问内容，不用缩字
      替代滚动；验证 conhost 和 Windows Terminal 的真实输入、输出、
      光标、尺寸/滚屏、Unicode、raw/cooked、键鼠与 Ctrl+C/Break。
      不以纯 stdout 文本或退出码替代交互验收，不虚构无来源的视口上限。
- [x] Window 输入统一由 frontend 接收，再按活动消费者分别送原始 DOS
      设备或 ConPTY 输入编码器。保留 S7 的移动/点击、失焦/捕获释放及
      DOS 原始输入归还能力；ConPTY 已投递输入按上述批准边界处理。
      不得假设 VT 鼠标已等价覆盖所有 native Console
      输入记录，须以实际目标读入与行为证明，无粘键、假点击或重复输入。
- [x] 保留会话认证和两段字符链隔离。ConPTY 中 native 启动 DOS、DOS
      再启动 native 时恢复正确端点、画面和输入；保持任务结果与 I/O
      生命周期分离。复验两条十二目标链和 S8 同步/异步 GUI 场景，
      不以新建每层 frontend/ConPTY 或树杀进程回避嵌套。
- [x] 覆盖创建失败、管道断开、背压、取消、目标提前退出、最终输出
      排空、EOF、重定向文件/管道和句柄别名、frontend/broker/worker
      故障。已有 helper 故障测试迁移为对应后端故障断言，不因删除 helper
      就删除它保护的语义。关闭 ConPTY 的目标影响须实测并遵守现有
      session-close 契约，不能将普通 launcher 退出变成执行树终止。
- [x] 在真实 CMD、MONITOR、COMMAND、MEM、EDIT 下完成 Console/Window
      两路交互、CAF/AE/X、图文转换及嵌套回归。通过正式 x86 构建、DOS17、
      既有故障与 WOW 前沿验证后才一致发布七文件到 O:/winnt；未通过的
      迁移候选不能替换当前可用包。测试留在仓库，证据记录准确产物身份。
- [x] 验收通过后从正式源码/构建图删除已替代 helper、隐藏 Console
      后端及重复 renderer，不留默认关闭的第二实现。分别汇报删除、
      新增自主代码及导入库规模，不把外部库计为零成本。提交推送后进入
      S10 全局收口审计，不自行关闭 T。

本 S 不实现新的 DOS/WOW scheduler、不修改 guest、不变更路径搜索
proposal，也不扩大成通用终端产品。能力缺口不能以旧 helper 常驻兜底
冒充迁移完成；必须在本 S 完成相应绑定/验证或向 owner 明确报告限制。

每个生产 P 仍执行 DOS17、逐层文本/结果、适用故障与 headless WOW
非回退验证。加入 frontend 后正式包为原六文件加 frontend.exe 共七文件；
S9 前 helper 不增加第八个产品文件，S9 后由 ConPTY 取代且不增加产品 EXE。
首次七文件发布必须整包通过且可恢复到
原六文件基线；纯文档/工作快照不触发候选发布。当前禁止创建新源码目录，
直到实施迁移时按本次明确命名的 frontend-exe 组件准入；其他临时目录
仍只能在 build 下。迁移账本见 [S3 记录](../etc/evidence/m0-t423-s3-hidden-console-ledger.md#frontend-exe-replanning-snapshot)。

## Owner 旧重启方案（历史，已被上述方案取代）

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

Owner 2026-09-26 最新验收授权：本 T 后续各 S 的物理桌面、前台焦点与
鼠标裁剪/释放实测可跳过，以源码逻辑审查及相关单元测试验收，记录为
owner-waived，不伪称实测通过。不再因不能操作桌面而阻塞；其他退出
条件满足后自动依序准入。已有缺陷、实际功能实现、安全后台集成测试、
生产 P 编译/发布及最终 owner 验收不在此豁免内。

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

2026-09-25 owner 特批：“构建能通过就行 特批你完成当前验证目标”。
本次 S1 按正式 x86 构建通过作有界结论；以下原始验收清单保留，
未完成项（特别是 WOW 三应用深度/交互比较）为本次豁免，不标通过。
已有 DOS17、生命周期及定向测试证据保留于
[S1 ledger](../etc/evidence/m0-t423-s1-restart-lifecycle.md)。
此例外不延伸到后续 S/P，不关闭 T，不授权远端历史重写。

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

Owner 澄清：同一嵌套执行链的前端始终由最外层根 run16 持有，包括
根任务是 DOS COMMAND 或 Win32 CMD 的情况。所有内层 run16 始终隐藏，
不创建或显示自己的用户 Console/Window，不抢焦点、不启动竞争的前端
输入读取者，也不因进入 DOS 子任务而提升为新的根前端。这里的“隐藏”
指 launcher 不拥有可见 UI，不是隐藏其目标程序的输出；S2 的原生字符
子程序仍使用既有可见 Console，Win32 GUI 仍正常显示自己的窗口。
内层可承担分类、子进程创建、命令提交、等待与退出码返回，不取得前端
所有权。S3 所需隐藏 Console 后端不属于 S2 的提前实现范围。

根 run16 等待任务时必须继续服务 DOS I/O；嵌套进入/退出不能销毁根
前端、夺走其关联或为内层重置后续 display 状态。原始执行交接与前端
归属分离；S2 证明唯一根前端和原生返回，不能以共享可见 Console 的
继承证明 S3 跨隐藏 Console 的关联已经成立。

Owner 最新准入：根 run16 的寿命就是该交互会话的寿命。根正常/异常退出、
真实 Console 关闭，均关闭其已认证关联的 DOS worker；优先调用原始
CntrlHandler(CTRL_CLOSE_EVENT)，对未完成关闭作有界收尾。这里是产品
会话到原始 Console-close 的映射，不是假称原版 launcher 死亡就强杀。
不操作无关 worker，不递归杀 Win32 后代。非根 launcher 死亡仍不杀
已交接 target。CAF/AE/X 显示切换不是会话结束。已完成任务的原始退出码
保留，worker 关闭时未完成 DOS record 明确失败。所有 Windows Console
交互只归 run16；ntvdm 只承接关闭通知和原始 VDM 清理。

本轮替换验收：根正常/异常退出均关闭关联 worker；非根退出和普通通道
错误不触发该关闭；无关会话及 native target 存活；实际 Console 关闭、
完成先于关闭/关闭先于完成、嵌套返回、DOS17 和 headless WOW 均须验证。
不新增镜像状态机、调度器或 guest 改动。以下上一版根存活预期及输入
pump 例外均已被本次准入替代，保留为先前实验的解释而非现行验收标准。

先前 no-root-termination 和输入 pump 退休方案已撤销；实验与失败堆栈保留在
[边界账本](../etc/evidence/m0-t423-s2-console-boundary-ledger.md#guest-progress-after-frontend-loss-retained-completion-failure)。
非根 run16 仍保留不连带终止规则；已交接 native target 不使用长期
KILL_ON_JOB_CLOSE。启动尚未交接的资源仍允许原有回滚。

- [x] 根正常/异常结束：关联 DOS worker 关闭，无关 worker 和 native
      target 不被主动终止；未完成内层任务明确失败，不能无限等待。
- [x] 非根 launcher 异常结束：已交接 native/DOS 继续，父方仅观察直接
      子进程结果；不存在递归 process-tree kill。
- [x] DOS 非零/错误完成只完成该 record；根仍在则 worker 不因结果非零
      关闭。完成先于 worker 关闭时保留原始结果，反向顺序明确失败。
      见[非零退出与嵌套验收](../etc/evidence/m0-t423-s2-console-boundary-ledger.md#resolved-batch-entry-contrast-and-nested-nonzero-return)
      及前述原始 ExitVDM/进程清理的完成顺序测试。
- [x] 普通传输错误不能冒充根进程死亡；身份、代次、会话隔离不改变。
- [x] 真实 Console close、原始关闭回调投递与阻塞回调的有界收尾已验证；
      见[关闭验收复核](../etc/evidence/m0-t423-s2-console-boundary-ledger.md#published-close-package-focus-record-recheck)。
      CAF/AE/X 显示切换尚未实现，其不触发关闭的运行验证仍由 S4 承担，
      不计为本项已通过的能力。
- [x] DOS→native→DOS 与 native→DOS→native，直接/嵌套退出及取消竞争；
      读取实际 guest 文本，保留现有 DOS17、键鼠、图形返回和 WOW3 门槛。

- [x] DOS 根与原生 CMD 根的嵌套均保持唯一根 run16；记录前端进程身份、
      可见窗口和输入 owner，断言所有内层 run16 无可见 UI、不抢前端，
      子任务返回后根前端仍可交互。退出码或未弹新窗单项不构成通过。
- [x] 原始 Console 调用点→协议→两端实现→测试断言的逐项账本。
      见[当前编译调用归属对照](../etc/evidence/m0-t423-s2-console-boundary-ledger.md#current-compiled-console-owner-reconciliation)；
      含不可用能力、worker 本地资源及尚未通过的物理指针验收，不将账本完整等同于功能全部通过。
- [x] 键盘/字符、鼠标坐标单位与按键配对、焦点丢失及释放事件。
- [x] 文本快照、流式输出/滚屏、字体/颜色/光标；guest 鼠标回调及释放已测。
      物理桌面焦点/裁剪/点击体验按 owner 授权由逻辑审查和单元测试验收，
      不声称完成物理实测。
- [x] 明确视频模式、图形帧及调色板契约和边界测试；实际显示接通归 S4。
      证据：[S2 验收核对](../etc/evidence/m0-t423-s2-console-boundary-ledger.md#s2-exit-checklist-reconciliation)。
- [x] 身份/版本、代次、背压、交接确认、取消/断连及最终输出排空界限。
- [x] 输入释放、模式、滚屏与控制有序不可丢；当前按同步确认有界传输，
      不丢控制事件；图形合并是允许的优化而非 S2 必须新增的功能。
- [x] 任务完成仍走 BaseSrv，不以断流推断成功。display/CAF/AE/X 的
      消费/拦截属于 S4，尚未实现，不计作 S2 热键能力。
- [x] COMMAND/MEM/EDIT 实际输出、滚屏、键鼠、标准流及累计生命周期不回退。

S2 最终[逐项收口核对](../etc/evidence/m0-t423-s2-console-boundary-ledger.md#s2-closure-reconciliation-under-the-approved-acceptance-rule)
记录测试入口、已发布产物、证据与豁免边界；旧实验中的生命周期规则
由最新 owner 会话关闭规则替代，不作为并行验收标准。

退出：真实 DOS 文本 I/O 已经经 run16 接通；不止定义头文件或局部 mock。
生产 P 全部回归、部署及原始语义门槛不降低。

### S3：隐藏 Console 后端与可见 Console 转接

最新 owner 指令：完成 S3 的验证、发布、提交推送后停下等待用户检查；
不得自动进入 S4。此要求覆盖此前连续自动准入授权。

以 S2 为基线，原生 CUI 无论直接或 DOS 内启动，都改用 run16 管理的
隐藏 Console。DOS 通道及原始执行交接不迁移；仍不引入 display/Window。

唯一根 run16 统一拥有和管理可见 Console、隐藏 Console、辅助进程及
后续 Window；不能将隐藏 Console 的管理权交给内层 run16。每个 run16
启动时先验证根/内层关联，不能根据有没有 Console 猜测角色。不存在
继承关联时才建立根前端；存在但失效或认证失败时明确报错，不静默
提升为新根。内层 run16 只负责分类、启动请求、等待对应目标和向直接
父程序返回结果；可继承 Console 附着，但不取得前端所有权，不建立
第二个输入读取者，也不在退出时回收隐藏 Console 或辅助进程。

需要新的原生 Console 后端时，由根管理的 run16 内部辅助角色创建
隐藏 Console 并代理原生目标创建，不增加新产品 EXE。必须明确区分
Windows 实际创建者（辅助进程）与启动请求/完成结果所有者（发起该
请求的 run16）；后者保留实际目标进程 HANDLE，等待和返回实际结果，
不得以辅助进程退出代替目标完成。根提供启动与 I/O 服务，不取得
原始 BaseSrv/MVDM 的 DOS 调度策略。GUI 程序保持独立原生窗口路径。

各后端直接关联根前端，不逐层转发画面/输入；切换的
是当前 I/O 端点及必要返回关联，不是可见前端的所有权。原生子进程按
原有 Console 继承关系运行，不机械地为每个 EXE 新建隐藏 Console。
跨 Console 的内层关联须经认证及进程能力校验，不能仅凭环境变量、
裸 PID/worker ID 或当前 Console 身份认定归属。根为原生 CMD 时，
进入首个 DOS 子任务也必须沿用该根前端；worker 选择与重入仍由原始
BaseSrv/MVDM 决定，不为演示此拓扑强制创建或复用 worker。

辅助进程异常属于 I/O 故障，不证明目标/隐藏 Console 已结束，不触发
目标或后代递归终止。内层 run16 异常不回收根所有资源。后端回收必须
考虑仍存活的原生 Console 使用者；根正常/异常退出则沿用 S2 已批准
的会话关闭契约：关闭关联 DOS worker，不递归杀原生目标或无关会话。
前端身份、执行 Console 关联及每层 completion 分别认证和管理，不能
用共享前端 capability 冒充原始 worker 选择依据。

以下两类真实交互链是独立、强制的 S3 收口测试；COMMAND 为 DOS
COMMAND.COM，CMD 为 Win32 cmd.exe。现代 CMD 中进入 DOS 的测试步骤
明确调用 run16 COMMAND.COM，不假定宿主自动识别并交接 DOS：

- [ ] A：`run16 COMMAND.COM → CMD → COMMAND → CMD`。从外层 DOS
      提示符启动 `cmd.exe /d`，在该 CMD 执行 `run16 COMMAND.COM`，
      再在内层 DOS 提示符启动 `cmd.exe /d`；逐层 exit 回到最外层 DOS，
      最后结束根任务并返回其调用者。
- [ ] B：`run16 cmd.exe /d → COMMAND → CMD → COMMAND`。在根 CMD
      执行 `run16 COMMAND.COM`，在 DOS 中启动 `cmd.exe /d`，再执行
      `run16 COMMAND.COM`；逐层 exit 回到根 CMD，最后结束根任务。

两项均须在每层用实际文本/命令证明交互，记录唯一根前端身份、活动
I/O 端点和返回关系；验证输入不被等待中的父层抢走、不重复回显，
隐藏后端不弹窗，逐层返回后可继续执行命令，保留原始退出码语义，
最终无残留等待者/helper/隐藏后端。保留正常原生 Console 继承与标准
流行为。增加对应断连/异常取消验证并证明不误伤独立会话；mock、
仅进程存在或仅退出码均不能代替两条真实链路。两项任一未通过，S3
不得收口，也不能将其核心关联或恢复工作留给 S4/S6。

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
- [ ] x86 构建、DOS17 及原生/嵌套生命周期回归通过。
- [ ] Owner 对本 T 特批：WINMINE/SOL/WRITE 全部豁免前台实测、可玩性及
      人工交互验收，改做 headless 启动/存活与故障观察；仅本 T 生效。
      NETWORK.DRV 提示后历史上可继续使用扫雷，因此提示本身不判为本轮
      回归；明确记录到达状态和是否仍停在 modal，不把它写成功能全通。
      此例外不豁免 DOS、原生/嵌套生命周期或六文件一致发布。
- [ ] O:/winnt 六 binary 与被测 manifest 一致，所需 guest/配置正确，
      提交推送且工作区干净；停下等待 owner 审计，不自行关闭 T。

依赖 S1→S2→S3→S4→S5→S6。S6 不承接前面未闭环的核心功能。每 S 维护自己的 ledger/checklist：
生产调用点、测试入口/参数、断言、代码及产物身份、运行证据、失败/清理。
当期能接通的全部接通；研究/接口/mock 不能代替生产闭环。
所有生产 P 遵守 [实施规范](../rules/EXECUTION.md) 的增量构建、DOS17、
WOW 三应用非回退、六 binary 发布规则；纯文档提交不触发编译部署。
guest 不修改，系统注册表不写，原始调度不重做。
