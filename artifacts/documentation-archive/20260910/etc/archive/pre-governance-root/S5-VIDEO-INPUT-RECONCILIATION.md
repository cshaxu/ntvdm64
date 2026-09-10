# S5 Video Composition Input Reconciliation

状态：本地候选树只读盘点完成；未发现可用的 historical composition input。

## 结论

原 C-VID source closure 可直接接入，且已由 SM-2 link evidence 证实它消除了 `C_Video`
及一组 C-VID provider unresolved。此项已进入 default-disabled trace target，但不引入任何
local vector binding。

通用 `Video` vector 的历史 binding/initializer 仍为 `unresolved`。它不能由
`C_Video` 自动推导，也不能由同树 `ev_glue.c`、`vglob.c` 或 CCPU `ntstubs.c` 的零初始化
storage 推导。

## T-S5.1 source-closure result

2026-08-09，default-disabled 的 v2 helper 原先编译了原 `video-provider` objects，
却遗漏 `ntdos64-opennt-cvidc-archive`。将该**原始 C-VID source archive**加入 helper 的
link input 后，无 `/FORCE` 的 clean-link diagnostic 从 244 个不同未解符号降为 228，
`_C_Video` 已不再是未解符号。

这只恢复了已证实的 `C_Video` owner；没有定义、复制或赋值通用 `Video` vector。新的
日志为：

```text
artifacts/build/current/opennt-r2-bridge-ninja-i686/
  s5-v2-helper-clean-link-cvid-20260809.log
```

仍未解的 `_getVideo*`/`_setVideo*` accessor 和 glue symbols 属于 generic `Video`
composition 缺口。该变化不得被解释为 video subsystem 已可运行，也不得推进 helper
进入 runtime。

## 本轮只读盘点范围

逐根使用精确文件 token `ccpu486`、`cvidc`、`genPg`、`vglfunc`、`ev_glue` 以及 SoftPC/MVDM
命名的 `.map`/`.lst` 检索：

| 根目录 | 结果 |
| --- | --- |
| `D:\home\repos.hobby\opennt` | 找到 `cvidc/vglfunc.c`、`cvidc/ev_glue.c` 与 guest map；未找到 `ccpu486` archive/map、C-VID archive/map 或 `host/genPg` |
| `D:\home\repos.hobby\opennt-4.5` | 同上；未找到 composition input |
| `D:\home\repos.hobby\win2000src\private` | 未找到 SoftPC CCPU/C-VID tree 或相关 archive/map |
| `D:\home\repos.hobby\winxpsrc` | 找到同源 `vglfunc.c`/`ev_glue.c` 与非 SoftPC map；未找到 composition input |
| `D:\home\repos.hobby\winxpscodes` | 与 `winxpsrc` 同样；未找到 composition input |

这只是在上述本地树内的 not-found，不是对任何历史 build drop、发行介质或其他来源的存在性断言。

## 与既有采证的关系

该结果与 `docs/research/softpc-source-matrix/NT4-NT5-SOFTPC-EVIDENCE-LEDGER.md` 的
`E-BUILD-005`、`E-VEC-001` 一致。后续只接受可追溯的版本匹配 `ccpu486.lib`/C-VID archive
member、link map、listing 或 generator input；不得把 NT4.5/XP 的重复源复制为 NT4 的
composition repair。

## 禁止替代

- 不得写入 `Video = C_Video`；
- 不得新增 fake vector、fake `setMarkPointers` 或 no-op video host shim；
- 不得因完整 BIOS table 静态引用而把外部 emulator/video code 伪装为 SoftPC source recovery。
