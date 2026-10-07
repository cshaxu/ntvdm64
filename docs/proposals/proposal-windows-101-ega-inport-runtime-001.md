# Proposal — Windows 1.01 彩色 EGA 与 InPort 鼠标运行修复

## 候选包与所有者目标

### 所有者选定的当前路线

所有者后续明确选择：将 NTVDM 当作已提供 INT33 鼠标能力的机器，
为 Windows 1.01 编写独立 guest 鼠标驱动，而不是为原厂 InPort 驱动
另改机器。实现放在 `src/ADDON/Mouse Driver 101`，保留原始 INT33、
IRQ/回调和 DOS 执行语义。下文 InPort 硬件修复是早期调查与备选路线，
不再是所选桥接驱动的验收清单；没有实施或声称那些硬件缺口已修复。

所有者亦明确批准在可恢复的安装副本中二进制替换独立 MOUSE 模块，
无需完整重装。只修改该模块槽位，保留原始安装/介质及其他模块字节、
位置和链接；禁止运行期热补丁或修改 Windows 原始核心算法。实际 ABI、
构建、移动/菜单按下释放、正常退出、同 worker 再次启动和发布回归见
[S2 证据](../etc/evidence/m0-t435-s2-int33-driver.md)。当前阶段与交付状态
仍以 CURRENT 为准，不把备选路线或观察超时计为通过。

所有者现已准入本提案，并确认 Windows 1.01 EGA 支持已实测通过。
当前执行阶段与范围以 CURRENT 为准；以下“未编号候选”措辞是原提案
的历史状态，不再作为未准入判断。EGA 保持通过基线，当前待修的是
InPort 鼠标设备与输入链路；既有退出错误需独立确认，不能当作鼠标
同根因或以重新实施 EGA 来替代鼠标修复。

所有者于 2026-10-06 要求将鼠标兼容所需的镜像 diff 修复放到
[队列](../states/QUEUE.md)第一条，并由这一候选包完整验收 Windows
1.01 的彩色 EGA 与鼠标功能。原队尾提案中的 Windows 1.01 运行范围
转入本提案；队尾只保留 [Windows 3.x 非 WOW 调研](proposal-windows-3x-dos-runtime-001.md)。
这是未编号候选 T，不改变[活动任务](../states/CURRENT.md)，不提前
实施源码修复、分配数字 T 或声称兼容已完成。性能候选包顺延，仍独立
负责通用交互及执行性能测量，本包不预先改变其 50Hz 发布上限。

## 已有调查与尚未证明的边界

- 从其他当前目录用绝对路径启动 WIN.COM，曾因找不到 Windows 启动
  文件失败。所有者确认在正确目录启动后可以进入 CGA Windows 1.01；
  这不是需要给程序搜索增加 EXE 目录优先级的依据。
- 原安装的 3239 字节 MOUSE.DRV 使用早期 8255 Bus Mouse 协议，不是
  InPort。Microsoft Mouse 6.24 提供的新版 Windows 1.01 MOUSE.DRV 为
  3358 字节，SHA-256 为
  `8573C81A7ED76C043C722DB5DDC445B5B836D12096303FCA7F8E1EF6DFA05B32`。
  来源与原厂重装说明在 SoftPC 对照记录中；不能用伪造 8255 探测响应
  让旧驱动误认 InPort。
- 本会话已按所有者授权，通过原始 Setup 在 `O:\Windows` 安装新版
  鼠标与第 6 项彩色 EGA。二进制只读核对确认新版鼠标的三个段与
  EGAHIRES.DRV 的两个段完整嵌入 WIN100.BIN，原 CGA 驱动段不再匹配。
  本地安装、备份、页面与写入审计在 `O:\Windows\_update`；它们是用户
  测试安装和调查证据，不是产品输入或已通过的交互验收。目录之外的
  安装文件创建请求已被防护拒绝。随后实测已进入彩色 EGA 桌面，见
  下节；完整图形/鼠标交互与正常退出验收仍未完成。
- 当前 `src/mvdm/softpc.new/base/keymouse/mouse.c` 仍使用
  `loadsainterrupts = 5` 的启动中断突发逻辑。项目输入桥
  `src/ntvdm-exe/softpc/mvdm_softpc_mouse_bridge.c` 目前提交队列并调用
  DoMouseInterrupt，没有接通 mouse_send 的硬件输入路径。仅更换驱动
  或只移植 timer 修复都不足以证明完整鼠标链路。
