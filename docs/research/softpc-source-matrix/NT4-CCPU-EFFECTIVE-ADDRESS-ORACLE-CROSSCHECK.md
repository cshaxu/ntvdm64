# NT4 CCPU Effective Address Oracle Crosscheck

## 目的

本记录只交叉核验已准入的 `c_effective_addr(segment, offset)` 恢复项的职责边界。它
不是把 Bochs 引入 runtime，也不是对 `c_sas_touch`、分页、异常、BOP 或 DOS 服务作任何
新的语义推断。

## 结论

当前恢复项的受限合同与独立 x86 CPU 参考实现一致：在已完成段寄存器/selector 验证的
前提下，它只形成 32 位 linear address。

```text
real mode 或 V86:          (segment << 4) + offset
protected non-V86:         descriptor.base + offset
selector table 无对应项:   0xffffffff
```

尤其，descriptor 的 segment limit、access rights、paging translation 和 physical mapping
**不属于**这个函数。它们必须仍由原 CCPU 后续访问路径决定；将这些检查塞进
`c_effective_addr` 会把“地址形成”错误改写为“内存访问”。

## 交叉来源

只读参考为本机 Bochs 2.6 源码：

- `D:\home\repos.hobby\bochs-2.6-compat\bochs-2.6\cpu\cpu.h:4427-4441`
  的 segment-base accessor 与 `base + offset` 地址形成；
- `D:\home\repos.hobby\bochs-2.6-compat\bochs-2.6\cpu\access.cc:65-88`
  和 `cpu\access32.cc:38-62` 的后续 segment-limit 检查；
- 同一 `access32.cc:1241-1243` 的已验证访问路径中，linear address 由
  cached segment base 加 offset 形成。

这些文件说明 Bochs 把 address formation 与 access validation 分开。它们只能作为
独立语义 oracle；OpenNT 自身的 `selector_outside_table`、`read_descriptor_linear` 和
`c_GetLinAdd` 仍是本项目恢复项的历史 owner 边界。

## Fixture 增补

默认关闭的
`ntdos64-opennt-ccpu-effective-addr-fixture` 现除原有 real/V86/GDT/LDT/invalid-selector
覆盖外，新增两项断言：

1. descriptor limit 很小不改变该函数返回的 linear address；这明确防止它吞掉后续的
   access fault 责任。
2. `0xfffffff0 + 0x30` 返回 `0x00000020`；这固定了 32 位线性加法回绕，而不是 host
   pointer 宽度或 64 位整数语义。

fixture 不执行 BIOS、BOP、DEM、DOS 指令或任意 guest 访问。它不能证明当前 CCPU profile
具备完整 descriptor provider，也不能把该恢复项提升为正常 runtime 输入。

## 关联

- `CCPU-MISSING-INTERFACE-CONTRACT.md`；
- `CCPU-SEMANTIC-RECONSTRUCTION-RECOVERY-MATRIX.md`；
- `SOFTPC-RECONSTRUCTION-GOVERNANCE.md`。
