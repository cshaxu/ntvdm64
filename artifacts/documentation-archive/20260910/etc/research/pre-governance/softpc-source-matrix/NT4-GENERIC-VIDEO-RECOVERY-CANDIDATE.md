# NT4 Generic Video Recovery Candidate

状态：候选采证，未准入，2026-08-09。本记录提出一个可检验的历史组合假设；它不是 overlay、
构建规则或 runtime 实现授权。

## 已知 81 槽 ABI

固定 NT4 `base/cvidc/evidgen.h` 明确定义 81 槽 `struct VideoVector`：

- 0--38：getter；
- 39--77：setter；
- 78--80：`setWritePointers`、`setReadPointers`、`setMarkPointers`。

`base/cvidc/vglfunc.c:86-167` 的 `C_Video` 是具有相同完整类型的静态表，按顺序明确
初始化 0--77 槽为 `S_0363...S_0440` generated C-rules providers；aggregate initialization
使其 78--80 槽为零。

`base/cvidc/sinit013.c:204-333` 同时有 `S_0441...S_0443` generated C-rules wrapper，但它们的
原始函数形式为四个 `IUH` 参数、返回 `IUH`，不匹配 78--80 槽所要求的 C ABI（分别为无参数
void、一个 `IUH` 参数 void、一个 `IUH` 参数 void）。因此这些 wrappers 不能靠 function-pointer
cast 直接填进 `VideoVector` 尾部。

与此相反，`base/cvidc/ev_glue.c` 本身定义了三个恰好匹配 78--80 槽签名的命名 C 函数：
`setWritePointers`、`setReadPointers`、`setMarkPointers`。原对象对 `_Video` 的 relocation 表明，
这些 glue 函数会经完整 generic `Video` 的 0--77 provider 槽调用，而不是经 CCPU 短表。

## 可检验候选

候选 historical composition 是一个**独立的**完整 generic `Video` storage：

```text
generic Video[0..77] = S_0363..S_0440
generic Video[78..80] = ev_glue setWritePointers/setReadPointers/setMarkPointers
C_Video[0..77]       = S_0363..S_0440
C_Video[78..80]      = zero
```

它解释了两个已观察事实：产品 `C_VID` 宏从普通视频代码转到 `C_Video` 或命名 glue 函数；
glue 内部通过 generic `Video` 重入 generated providers。

这不是 `Video = C_Video`、二进制 copy 或 function-pointer cast。若成立，它是两个不同 storage，
各自有静态、typed initializer。

## 尚未解决的 owner 冲突

`ccpu386/ntstubs.c`（固定 `ccpu386/sources` 中的输入）也定义同名 `Video`，但它使用五槽
CCPU ABI。该源文件与完整 generic table 不能共同定义同一 COFF symbol。因此缺失的
`ccpu486.lib`/生成组合仍必须回答至少一个问题：

1. CCPU + C-VID profile 是否排除或替换 `ntstubs.c`；
2. 是否为该 profile 选择了不同的 generated `evidgen.h`/storage owner；
3. 是否通过未保留的 composite member 使 short table 不参与 link；
4. 上述选择发生在何处、何时，且 `Cpu.Video`（如使用）如何绑定。

没有该答案，新增完整 `Video` 将与短 `Video` 发生重复定义，或静默选择一个错误 ABI。这不能被
weak symbol、link order、`/FORCE`、rename、alias 或删除原 owner 解决。

## 准入要求

任何 default-disabled reconstruction fixture 必须先提供：

1. **owner selection proof**：从 archive member、link map、build rule 或独立 profile-aware
   object experiment 证明 short owner 在该组合中不参与；
2. **81-slot typed initializer proof**：逐槽固定为上文的 C-rules/provider 或 glue function，
   不使用 copy/cast；
3. **dual-table proof**：`C_Video` 与 generic `Video` 均可访问且地址不同；
4. **positive fixture**：原 `setMarkPointers(0)` 经 generic table 设置并可从对应 generated
   getter 读取；
5. **negative fixture**：缺少任一 required provider 时 fail closed，不能继续 BIOS/BOP/DEM；
6. **scope proof**：fixture 保持 default-disabled，且不成为 `ntdos64-run`、DOS startup 或
   任何 device shim 输入。

取得这些证据前，该候选仅缩小缺失组合的形状，不能被称为 SoftPC recovered。

## 后代补丁不构成准入证据

