# M92 Overlay Delta Matrix

状态：第二阶段复核完成，2026-08-08。

本矩阵是对 `src/opennt/overlay/` 的机械复核结果，而不是功能验收。每个有同路径
基线的项均使用 `git diff --no-index --numstat` 与
`src/opennt/<path>` 比较；`+/-` 是本地差异行数。每个文件还逐一核验了
`SOURCE-MANIFEST.json` 的 SHA-256 和字节数。根 CMake 未加入 `src/opennt`，故下列
任何项都不进入默认构建或正常 runtime。

| Overlay 相对路径 | 上游同路径基线 | Delta | 治理类别 | 准入 |
| --- | --- | ---: | --- | --- |
| `base/mvdm/dos/dem/demfile.c` | 有 | +11/-22 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/dos/dem/demhndl.c` | 有 | +1/-1 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/dpmi32/i386/dpmimem.c` | 有 | +1/-1 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/inc/mvdm.h` | 有 | +5/-0 | `compat/compiler` | default-disabled compile input |
| `base/mvdm/softpc.new/base/bios/reset.c` | 有 | +19/-0 | `overlay/instrumentation` | fixture-only |
| `base/mvdm/softpc.new/base/ccpu386/c_effective_addr.c` | 无 | 新增输入 | `overlay/reconstruction` | default-disabled reconstruction target |
| `base/mvdm/softpc.new/base/ccpu386/c_main.c` | 有 | +8/-0 | `overlay/instrumentation` | fixture-only |
| `base/mvdm/softpc.new/base/ccpu386/c_xcptn.c` | 有 | +1/-1 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/softpc.new/base/ccpu386/ccpusas4.c` | 有 | +1/-1 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/softpc.new/base/ccpu386/zfrsrvd.c` | 有 | +1/-1 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/softpc.new/base/dos/emm_mngr.c` | 有 | +1/-1 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/softpc.new/base/inc/host.h` | 有 | +1/-1 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/softpc.new/base/keymouse/mouse_io.c` | 有 | +25/-0 | `overlay/instrumentation` | fixture-only |
| `base/mvdm/softpc.new/base/support/ios.c` | 有 | +1/-1 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/softpc.new/base/support/main.c` | 有 | +15/-0 | `overlay/instrumentation` | fixture-only |
| `base/mvdm/softpc.new/base/system/illegalp.c` | 有 | +1/-1 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/softpc.new/host/inc/x86/prod/sas4gen.h` | 无 | 新增输入 | `overlay/reconstruction candidate` | default-disabled compile input |
| `base/mvdm/softpc.new/host/src/config.c` | 有 | +49/-3 | `compat/host candidate + overlay/instrumentation` | trace/isolated only |
| `base/mvdm/softpc.new/host/src/copy_fnc.c` | 有 | +6/-0 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/softpc.new/host/src/nt_bop.c` | 有 | +13/-13 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/softpc.new/host/src/nt_cpu.c` | 有 | +2/-0 | `compat/compiler` | object/link diagnostic only |
| `base/mvdm/softpc.new/host/src/nt_eoi.c` | 有 | +8/-0 | `overlay/instrumentation` | fixture-only |
| `base/mvdm/softpc.new/host/src/nt_error.c` | 有 | +7/-0 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/softpc.new/host/src/nt_event.c` | 有 | +22/-2 | `overlay/instrumentation + compat/compiler` | fixture-only |
| `base/mvdm/softpc.new/host/src/nt_hosts.c` | 有 | +27/-0 | `overlay/instrumentation` | fixture-only |
| `base/mvdm/softpc.new/host/src/nt_msscs.c` | 有 | +37/-1 | `compat/host candidate + overlay/instrumentation` | trace/isolated only |
| `base/mvdm/softpc.new/host/src/nt_reset.c` | 有 | +21/-1 | `overlay/instrumentation` | fixture-only |
| `base/mvdm/softpc.new/host/src/nt_sound.c` | 有 | +5/-0 | `compat/compiler` | default-disabled object gate |
| `base/mvdm/xms.486/xmsblock.c` | 有 | +5/-5 | `compat/compiler` | default-disabled object gate |

## 汇总与处置

- 实体 overlay、manifest 条目、hash/字节数均为 `29/29`；没有漏登、孤儿或哈希漂移。
- 27 个项有同路径固定基线；两个无基线项均已明确为缺失输入，不能借此获得 runtime 准入。
- 类别分布：`compat/compiler` 17，`overlay/instrumentation` 7，混合
  instrumentation/compiler 1，host candidate/instrumentation 2，已确认 reconstruction 1，
  reconstruction candidate 1。
- 所有 `fixture-only`、`trace/isolated only` 和 `object/link diagnostic only` 项均不得
  修改 BOP/SVC、DOS 返回、设备响应或启动成功条件；任何未来 runtime 准入必须以已抵达的
  原始 caller、输入/输出合同和独立治理审计为前提。

详述的语义判定、caller/owner 线索和历史风险保留在
`M92-OPENNT-OVERLAY-AUDIT.md`；本文件只负责让当前树的每一项差异可枚举、可复跑、
可发现漂移。
