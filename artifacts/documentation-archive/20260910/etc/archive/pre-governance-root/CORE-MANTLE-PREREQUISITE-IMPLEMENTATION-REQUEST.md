# Core 缺失能力实施任务报告

**面向：** `ntvdm64` core 维护者
**目的：** 为未来 `mantle` 提供必要的中性机器语义。
**本报告不要求实现 mantle、DOS、VDM、NT4 profile 或任何外部 wrapper。**

## 结论

`core` 不是缺少一台完整的 DOS 虚拟机。现有的 CPU、RAM、A20、ROM、
reset、run、entry plan、checked physical memory、provider freeze 已足以成为
基础机器。

在 `mantle` 可以正确组装之前，core 缺少且只缺少以下两项**通用机器能力**：

1. **configured decoded-transition**：在正式 CPU 取指/解码边界观察预先
   注册的字节模式，并以受限结果继续、停止或故障；
2. **paused ordinary-RAM transaction**：在停止/暂停边界对多个普通 RAM span
   作全有或全无的复制提交。

这两项应先作为 core 内部/实验性合同完成并有 core corpus 证明；后续 mantle
和 NXVDM 才是它们的真实产品消费者。第二 consumer 是冻结长期 ABI 的依据，
不是阻止 core 先实现中性语义的理由。

## 当前已有基础

当前 `ntvdm64` 已有下列可复用设施，不应重做：

| 现有设施 | 当前位置 | 处理方式 |
| --- | --- | --- |
| CPU create/reset/run 与有限 stop/fault | `src/core/machine/machine_interface.h` | 保留 |
| checked physical read/write/query | `src/core/machine/memory_interface.h` | transaction 必须复用其 route 语义 |
| A20 与 immutable ROM mapping | `src/core/machine/memory_interface.h`、`rom_mapping_interface.h` | 保留 |
| stopped entry plan / preload | `src/core/machine/entry_plan_interface.h` | 保留；它不是一般运行期 transaction |
| real-mode `#UD` transition | `undefined_instruction_transition_interface.h` | 保留；它不是本任务的 decode facility |

现有 `core_machine_register_undefined_instruction_transition()` 仅在 CPU 已发生
real-mode `#UD` 后被调用。它不能观察一条本来可执行的已配置指令流，也不能
作为新接口的别名或偷偷扩展其语义。

---

## Task C1：Configured Decoded Transition

### 目标

在 CPU 已到达一条 guest instruction-stream 的起始边界、尚未执行正常 decoder
之前，core 根据冻结注册表匹配一个有限字节 pattern。匹配命中时，core 把**复制的**
执行状态交给 observer，并由 core 决定有限 continuation。

该能力是通用 instruction-stream gateway；core 不知道任何 profile、DOS、BIOS、
文件或 host policy。

### 建议新接口

命名可按代码库习惯调整，但语义不得变更：

```c
#define CORE_MACHINE_DECODE_PATTERN_MAX_BYTES 15u

typedef enum core_machine_decoded_transition_outcome {
    CORE_MACHINE_DECODE_UNHANDLED = 0,
    CORE_MACHINE_DECODE_HANDLED_RESUME,
    CORE_MACHINE_DECODE_STOP,
    CORE_MACHINE_DECODE_FAULT
} core_machine_decoded_transition_outcome;

typedef struct core_machine_decoded_transition_input {
    uint32_t physical_fetch;
    uint16_t cs;
    uint32_t eip;
    uint8_t bytes[CORE_MACHINE_DECODE_PATTERN_MAX_BYTES];
    uint8_t available_bytes;
    uint8_t matched_length;

    uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp;
    uint32_t eflags;
    uint16_t ds, es, ss, fs, gs;
    core_machine_execution_mode mode;
} core_machine_decoded_transition_input;

typedef struct core_machine_decoded_transition_patch {
    uint32_t write_mask;
    uint32_t eax, ebx, ecx, edx, esi, edi, ebp;
    uint32_t eflags;
} core_machine_decoded_transition_patch;

typedef struct core_machine_decoded_transition_response {
    core_machine_decoded_transition_outcome outcome;
    uint32_t fault_detail;
    core_machine_decoded_transition_patch patch;
} core_machine_decoded_transition_response;

typedef C_VOID (*core_machine_decoded_transition_consumer)(
    C_VOID *owner,
    const core_machine_decoded_transition_input *input,
    core_machine_decoded_transition_response *out_response);

type_status core_machine_register_decoded_transition(
    core_machine *machine,
    const uint8_t *pattern,
    uint8_t pattern_length,
    core_machine_decoded_transition_consumer consumer,
    C_VOID *owner);
```