- 所有者在 Windows 1.01 内正常退出时报告 “The handle is invalid”
  系统错误。根因尚未证明；本包必须调查并完成正常退出/文本恢复，
  不能把该错误归咎于鼠标、忽略对话框或把观察超时当作正常退出。

本地证据不是新 checkout 的前提。准入时保留必要结论、来源、输入
hash 与可复现测试；本地目录缺失时重新取得前提，不能用叙述替代实测。

## 2026-10-06 启动阻断实测结论

所有者要求核实“原 CGA 能启动，更新 EGA/鼠标后是否因 EGA 卡住”，
并将研究结论合并到本提案。结论是：**本次直接阻断发生在新版原厂
鼠标驱动的初始化；宿主 InPort IRQ 检测未通过，驱动转去探测串口，
不存在的 COM2/COM1 触发同步错误对话框。EGA 本身能够进入彩色桌面。**
不是新版驱动二进制已被证明有缺陷，也不是需要退回 CGA。待修复对象
是宿主设备仿真及项目输入绑定，保留当前原厂驱动与 EGA 安装。

### 输入与对照

运行基线是已复制的 APP434/RPC42/I/O25 包，NTVDM 为 x86 CCPU40；
测试 NTVDM 私有 relink 仅加入文件写入防护、有限端口/调用观测。
run16/NTCON 与对应已发布文件 hash 一致。未修改产品源码或 guest；
原安装在各次试验后核对未改变。除上述鼠标 hash 外，安装后的
WIN100.BIN SHA-256 为
`80E5B0F802F2DEF787F72F99ED83AA1734B6FD0D76115165C0856A1503ECFAF9`。
准入时重新冻结实际已交付包，不能把测试 relink 当作正式产品。

| 试验 | 实际观测与边界 |
| --- | --- |
| 彩色 EGA＋新鼠标，初次截图 | 停在蓝白启动屏；测试防护误把 COM2 非磁盘设备请求当作目录外文件拒绝。此轮不能证明 EGA 故障。 |
| 修正测试防护的非磁盘设备识别后重跑 | 真实 `NtCreateFile(\??\COM2)` 返回 `C0000034`（OBJECT_NAME_NOT_FOUND），仍弹原始“cannot open COM2”对话框。排除了仅由防护拒绝导致的解释。guest 此时在新版 MOUSE.DRV 第 2 段 `0142h` 执行 `OUT DX,AL`，DX=`02FBh`；BIOS `0040:0049` 为 `06h`，尚未切到 EGA。 |
| 相同 EGA＋新鼠标，对本次自有对话框分别响应 Ignore | COM2 后还有 COM1 错误；响应两者后，两次独立运行进入 MS-DOS Executive。NTCON 实际 surface 为 **640×350**，画面统计有 7 种 RGB 颜色，包含蓝、黄、红、绿、青。证明可进入彩色桌面，不证明鼠标、完整 EGA 重绘或正常退出通过。 |
| 更新前 CGA＋旧鼠标的安装副本 | 相同运行包/防护下无串口错误对话框，进入黑白桌面；NTCON surface 为 640×400，画面仅 2 种 RGB 颜色。640×400 是该渲染 surface 的尺寸，不将其当作 guest 原生 CGA 模式分辨率。 |

这些是有界诊断，不是完整组合兼容矩阵。观察器保留桌面观察后按
时限清理，报告的 `result=timeout` 不能当作 Windows 自行退出、启动
失败或鼠标验收成功。测试使用独立未切换桌面，清理准确识别的自有
进程和 W: 映射；所有安装/诊断写入仍限于 `O:\Windows`。

### 已定位的驱动与 host 控制链

1. 有限真实端口观测中，`023Eh` 依次返回 `DE/10/DE/10`。新版驱动
   已识别 InPort identity，不能再归因于缺少芯片签名。
2. 随后 `023Ch` 写 `80h` reset 和 `07h` mode-register index；
   `023Dh` 反复写 `11h`／`01h`，进入启用／禁用定时 IRQ 检测，
   未成功选择 IRQ 后才探测 COM2/COM1。
3. 原厂驱动只读反汇编：第 1 段 `024Dh` 识别 InPort，`0284h` 调用
   `02B2h` IRQ 检测；`02FDh` 写 `11h` 开启 30Hz 定时 IRQ，`034Ch`
   统计检测中断，`0303h` 要求至少 3 次；`0308h` 写 `01h` 禁用，
   随后要求计数为 0。失败则清空候选并试下一项，全部失败以 CX=0
   返回，入口的 JCXZ 到 `001Ch` far-call 串口探测段。停止时匹配的
   InPort 代码段 `0335h` 计数快照为 0；这是最终快照，不等于对所有
   检测轮次递送次数的完整测量。
