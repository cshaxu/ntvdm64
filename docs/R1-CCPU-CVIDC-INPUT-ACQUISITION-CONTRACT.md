# R1 CCPU/C-VIDC Input Acquisition Contract

状态：历史组合输入的接收与验真合同，2026-08-09。

## 目的

固定 NT4 OpenNT `obj.vdm/sources` 在 `CCPU != ""` 时只引用
`ccpu486.lib`，而当前可见 source snapshots 没有该 archive、其 member list、map/listing 或
EDL/generator input。本合同定义未来取得候选材料后如何判断它能否改变该结论；它不授权寻找不到
输入时自行拼接 `ccpu386.lib + cvidc.lib`，也不授权实现 generic `Video` provider。

## 可接受的候选证据

以下任一材料可开启新的只读采证项，但都不能直接进入 runtime：

1. NT4 同版本、同 build profile 的 `ccpu486.lib`，或能列出其 COFF member/symbol 的未修改副本；
2. 将 `ccpu486.lib` 放入 `obj.vdm` link 的原始 link map、listing 或 build log；
3. `EvPtrs.edl`、`Vglob.edl`、`Evid.edl`、`MkCpuInt`、`host/genPg` 或同等 generator input，且
   可证明与固定 NT4 profile 的版本关系；
4. 可追溯到同一 build drop 的 CCPU/C-VIDC composition makefile，以及其输入/输出 member list。

仅有以下材料不够：后代 patch、独立 `ccpu386.lib`、独立 `cvidc.lib`、V86 `ntvdm.exe`、测试
harness、单个 generated `.c/.h`、匿名二进制或仅凭文件名相同的 archive。

## 接收与隔离步骤

1. 在 `artifacts/research-inputs/` 下建立只读保存的来源目录；不得先复制到 `src/opennt/`；
2. 记录取得位置、声明版本、原始路径、大小、SHA-256、可用时间戳和许可证/研究限制；
3. 对 COFF archive 记录 machine、每个 member 名、全局定义/未定义符号与是否存在 `Video`、
   `C_Video`、`c_sas_touch`、`c_VirtualiseInstruction`；
4. 对 map/listing 记录完整 link command、library 顺序、build defines 与 `obj.vdm/sources` 的
   差异；对 EDL/generator 记录输入、生成器版本、命令行与生成 C/H 的 hash；
5. 先更新 source matrix 与 evidence ledger。只有原始 profile 和输入链匹配，才可以建立
   `EXCLUDE_FROM_ALL` object/archive audit target；默认 runner、fixture 和 runtime 不得链接它；
6. 只有在 product link composition、storage layout、initializer order 及 caller trace 都通过审计后，
   才能另行讨论 reconstruction admission。

首次接收时应先运行只读检查器：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\Inspect-SoftPcCompositionCandidate.ps1 `
  -CandidatePath <candidate> `
  -OutputDirectory artifacts\research-inputs\<source-id> `
  -ProvenanceNote '<source, version, and research restriction>'
```

该输出只提供 hash、archive member、COFF machine、关键符号和文本命中；它不是 provenance
证明，也不构成迁入 `src/opennt`、CMake、fixture 或 runtime 的准入。审计人仍须按本合同逐项
判断 profile、来源和历史组合关系。

## 当前搜寻边界

已完成的权威范围为固定项目输入、`D:\home\repos.hobby` 下的 OpenNT、OpenNT-4.5、Win2000
private、XP source snapshots，以及已登记的本地 `.lib`/build metadata 审计。它们没有提供可接受的
候选输入。

2026-08-09 对 `D:\archive`、`D:\downloads` 与 `C:\Users\neko\Downloads` 的压缩包/映像和
精确命名候选快速清点没有命中。一次扩展到整个 `D:\home` 的精确文件名递归在 122 秒时超时，
在已遍历部分未产生命中；该结果是**未完成扫描**，不是“不存在”的断言。

后续取得任意新材料时，必须先走本合同；在此之前，R1 继续保持 source-evidence 队列，S5 不以
缺失 composition 为理由增加 shim 或缩减 `BIOS[]`。
