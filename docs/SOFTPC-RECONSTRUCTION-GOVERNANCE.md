# SoftPC 重建治理决议

状态：当前执行主线，2026-08-08。

## 目标

`ntdos64` 的唯一运行时主线，是在本仓库内以 OpenNT NT4 的历史职责链为
基础，修复、补完并组合 SoftPC/CCPU 与现代宿主接缝，最终形成研究性的、
非侵入式 CLI `ntvdm.exe` 样本。目标链固定为：

```text
CCPU/SoftPC monitor -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS
```

这里的“补完”指原始输入已缺失、但调用方、数据布局、失败语义和相邻历史
实现足以界定的 SoftPC 语义恢复。它不是另写 DOS runtime、另写 BOP/SVC
dispatcher，亦不是把早期的自写 CCPU/BIOS/DEM 探针提升为正式路径。

## 来源角色

| 来源 | 角色 | 不允许成为 |
| --- | --- | --- |
| OpenNT NT4 | 正式历史基线与实现主体 | 可随意改写的素材 |
| OpenNT-4.5、Win2000SRC、winxpscodes、MinNT | 差异、缺损接口与 ABI 的比较证据 | NT4 行为的默认替代品 |
| NTVDMx64 | 现代移植线索与构建/宿主差异证据 | 本项目运行后端、链接依赖或执行 gate |
| ntvdm64 | 只读的未来架构比较对象 | SoftPC 修复的前置条件、运行时依赖或替代 CPU |
| Bochs、PCjs 等 | x86/PC 语义的交叉校验与测试灵感 | 复制进来的执行后端或未注明来源的实现 |

参考实现只能帮助回答“原始调用方要求的语义是什么”。任何本地补充实现都必须
独立编写，并记录调用点、证据、输入域、失败域和验证夹具；不得把外部工程代码
直接并入正式实现。

本地代码的位置由职责决定，而不是由“方便链接”决定：缺失的 SoftPC/CCPU
语义恢复放在 `src/opennt/overlay/` 中与原 owner 对应的历史目录；仅当已经抵达的
原始调用方要求一个已废弃的宿主 API、context、event、console、process、filesystem
或 timing 接缝时，窄的现代 host seam 才可放在 `src/opennt/local/compat/host/`。后者只
适配宿主边界，绝不拥有或重写 CCPU、BIOS、BOP、DEM、DOS 或 PC/AT 语义。

对于 NT4 x86 V86 profile 未保存、但 S3 已逐条以原始 CPU caller 限定的 x64 software
execution mechanics，可在独立的 `refs/archive/reconstruction/monitor/` 受限目录重建。该目录
只拥有 CPU/memory/stop mechanics，并必须通过明确 bridge transport 调用历史侧
`BIOS[]`；它不得拥有或重写 BIOS、BOP、DEM、DOS、IVT/BDA、设备或 host policy。每项
新增实现必须同时登记在 manifest、治理台账和 S4 准入记录。

## 执行规则

1. 优先恢复原函数边界和原数据流。现代 shim 仅填补现代 Windows 缺失的 host
   API、context、event、console、process、filesystem 或 timing 接缝，不能重定义
   DOS SVC、BOP、PC/AT 设备语义或启动成功条件。
2. 每个缺损接口先做 caller-first 审计，再定义最小受控实现和正反夹具。未知输入
   必须停止或返回原调用方可识别的失败，不可猜测成功。
3. `refs/archive/legacy-adapters/nvtdm.c` 及既有自写 CCPU lifecycle adapter、单项 BOP/DEM handler、IVT/BDA
   scaffold 均只可作为 trace/instrumentation；它们不得进入 `ntvdm.exe` 的正式
   链接或执行路径。
4. `ntvdm64/core` 只能作为仓库外的只读对比证据。它没有资格阻塞 CCPU/SoftPC
   重建，也不能替代该重建的验收；本仓库 CMake 图不得保留其 source/include/link
   integration target。
5. 从 SoftPC 观察到的中性需求可在运行链取得实证后另行写入 mantle/core 报告；
   该提炼是下游架构工作，不是本仓库当前执行路径的一部分。
6. 所有本地 shim、overlay、fixture 和编译兼容输入必须受
   `LOCAL-CODE-GOVERNANCE-LEDGER.md` 分类管理。未被 manifest 和该账本同时登记的
   本地代码，不得进入任何研究型 runtime target；`trace-only` 与 `archived` 项
   不得因 link closure 或 fixture 成功而升级为正式实现。
7. `ntvdm64`、Bochs、PCjs 与其他比较树不得作为任何本地补充实现的代码来源。
   它们只可被文档或测试说明引用以交叉校验结论；不得进入 CMake source list、
   include path、link library、生成步骤、默认或 opt-in runtime target。

## 范围与排除项

本仓库当前不实现、也不以任何方式预先要求 `core.dll`、`mantle.dll`、
`ntvdm64/core` 或第三方 CPU 后端。它们不是 SoftPC 修复的临时替身，更不是
启动、链接或验收 gate。`ntdos64` 的工作对象始终是 OpenNT 的历史函数边界：
先使该边界在软件模拟 monitor 下具备可审计的语义，再把已观察到的通用需求
交给下游架构讨论。

同样，默认构建之外的 startup trace fixture 只可回答“原始调用链首先到达
哪里、缺少哪个历史 owner 或现代 host 接缝”。fixture 不得因为探针抵达某处
就新增 DOS service、BOP handler、BIOS 设备语义或 CPU 执行替代物；不能作为
`ntvdm.exe` 的启动路径，也不能以其通过来宣称 runtime 已恢复。

## 当前优先序

1. 以 caller-first 审计确定 `c_sas_touch`、`c_VirtualiseInstruction` 的真实 contract；
   `c_effective_addr` 已有受控 overlay，不扩大它的职责。
2. 完成历史 CCPU、SAS、BIOS 表与 host foundation 的实际 link/runtime closure，建立
   可重复的软件 monitor 初始化和执行切片。
3. 按首次实际抵达的原始 caller，引入最小现代 host shim，逐步贯通
   `CCPU/SoftPC -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM`。
4. 在该链上以 BYOB 的 `NTIO.SYS -> NTDOS.SYS -> COMMAND.COM /C` 进行有界 smoke
   test；未实现的设备或宿主能力必须形成可记录 stop，而不是合成 DOS 成功。
5. 仅在第 1--4 项取得直接历史链证据后，才可整理一份下游 handoff，说明已观察到
   的中性能力需求。该 handoff 不在本仓库实现 core、mantle 或替代 monitor。

## 验收语言

“可运行”只表示一个受控切片经由原始职责链到达了已记录的点。除非已取得完整
链路与边界测试证据，不得将单个 link closure、单个 BOP、或单个 guest 工具的
构建成功描述成 NTVDM/NTDOS 已恢复。