`core_machine_execution_mode` 可为只读的 `REAL`、`PROTECTED`、`V86` 枚举；它不
授予 observer 切换 mode 的能力。

### 注册与匹配规则

1. pattern 长度必须在 `1..15`；零长度、超长、空 pattern、空 consumer、空 owner
   必须拒绝。
2. 仅 machine configuration-open 阶段允许注册；freeze 后注册表不可变。
3. 完全相同 pattern 必须拒绝。
4. 任意前缀重叠必须拒绝。例如 `AA BB` 与 `AA BB CC` 不可并存。这使命中无顺序
   依赖。
5. 匹配点是 CPU 的**实际物理取指地址**，即经正式 CS/mode/A20/mapping 语义后的
   fetch；不可让调用者给出 linear address，也不可用 RAM 容量或外部 memory read
   近似。
6. 需要的字节不足以判断一个候选 pattern 时，不调用 consumer；保留原有 fetch/
   fault 行为。`available_bytes` 只说明 core 实际安全取得的字节数。
7. 模式匹配发生在 instruction-stream 起始边界而非任意中间字节。pattern 可以是
   profile 自己定义的有限 stream，但 core 不对它赋予语义。

### continuation 与原子性

consumer 只能产生四种 disposition：

| Outcome | core 行为 |
| --- | --- |
| `UNHANDLED` | 不修改 CPU 状态；回到正式 decoder，按原 x86 语义执行。 |
| `HANDLED_RESUME` | core 固定把 IP/EIP 前进 `matched_length`，然后应用经验证 patch 并继续。consumer 不可指定消费长度。 |
| `STOP` | core 在该 instruction boundary 停止；不执行被匹配 stream。 |
| `FAULT` | `fault_detail != 0` 才合法；core 记录结构化 fault 并停止。 |

必须满足：

- consumer 得到的是 copied state；不得得到 `t_cpu *`、executor state、RAM pointer、
  mapping/provider pointer 或 device pointer。
- patch 只能写经 `write_mask` 明示许可的 GPR 和受限 FLAGS 位。推荐第一版只允许
  `EAX/EBX/ECX/EDX/ESI/EDI/EBP` 与 arithmetic status FLAGS；`ESP`、segments、
  descriptor cache、CRx、mode、IRQ、device、memory mapping 一律不可写。
- core 必须在修改任何寄存器、FLAGS 或 IP/EIP 前验证 outcome、mask、所有保留位和
  fault detail。任何非法响应等价于未处理或明确 typed error，但不得留下部分 patch。
- CPU mode transition 永远只能由 core 的正式 x86 指令语义完成；该接口不得有
  `switch_mode`、`set_segment`、`set_cr*` 或任意 IP setter。

### 必需 core tests

1. zero、16-byte pattern、NULL input/consumer/owner 拒绝。
2. duplicate 与 prefix-overlap 双向拒绝。
3. configuration-open 注册成功，freeze 后失败。
4. RAM、ROM、A20 wrapped alias 各自命中时 `physical_fetch` 与 CPU 实际 fetch 一致。
5. pattern 位于可读范围末尾时，不因强行读取 15 bytes 改变正常 CPU fault。
6. `UNHANDLED` 的寄存器、FLAGS、IP 和普通 decoder 结果与未注册时一致。
7. `HANDLED_RESUME` 精确消费 registered length，不能由 callback 篡改。
8. 每个允许 patch 位都可提交；禁止位、非法 outcome、零 fault detail 均无部分状态变更。
9. `STOP` / `FAULT` 的 run result、fault diagnostic、CPU state 定义明确。
10. 既有 `undefined_instruction_transition` corpus 必须不变，证明两接口语义未混淆。

---

## Task C2：Paused Ordinary-RAM Transaction

### 目标

提供独立、复制值、无 raw pointer 的多 span RAM 发布机制。它只解决“多个普通 RAM
写要么全部可见、要么全部不变”，不处理 profile 数据、文件、设备或 CPU mode。

### 建议新接口

```c
typedef struct core_machine_memory_transaction
    core_machine_memory_transaction;

type_status core_machine_memory_transaction_begin(
    core_machine *machine,
    core_machine_memory_transaction **out_transaction);

type_status core_machine_memory_transaction_stage_copy(
    core_machine_memory_transaction *transaction,
    uint32_t physical,
    const C_VOID *bytes,
    STD_SIZE_T byte_count);

type_status core_machine_memory_transaction_commit(
    core_machine_memory_transaction *transaction);

void core_machine_memory_transaction_abort(
    core_machine_memory_transaction *transaction);
```