4. 原始 [host_com_open](../../src/mvdm/softpc.new/host/src/nt_com.c)
   在 guest 首次写串口时调用真实 CreateFile。打开失败且 DisplayError
   为 TRUE 时，通过 RcErrorBoxPrintf 同步等待用户响应；之后才设置
   ADAPTER_NULL 并返回失败。
   [资源文本](../../src/mvdm/softpc.new/obj.vdm/resource.rc)中的
   EHS_ERR_OPENING_COM_PORT 与实际对话框吻合。这是此次“卡住”的
   直接等待点，不是已经证明的 CCPU 死循环或显示线程死锁。

当前 [mouse.c](../../src/mvdm/softpc.new/base/keymouse/mouse.c) 的
mode 1/2/3/4 仍由 `loadsainterrupts=5` 调用
`ica_hw_interrupt(...,100)` 发送有限突发，没有真实周期 timer。
已证明的是驱动 IRQ 检测失败分支及之后的阻塞链；**尚未分别证明
timer、PIC mask、IRQ2/IRQ9 映射、门控/reset 与 quick-event 调度
对检测失败的贡献**。实施必须核对整条契约，不能预先声称移植某一行
就会解决全部问题。匹配的 SoftPC 修复仍是来源依据，而非本项目通过
证明。输入桥尚未接通 mouse_send 是后续运动/按钮的独立缺口，不能
把没有输入注入当作启动定时 IRQ 检测失败的唯一原因。

### 保留证据与修复后的明确要求

本地报告为 `O:\Windows\_update\logs\ega-startup-diagnosis.md`；其
`windows101-ega-after-serial.png`、`windows101-cga-control.png` 和
`color-comparison.json` 保留画面/颜色对照。`ega-serial-dialog.txt`、
`ega-serial-cpu.txt`、`ega-driver-attribution.json` 保留阻断位置；
`writes-ntvdm.txt`、`mouse624-irq-detection-disassembly.txt` 和
`irq-probe-result.json` 保留原始 API 结果、端口顺序与检测分支。
测试脚本在同级 `tools` 下的 `ega-serial-test.ps1`、
`ega-ignore-test.ps1`、`cga-old-test.ps1`、`ega-ports-test.ps1`。
将上述结论直接保留在提案中，避免未来执行依赖本地未提交目录存续。

后续要让原厂鼠标驱动直接通过 InPort IRQ 检测，以正常硬件路线
启动并实际移动/点击。Ignore 只是诊断操作，不能作为产品修复或
验收步骤；不自动点 Ignore、不伪造驱动检测成功、不禁止所有合法
串口探测，也不修改原始串口不可用错误合同来隐藏设备缺陷。修复后的
Windows 1.01＋InPort 场景应无需 COM 错误对话框即可进入彩色桌面。
原先 Windows 内 Close 的 Invalid handle 错误继续独立调查，本次
既未证明它与鼠标相同根因，也未证明正常退出已修复。

## 有来源的最小修复范围

按[来源政策](../etc/operations/policy/source-policy.md)逐项分类：
原始 host 缺陷只采用已经存在、语义匹配的 SoftPC 修复；项目新增
适配缺陷在其唯一 owner 修复；原始 guest 限制保留并明确报告。

SoftPC 的只读对照为
`O:\repos.hobby\softpc\docs\history\M9-T85-S1-windows-101-startup-repair.md`，
修复提交 `ee62ad01`，复核提交 `9d102563`，其原始设备 owner 是
`src/app-softpc/softpc.new/base/keymouse/mouse.c`。该修复以原始 quick
event 提供 30/50/100/200Hz 中断，正确保留 InPort identity、索引寄存器、
有符号位移、按钮与 HOLD，门控 data/timer IRQ，并去掉虚构的 8255
探测回显和有限启动突发计数。它在对照项目已有实测；对照通过不能
充当本项目验收，更不能引入其 EXE、设备实现或 snapshot 子系统作为
本产品的构建/运行依赖。

