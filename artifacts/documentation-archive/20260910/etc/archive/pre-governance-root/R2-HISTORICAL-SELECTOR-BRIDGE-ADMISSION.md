# R2 Historical Selector Bridge Admission

状态：caller-first 准入报告，2026-08-09。

## 范围

本报告只处理第一个历史侧采证目标：让一个 i386 闭包在原始
`CCPU -> BIOS[50h] -> MS_bop_0 -> DemDispatch` 上完成无副作用 date-service
调用，并把结果以固定宽度 record 交给未来的 R2 bridge。它不授权 NTIO、NTDOS、文件、设备、
DOS namespace 或任意 SVC 的实现。

## 已执行证据

`ntdos64-opennt-original-selector50-trace-fixture` 在独立 i686 Ninja island 中已构建并以
退出码 0 运行。其 guest stream 为：

```text
C4 C4 50 14    selector 50h, SVC_DEMQUERYDATE
C4 C4 FE       original CCPU invocation exit
```

原始 CCPU 的 C4 BOP 解码与 IP 更新位于
`base/mvdm/softpc.new/base/ccpu386/c_main.c:3294-3352`；`FE` 的 unsimulate exit 位于
`c_main.c:3340-3342`，执行 loop 在 `c_main.c:4831`。`bios.h:179-183` 定义的
`bop(n)` 是 `(*BIOS[n])()`，而不是一个可跨 ABI 导出的 dispatcher 函数。

## 当前 fixture 不能晋级的原因

该 trace target 仍链接
`local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c`。它提供了一个本地 RAM aperture，
但同时用 `abort()` 拦截 `c_sas_touch`、`c_VirtualiseInstruction`、`EDL_fast_bop`、IRQ、timer、
mouse、host simulation 和其他 callback；并以本地返回值伪装 `config_inquire` 与
`host_read_resource` 的受限情形。

它的作用是证明该 fixture 没有碰到未准入边界，并非说明这些边界已经实现。因此：

- 它只能留在 `archived/legacy-adapters` + `trace/fixtures` 图中；
- 不能被移动、改名或重新分类为 bridge/runtime；
- 未来 bridge 不得链接它，且不得复制其空成功返回或 `abort()` 语义。

## 原始 owner 与缺口

| 现有 fixture 需求 | 原始来源 | 准入结论 |
| --- | --- | --- |
| CCPU init、C4 解码、simulate/unsimulate | `ccpu386/c_main.c` | 已有原始 owner；R2 应以其行为为 oracle。 |
| selector table | `base/bios/bios.c` | 已有原始 owner；必须整表保留，不能局部重写 50h。 |
| BOP service-byte fetch/DEM dispatch | `host/src/nt_bop.c` | 已有原始 owner；必须由历史侧调用。 |
| date service | `dos/dem/dem*.c` | 已有原始 owner；当前只作为无副作用采证。 |
| SAS allocation | `host/src/sim32.c:325-445` 或 x86 profile 对应 owner | 必须先选择并证明一个一致的 historical profile；不得从 fixture aperture 提升。 |
| effective address | `overlay/.../ccpu386/c_effective_addr.c` | 已有受控 reconstruction，但只限其已审计的 selector/descriptor contract。 |
| `c_sas_touch`、`c_VirtualiseInstruction` | CCPU `sascdef.c:137-139` 只给出 vector slots | 尚无活跃 caller contract；继续 fail-closed，不能为 bridge 猜测成功语义。 |
| config/resource policy | `host/src/config.c`、`host/src/nt_rez.c` | 尚未按 runtime caller 准入；不能使用 fixture 的固定 absent 值。 |

## 最小 bridge record

未来 bridge 必须是一个**单独的 i386 historical worker** 的受控入口，而不是把 `BIOS[]` 或
CCPU/SAS pointer 暴露给 x64。第一版 record 只能包含：

- ABI magic、version、request/response structure size；
- selector（仅允许 50h）、显式 service byte（首次仅允许 14h）；
- 固定的、可审计的 initial register/memory profile identifier；
- disposition、next IP 和 date-service 的 AL/DH/DL/CX 输出；
- historical worker 的 unexpected-callback/fault code，绝不传递 host pointer。

record 不包含任意 guest buffer、DOS 路径、文件 handle、函数地址、C++ object、CRT allocation
或未标注可变长 payload。请求字段超出第一个已验证 profile 时，worker 必须拒绝，而不是尝试运行。

## 实现准入门槛

1. 新 target 只可在 i386 historical island 构建，且与 R2 通过 versioned fixed-width ABI 通信。
2. 新 target 不得依赖 `archived/legacy-adapters`、trace-only overlay 或 `/FORCE:UNRESOLVED`。
3. RAM、ROM、config/resource、timer/IRQ 与 host callback 的每一项都必须指向原始 owner 或另有
   caller-first reconstruction 记录；未知 callback 返回显式 bridge stop。
4. 首次验收必须重复当前 selector-50/date 正例，并加入：错误 ABI version、错误 selector、错误
   service、未知 callback 和未映射内存五个负例。
5. 只有这些条件满足后，才可让 x64 R2 以 IPC/RPC 方式消费该 record；这仍不等于运行 DOS。

## 下一步

先为 `host_sas_init` 的实际 profile、CCPU reset 生命周期和固定 initial-state 写一份原始 caller
链证据。该证据必须说明何时分配、何时释放、哪个 owner 处理 ROM/resource 与 reset，之后才可
替换 trace fixture 的 RAM aperture。
