# Guest Artifact Manifest Integrity 2026-08-09

状态：复核完成。本记录只验证本地 retained build manifest 与其当前输出文件的一致性；不证明
零售字节一致、guest boot、host execution，或任何二进制的发布资格。

## 方法

对 `artifacts/toolchain-runs/` 下所有 `*-build-manifest.json`：

1. 读取 `Output`、`ByteCount` 与 `Sha256`；
2. 检查输出文件存在；
3. 重新计算输出长度和 SHA-256；
4. 比较 manifest 记录与实际值。

结果：共 28 份 manifest；`missing=0`、长度不匹配为 0、hash 不匹配为 0。

本次同时补齐此前 `ntio-tools16-opennt-v1/ntio-build-manifest.json` 中遗漏的
`NTIO.SYS` 记录：33,792 bytes，SHA-256
`CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937`。
该条已有原始 MASM/LINK16/RELOC 工具及 link log 记录；本次没有重建、修改或替换其二进制。

## 计数边界

28 是 manifest 数，不是“NT4 guest 已重建文件数”。正确分类为：

| 范围 | 数量 | 含义 |
| --- | ---: | --- |
| NT4 direct DOS source-built payload | 22 | 历史工具链重建的 DOS `.SYS`/`.COM`/`.EXE`。 |
| WOW16 guest-adjacent drivers | 5 | 原始 MASM/LINK16/RC16 路径；不属于 33 文件 DOS payload。 |
| 独立 MS-DOS 6 research sample | 1 | `MSHERC.COM` 的来源比较实验；不拥有 NT4 provenance，绝不计入 NT4 closure。 |

NT4 direct DOS payload 的完整分类仍是 33 文件：22 项 source-built、7 项静态伴随数据、4 项
仅有 `binplace` 输入的原始二进制。权威范围与非主张见
`NT4-GUEST-ARTIFACT-CLOSURE.md` 和 `M76-NT4-GUEST-CLOSURE-EVIDENCE-AUDIT.md`。

## 结论

现存输出和 manifest 没有检测到漂移。此结果仅闭合「已记录构建产物的可复核性」；它不会减少
SoftPC/CCPU host reconstruction、完整历史 host composition 或 `nvtdm.exe` guest execution
所需的剩余工作。
