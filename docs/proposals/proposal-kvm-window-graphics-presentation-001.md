# SoftPC `kvm-window` 图形／全屏呈现接入

## 候选包

在前序 CCPU40/V86 契约审计与 `WRITE.EXE` workload 恢复之后，引入隔壁
SoftPC 项目的 `kvm-window`，为现有 Win32/x86 CCPU40 worker 提供图形
模式及可选全屏的 native Window 呈现。它是 `ntvdm.exe` 的 worker-local
presentation adapter，不是第二个 CPU、V86 monitor、broker、guest
媒体，亦不改变现有 `run16.exe` / `basesrv.exe` / 多 worker 架构。

文本模式继续走当前 native Console route；当已选 SoftPC 视频状态进入
图形模式，或用户明确请求全屏呈现时，worker 以 `kvm-window` 呈现其
自身的 frame buffer。切换、关闭和 worker 退出必须回到既有 worker
lifecycle，不得留存独立 GUI 进程或跨 worker 共享窗口状态。

## 来源与架构准入

隔壁 SoftPC 项目在当前来源政策中是比较材料，不能直接成为产品依赖。
本候选获得 owner 的“引入 `kvm-window`”目标授权，但其首个 S 必须在
不复制代码的前提下完成来源政策准入：固定项目、revision、路径、
hash、notice、许可证、原始 SoftPC caller 和所需最小文件闭包；证明
OpenNT/当前镜像不存在可直接组合的等价呈现 owner；列出每个 Windows
API、线程、输入、frame-buffer 与退出依赖；并在 source-policy register
中记录针对该有限组件的明确例外。若无法满足该准入，停止并报告，不
得从比较树悄然复制或把代码伪装为 `mvdm`/`opennt-host` mirror。

接入后的代码归 `ntvdm-exe` 的具名 presentation adapter；原始 MVDM
镜像只可保留最小、已登记的 output hook。不得在 `src/mvdm/` 或
`src/opennt-host/` 新增 `kvm-window` 文件，亦不得建立泛用 `common`
窗口层。

## S 切分

| S | 范围 | 退出条件 |
| --- | --- | --- |
| S1 | 来源、许可证、最小文件闭包与现有 SoftPC video/output boundary 审计；完成 source-policy 例外/register 与组件归属设计。 | 每个候选文件及外部接口有 provenance/disposition；已证明最小 adapter placement、线程/worker ownership 和不可采用的原始替代；未复制代码。 |
| S2 | 导入获准的最小 `kvm-window` source-shaped adapter，连接现有 CCPU40 video/frame-buffer provider 和 worker lifecycle。 | 文本 Console route 不变；图形 mode 可建立/更新/销毁 worker-local window；无新 mirror 文件、无第二执行器、无跨 worker 全局窗口状态。 |
| S3 | 完成全屏请求、窗口关闭、焦点/键鼠输入转交、模式往返和异常 worker 退出的有限契约。 | 全屏/窗口化切换与退出不遗留窗口或输入捕获；失败回到定义的 Console/worker failure，而非挂死。 |
| S4 | 完成 x86 formal build、focused presentation tests 和真实 package 回归。 | 直接及 interactive `COMMAND`/`MEM`/`EDIT` 保持通过；图形 workload 与至少一种全屏/窗口化切换有真实观测；多 worker 不相互夺取窗口、输入或 frame buffer。 |

## 验收与非目标

验收须报告镜像 diff、adapter 代码与引入来源代码的独立行数；记录
frame-buffer 格式、调色板、刷新、输入、线程和 teardown 的实际边界。
窗口层不承担 DOS/WOW policy、BOP dispatch、DPMI、descriptor、Console
文本渲染、BaseSrv task scheduling 或任务管理。

不引入 KVM、hypervisor、driver、内核模式、CPU30、guest binary patch、
任意外部项目的整目录复制或单独 GUI 服务。任何需要扩展现有来源政策
例外、改变 BOP ABI、修改 guest 介质、或导致既有 DOS/Console 回归的
发现均为停止条件，需重新取得 owner 方向。
