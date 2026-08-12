# SoftPC 启动 Host 闭包记录

状态：进行中；仅记录默认关闭的 bounded trace 证据。

## 本次事实

启动边界 fixture 按以下顺序调用历史 owner：

```text
checkpoint(1) -> gfi_init() -> checkpoint(2) -> config()
```

其中 `gfi_init()` 位于 OpenNT 的
`base/mvdm/softpc.new/base/disks/gfi.c`，会调用 `gfi_empty_active()` 为 A/B
软盘填充空服务器函数表。后者的原始实现位于
`base/mvdm/softpc.new/base/disks/gfi_mpty.c`。此前稀疏的 trace link closure
没有选入该 owner，因而调用不安全；现已仅为该 fixture 选入原始
`gfi_mpty.c`。

两个更窄的对照 fixture 已排除 GFI 自身：一个以记录 seam 验证 `gfi_init()` 恰好
请求一次 `C_FLOPPY_SERVER = GFI_REAL_DISKETTE_SERVER` 后返回；另一个以原始 NT
`config.c::host_runtime_set()` 作为该调用的 owner，同样返回。由此可知，stage 2
未在十秒内抵达 checkpoint 的原因不在 `gfi_init()`、`gfi_mpty.c` 或该 NT host
callback。本阶段只能把它归类为更宽 startup fixture 的组合/链接诊断，不能把
`stf_conf.c` 的另一套 SoftPC host 状态机移植过来，也不能藉此创造软盘运行时。

第三个对照保留 startup fixture 的其余对象集合、宏与历史 archive，但只调用
`gfi_init()`，同样正常返回。因此“组合/链接诊断”已进一步收窄为
`config_prefix_trace_fixture` 自身在 `gfi_init()` 前建立的 PIF、direct-CLI、控制台
标题和 `setjmp` harness。该 fixture 的 cleanup 亦已修正：stage 1/2 在尚未调用
`sas_init()` 时不再调用 `sas_term()`。这没有消除 stage 2 的异常，故下一步是拆分
harness 的预置步骤，而不是继续检查 SoftPC GFI。

进一步的最小 `gfi_init -> checkpoint -> longjmp` fixture 已通过。与此同时，完整
harness 的标题缓冲区已改为零初始化：先前无控制台时把未初始化缓冲区交给标题恢复和
比较逻辑，会把等待表现掩盖为不稳定的未定义行为。修正后 stage 2 呈现稳定 access
violation，仍只属于该 PIF/transport/title teardown harness 的未闭合诊断，不可作为
SoftPC、GFI、CPU 或 guest 的失败证据。

## 约束

1. `gfi_mpty.c` 是历史 OpenNT 输入，不能被新的软盘语义替代。
2. 本 fixture 不枚举或屏蔽宿主 A:/B:，也不提供真实/镜像软盘策略。
3. 不得为了通过 stage 2 在 fixture 中补写 `host_runtime_set`、BOP、DEM、BIOS 或
   DOS 行为。尤其不得将 `stf_conf.c` 的非 NT host 状态机当作 `config.c` 的补丁。
4. 外部 `ntvdm64`、Bochs、PCjs 只能协助解释语义；它们不得链接进此 fixture 或
   充当修复方案。

## 下一步

对比工作已确认 NT4、NT4.5 与 NT5.1 的 `config.c` 都维持相同的最小 NT host
runtime 字段；完整 `stf_conf.c` 不是可直接替换的 NT owner。下一步应将
`config_prefix_trace_fixture` 的预置分为独立、无 GFI 的 PIF/transport/title/
`setjmp` 检查，定位 stage-2 等待的第一个 harness 操作。只有真实 caller 证明某个
现代 host API 缺失时，才允许新增 overlay。