本地参考树的 `patches/common/cvidc.patch` 把原声明改为
`struct VideoVector C_Video, Video = { ... };`，而 `ccpu.patch` 注释掉短 `Video`。
按 C aggregate-declarator 规则，initializer 只属于 `Video`，`C_Video` 为零初始化；
该栈也没有改写原 `egacpu.h` 中 `C_VID` 对 `C_Video` 的 ordinary-latch 宏。

更重要的是，该项目的最终 CCPU build rule 选择的是独立手写的 `cvidcnew` library，
不是原 C-VIDC。它最多说明后代维护者尝试过哪些兼容绕行，不能回答本候选所需的
NT4 storage owner、双表初始化时刻或 archive composition。故不得以该补丁满足任何
准入要求，也不得将它复制为 overlay。

## R1 C-VIDC-only typed-vector 采证

`ntdos64-opennt-cvidc-generic-video-vector-objects` 是一个 default-disabled 的 object
target。它仅把原始 `base/cvidc/vglfunc.c` 第二次编译，并将该 translation unit 中的
`C_Video` table symbol 预处理为 `Video`。因此该对象的 0--77 槽仍是原 generated
function-address initializer，而非 `memcpy`、alias、运行期 copy 或手写函数表。

该 target 故意不链接、不生成 EXE、不调用 `ev_glue`，也不引用 CCPU、短 `ntstubs` owner、
BIOS、BOP、DEM 或 DOS。它可证明的范围仅是：原始 C-VIDC initializer 可以以完整 ABI
类型重放到一个独立的 generic `Video` symbol。其 78--80 槽依旧遵循原 `vglfunc.c` 的零值；
它不满足上文的完整 composition 准入要求，更不证明任何历史 product build 的 owner selection。

2026-08-09 在既有 clang-cl i386 research island 构建该 object target 后，
`llvm-nm` 只报告 data definition `_Video`，未报告 `_C_Video`；`llvm-objdump -r` 对该 object
报告 78 个 provider relocation，首尾分别是 `S_0363_CiGetVideolatches_00000000` 与
`S_0440_CiSetVideowrstate_0000004d`。这证明 rename 没有改变 initializer 的 field sequence，
但不扩张该 target 的 C-VIDC-only 证据范围。

## `setMarkPointers` 字段级采证边界

2026-08-09 对原始 `base/cvidc/ev_glue.c:672-705` 与其 COFF relocation 的复核，进一步把
78--80 中 `setMarkPointers` 所需的输入收敛为下列精确链路：

```text
setMarkPointers(markset)
  -> generic Video.SetVideomark_{byte,word,string}   (ordinal 58--60)
  -> one of {simple,cga,unchained,chain4}_mark_evid   (base/cvidc/evidfunc.c)
  -> original S_2126 ... S_2138 generated mark functions
```

- `markset` 仅接受源代码明确列出的 `0`（simple）、`1`（CGA）、`2`（unchained）和 `3`
  （chain4）；其他值落出 `switch`，不作写入。
- 该函数每个已列分支都按 byte、word、string 的固定顺序写入三个 full-vector setter；
  原对象在这 12 次写入位置均对 `_Video` 产生 `DIR32` relocation。函数本身不读取
  CCPU short `Video`，不触及 `Gdp`、`Sas`、BIOS、BOP、DEM 或 DOS。
- 四个 `*_mark_evid` 不是本地测试常量：它们由原始 `evidfunc.c:833-861` 定义，其字段又
  指向 `S_2126...S_2138`。因此，使用本地哨兵结构来让 `ev_glue.c` 链接，会绕过恰待恢复的
  C-VIDC provider 输入，不能作为 positive fixture 或历史初始化证据。
- 空的 `Video` storage 也不是可安全调用状态。原函数没有 null guard；任何未来的
  default-disabled fixture 必须先由原始、完整的 provider/storage 组合证明三个 setter 已
  初始化。它不得把空指针崩溃改写为 runtime fallback，更不得继续推进 BIOS/BOP/DEM。

这项采证只回答 full generic vector 的三个字段如何被原始 glue 消费；它既不解决
`ccpu486.lib` 的 storage-owner/profile selection，也不授权创建 `Video` 的运行时绑定。
下一步可接受的工作仅是以原始 `ev_glue.c`、`evidfunc.c` 与其所需 generated providers
组成 default-disabled 的完整 C-VIDC object closure，并在链接前先取得 owner-selection
证据；不得用本地替身缩短该依赖链。
