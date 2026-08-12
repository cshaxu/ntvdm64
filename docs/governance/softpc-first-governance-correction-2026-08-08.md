# SoftPC-First 治理校正（2026-08-08）

## 目的

本次校正消除文档中两种容易混淆的叙事：一是把外部 CPU/backend 当作
当前执行候选，二是把未来的 core/mantle 提炼当作 SoftPC 修复的前置任务。
两者均不再属于 `ntdos64` 的当前实现路线。

## 现行路线

唯一可进入 `ntdos64` 正常研究 runtime 的职责链为：

```text
OpenNT CCPU/SoftPC -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS
```

本地代码只能做两类工作：

1. 在有 OpenNT caller、数据布局和失败行为证据时，于原 owner directory 的
   overlay 中补全缺失 SoftPC 语义；
2. 在真实历史 caller 首次触达已失效的宿主接口时，用窄的现代 host shim 保持
   原函数边界与数据流。

它不能自造 DOS runtime、BOP/SVC dispatcher、PC/AT device policy，亦不能把
startup trace fixture 变成能让 DOS 继续前进的 engine。

## 外部来源的固定角色

| 来源 | 允许用途 | 明确禁止 |
| --- | --- | --- |
| `ntvdm64` | 只读架构和语义比较 | source/build/link/runtime dependency，或 SoftPC gate |
| Bochs、PCjs | 受控 x86/PC 语义交叉校验、测试灵感 | 复制实现、bridge、execution backend |
| NTVDMx64 与后续 Windows 树 | host seam、构建差异及历史演化比较 | NT4 默认行为替代、runtime component |
| OpenNT-4.5、Win2000SRC、winxpscodes、MinNT | 缺损接口、ABI 和版本差异证据 | 未经 NT4 caller 证实的实现替代 |

外部源码不进入 `src/opennt` 的正式实现，也不进入 `ntdos64` 的 CMake 链接图。
当参考材料支持某一恢复结论时，记录的是证据和独立写出的窄实现，而不是复制
其代码或采纳其架构。

## 本次文件校正

- `SOFTPC-RECONSTRUCTION-GOVERNANCE.md` 保持唯一执行治理权威；
- `document-map.md` 将 backend/core/mantle 方案类文档统一列为归档跨项目观察；
- `CCPU-SEMANTIC-RECONSTRUCTION-RECOVERY-MATRIX.md`、
  `BACKEND-ADOPTION-READINESS.md`、`CPU-EXECUTION-BACKEND-DECISION.md` 与
  `CCPU-STANDALONE-BACKEND-ASSESSMENT.md` 增加显式归档状态；
- `ROI-DRIVEN-EXECUTION-ROADMAP.md` 中 mantle/core 小节标为下游资料整理，
  不得排入当前队列或成为 gate。

这些文件的既有实验结果仍可被引用为历史证据；其任务推荐、候选选择和旧验收
语言不再有执行效力。

### 补充校正（2026-08-08）

本次复核进一步消除了两个容易在实施时回流的歧义：

1. `src/opennt/overlay/` 只承载 caller-first 证实的缺失 SoftPC/CCPU 语义恢复；
   `src/opennt/local/compat/host/` 只承载已经抵达的原始 host 调用方所需的现代宿主接缝。
   两者均不得承载外部后端、DOS/BOP/BIOS/设备重定义或启动成功捷径。
2. `ntvdm64`、Bochs、PCjs 和同类工程的角色是只读对照。它们不可成为本项目的
   CMake 源文件、include/link 依赖、代码生成输入或 runtime component；即使某段
   行为有参考价值，也必须由 OpenNT caller 证据约束，并在本项目独立实现。

同时，`ROADMAP.md` 中的外部 wrapper 章节被明确降为 deferred questions。它只有在
修复后的 OpenNT software-monitor 链取得真实有界执行证据后才可被单独规划，不能
与 SoftPC 修复并列排期。

## 发现的构建图治理债务

本次是文档治理校正，未改动任何 CMake target。审计发现根
`CMakeLists.txt` 仍默认声明旧的 `whpx-probe`、`whpx-hlt`、`whpx-io`、
`whpx-int` 与 `nt4-mantle-initial-state-test`，且保留相应 CTest 项。更重要的是，
它还保留 `NTDOS64_ENABLE_NTVDM64_CORE_EXPERIMENT`；开启后会把外部 `ntvdm64`
的 source/include path 与 archives 接入 `ntdos64-core-machine-probe`。该开关
虽默认关闭，但已经不符合“比较树不得成为 CMake build input”的校正规则。

这些项目没有进入当前 SoftPC runtime；但不得把“默认关闭”误写成“已经完成隔离”。
它们是明确的 build-graph 治理债务，必须在 M92 中移出当前工程的配置/CTest 图，
而不是保留为可选执行路线。

后续应以一个独立、可验证的 build-graph 清理任务，删除外部 `ntvdm64` probe 的
source/include/link 路径，并将旧 WHPX/mantle probe/测试移出当前配置图。该任务
不得借机引入新 backend，也不得改变 OpenNT SoftPC runtime 的实现顺序。本报告不把
它伪称为已完成。

## 复核准则

后续新增或修改文档时，出现 `ntvdm64`、Bochs、PCjs、WHPX、core 或 mantle，
必须同时满足以下条件：

1. 文字明确其为只读比较或下游信息；
2. 不把它列为当前 task、runtime、build target、link library 或 acceptance gate；
3. 若记录 SoftPC 恢复，能指向 OpenNT 的 caller、owner 和 bounded fixture；
4. 与本记录冲突时，以 `SOFTPC-RECONSTRUCTION-GOVERNANCE.md` 为准。