| 修复面 | 唯一归属与实施要求 |
| --- | --- |
| InPort 寄存器、HOLD、reset、IRQ 与 timer | 审计本项目 mouse.c 对选定 OpenNT 与上述已接受 SoftPC 修复的语义差异，只采用匹配的最小修复。继续使用本项目原始 quick-event/PIC 合同；登记来源、hash、DIVERGENCE 与组件 README diff，不批量移植其他 SoftPC 变化。 |
| NTCON 输入到真实设备 | 项目 NTVDM softpc 输入桥与原始 `host/src/nt_mouse.c` 边界共同审计。把相对位移/按钮送到真实 InPort mouse_send owner，明确硬件位移单位、坐标换算、IRQ/CPU 线程、锁、队列和事件顺序。保留 INT33 路径，不重复消费/注入同一事件，不另造设备或 guest 版本分支。 |
| quick-event 生命周期 | 核对 mode 切换、IRQ enable/disable、HOLD、reset、事件取消/重排、机器重置和 worker teardown。旧 callback 不得在新 epoch 或资源释放后递送中断。SoftPC 的 snapshot 修复仅作语义参考，本项目没有对应能力就不引入它。 |
| 彩色 EGA | 验证原厂 EGAHIRES 驱动的实际模式、分辨率、色数、平面、palette、重绘与 NTCON 图形发布。仅修复证明阻断的原始 host/项目边界；安装完成、单色画面或静态截图都不能单独证明彩色交互可用。 |
| 正常退出与重复启动 | 定位 guest Close 到 DOS/NTVDM/NTCON 返回链的实际无效 handle 与资源 owner，依来源政策修复可修复缺陷，保持既有完成、交接和生命周期合同。复原文本模式、光标和输入并再次启动。 |

镜像保留原路径、owner、命名与控制形状；项目新增绑定留在 NTVDM
对应适配目录，NTCON 不获得硬件或 guest 策略，worker-base 不获得
DOS/InPort owner。未证明原因或没有匹配历史修复时，先有界调查并
报告准入边界，不能为了达到运行目标发明原始算法修复。

## 实施与验收

1. 冻结实际正式包、原始介质、安装输出、驱动/配置 hash、host 与
   输入方式。复核上述对照和当前 production caller/provider；先
   提交镜像语义差异、输入路由及最小 diff 设计，再实施有界阶段。
2. 对设备提供可复现的 production-linked 测试：identity/index、旧
   8255 探测不能成功、有符号正负位移/溢出、按钮、HOLD 锁存与释放、
   data IRQ 门控、四种 timer 频率/递归重排、reset/disable/teardown。
   确认实际 quick-event/PIC 路径，不以 Sleep 次数或编译通过代替。
   覆盖原厂驱动的 IRQ 启用时产生中断、禁用后不再产生中断与实际
   IRQ/vector 选择，证明其原始检测直接成功而非被跳过。
3. 在真实 run16/NTVDM DOS guest 路径进入 Windows 1.01 彩色 EGA
   桌面，guest 自己执行 kernel、USER/GDI、驱动与应用，不改走 WOW。
   本场景无需 COM1/COM2 错误弹框或 Ignore；保留已证明的彩色启动
   基线，并补足尚未通过的完整交互。
   验证键盘、鼠标移动/按钮释放、菜单、窗口移动、遮挡/重绘及至少
   一个随附应用的启动、交互与退出；空闲后继续操作仍正常。
4. 实测 Windows 内正常 Close，无无效 handle 系统错误，返回 DOS 后
   文本、光标、输入可用，再次启动和退出仍正常。分别记录 task 完成、
   I/O 归还和 worker 生命周期，不将观察器清理当作 guest 自行退出。
5. 回归 EDIT/INT33、DOS 图形鼠标及已有 DOS/native 交接和 WOW 输入，
   覆盖相关 mode/reset/前端输入捕获变化；运行当时要求的正式包回归，
   复核镜像 diff、介质不可变与一致十文件发布/恢复。实际物理/RDP
   鼠标未测则单独记录，不能借用对照项目的结果宣称通过。

生产 CPU 仍为 x86 CCPU40；本包不引入 CPU30、第二模拟器、guest
补丁、新 helper 或宿主系统修改。测试源码/脚本进入 tests，临时
构建和原始日志遵循正式输出规则。原厂驱动安装是所有者授权的用户
工作文件更新，不是篡改原介质或发布许可；不重编译、二进制修补或
热补丁 guest。后续若需再安装，必须核对用户授权目录并约束实际写入，
不能沿用 Setup 的默认 C 盘目标。既有 `O:\Windows` 安装修改只在其
目录内进行，保留可恢复副本。

关闭条件是本项目实测彩色 EGA、鼠标完整交互与正常退出/重复启动均
有证据，设备测试和受影响回归通过，镜像来源/diff 已复核。缺少
前提、未测试或未解决错误明确保留为未完成，不用安装成功代替。