接口也可采用单次 `commit(machine, spans, count)` 形式；无论形状如何，core 必须
拥有 staged byte copy，不能保存调用方 buffer 的生命周期。

### 生命周期与验证规则

1. `begin` 仅在正式 `STOPPED` 或 `PAUSED` boundary 成功；running/configuration 阶段
   必须拒绝。
2. `stage_copy` 的 `byte_count == 0` 必须拒绝；`physical + byte_count` 溢出必须视为
   inaccessible range，不能回绕。
3. 每个 span 都必须经现有 `core_machine_memory_query(..., WRITE, ...)` 的同一路由
   验证，且结果必须为 `ORDINARY_RAM`。
4. provider、ROM、device-backed mapping、immutable mapping 一律拒绝；transaction
   验证或 commit 不得调用 provider data callback。
5. span overlap 一律拒绝，避免隐含 last-write-wins 语义。
6. commit 前须完成全部 range、route、lifecycle、copy-resource 预检。任何失败前，
   不得写入第一个 byte。
7. commit 成功时所有 staged span 一次发布；失败时全部 guest bytes 保持 pre-commit
   snapshot。
8. resume、reset、stop、fault、machine destroy、execution-context generation 变化后，
   pending transaction 必须 stale，不能再 commit。
9. abort 可重复调用；commit/abort 后 handle 进入 terminal state，不可再次 stage 或
   commit。
10. reset 不得改写 immutable ROM；transaction 本来就不得针对它建立。

### 与 C1 的组合规则

当 decoded-transition consumer 需要同时发布 RAM 和返回 GPR/FLAGS 时，不能先
commit transaction、再应用 patch。应提供内部组合路径：

```text
validate response
  + validate staged transaction
  + validate lifecycle generation
        -> one atomic boundary commit
             RAM spans + permitted CPU patch + fixed IP consumption
        -> resume / stop / fault
```

这可由 transaction attach 到 response，或由 core transition context 管理；实现细节
可选，但跨越该 boundary 的观察者不能看到“新 RAM + 旧 FLAGS”或“新 FLAGS + 半套 RAM”。

### 必需 core tests

1. zero span、overflow、NULL bytes、空 transaction、错误 lifecycle。
2. RAM 成功；ROM/provider/device/immutable mapping 经真实 routing 拒绝。
3. 单 span、多个不相邻 span 成功；各类 overlap 拒绝。
4. 任一 span 预检失败时，所有之前 span 的 RAM 内容不变。
5. 注入复制/提交失败时验证全部 rollback。
6. 成功提交后所有 span 精确可见且仅一次。
7. paused -> resume、reset、fault、stop、destroy 各自使 pending handle stale。
8. transaction 与 decoded transition 的合法 patch 联合提交成功。
9. transaction 与非法 patch/outcome 联合提交失败时，RAM、IP、GPR、FLAGS 全部不变。
10. existing entry-plan/ROM/reset corpus 继续通过，证明 transaction 未改变其语义。

---

## 明确非目标

本任务不得新增下列内容：

- DOS、NTDOS、NTIO、BOP、SVC、DEM、COMMAND、selector 或任何 profile 名称；
- 路径、文件、目录、host handle、Win32 process、console、registry、TEB、VDM_TIB；
- HAXM/WHVP/V86/VM-exit 接口；
- raw guest pointer、CPU 内部对象、C++ ownership 或 CRT allocation 跨边界；
- 任意 CPU mode/segment/CRx 修改接口；
- 新 PC/AT device、BIOS 服务、FDC/HDC/CMOS 策略；
- 为某个外部 wrapper 伪造的 BOP/SVC dispatcher。

## 实施后交付物

core 维护者完成 C1/C2 后，应提供：

1. 所有新公共头、实现文件和 source ownership 位置；
2. focused core CTest 名称与通过结果；
3. 每项 capability 的一个非 DOS、第一方 consumer 计划；
4. 生命周期、错误码、copy ownership、thread/pause boundary 的正式说明；
5. 一个最小 core-only fixture，证明 copied-value registration/transaction，
   不需要本报告所在项目、历史源或外部 profile。

完成 C1/C2 并不等于实现 mantle，更不等于可运行任何 NTDOS binary。它只是让 mantle
第一次能够在不重复 CPU/memory semantics 的前提下被正确组装。
