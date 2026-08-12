# GFI 媒体 Host 边界

状态：已完成原始路由 trace，2026-08-08。

## 已证实的职责链

在 CCPU 软件执行 research profile 中，原始 `config.c` 会为 A、B 两个软驱调用：

```text
config()
  -> gfi_floppy_active(hostID, TRUE, NULL)
  -> host_gfi_rdiskette_active(hostID, TRUE, NULL)
```

前一项来自原始
`src/opennt/base/mvdm/softpc.new/base/disks/gfi.c`。它没有实现 FDC、INT 13h、
DMA、DOS 设备或 BOP；它是把 SoftPC 的通用软驱请求路由到当前 host 后端的薄层。

`host_gfi_rdiskette_active` 的 NT4 实现位于原始
`src/opennt/base/mvdm/softpc.new/host/src/nt_rflop.c`。当 `active` 为真时，它调用
`nt_gfi_rdiskette_init`，后者以 `\\DosDevices\\A:` / `\\DosDevices\\B:` 打开
物理软驱并查询介质几何信息。这是当时 Windows NT 物理设备策略，不是 NTDOS
二进制的启动 ABI。

## 受控验证

默认关闭目标 `ntdos64-opennt-config-x86-gfi-trace-fixture` 连接原始：

- `config.c`；
- `nt_umb.c`；
- `gfi.c`。

它在原始 GFI 调用 host 后端的瞬间由 `host_gfi_rdiskette_active` trace guard
停止。执行退出码为 `0`，同时验证原始 `sas_term` 回收 RAM aperture。为使 `gfi.c`
其余未执行导出函数可链接，`host_gfi_rdiskette_valid`、
`host_gfi_rdiskette_change` 与 `gfi_empty_active` 同样只能 fail-closed；它们不
提供设备响应。

## 已验证的无副作用 trace 策略

`src/opennt/local/trace/fixtures/media_capability_absent.c` 只提供无副作用 trace
policy：保留历史函数边界
`host_gfi_rdiskette_active(hostID, active, err)`，但在 `active == TRUE` 时
返回历史 `C_CONFIG_NOT_VALID`（`-1`），并在停用请求时返回
`C_CONFIG_OP_OK`（`0`）。它不枚举、不探测也不打开 host `A:` / `B:`。该文件
仅由显式命名的 trace fixture 编译，绝不可进入正式 runtime 或 library target。

这不是正式 modern-host shim，也不能被扩展成“默认永远没有软驱”的产品
策略。当前仓库尚未实现正式 `host_gfi_rdiskette_active` 适配器；在它存在
之前，任何 runtime target 都不得链接 `media_capability_absent.c`。

默认关闭目标 `ntdos64-opennt-config-x86-media-absent-fixture` 连接此 shim 和
原始 `config.c`、`nt_umb.c`、`gfi.c`。结果是：原始 `config()` 在两次 A/B
软驱查询后自然返回；夹具随后调用原始 `sas_term`，并以退出码 `0` 验证 RAM、
临时 PIF 与 console 状态均已清理。

这不是可供运行时采用的媒体实现；它只表达 trace 中的“无软驱”，不提供映像
挂载、FDC 命令、读写或格式化。

## 对研究型运行时的含义

完整运行时不应把 `nt_rflop.c` 的“必须访问物理 A:/B:”策略当作基础能力。一个
现代、非侵入式的 host media capability 至少必须区分：

- `auto`：探测可访问的 Windows `A:` / `B:` DOS 卷；传统 FDC、USB 软驱和
  虚拟软驱均可作为宿主驱动器，存在且可访问时应激活；
- `disabled`：由产品配置明确禁用，才报告 inactive；
- `host-drive`：明确绑定到可访问的 `A:` 或 `B:` 卷；
- `image-backed`：由产品层注入镜像后端，不能把镜像路径或文件策略塞入 GFI。

其中 `auto` 的判定对象是 Windows 可访问的 `A:` / `B:` DOS 卷，而不是
“传统内置 FDC”这一硬件类别。因此 USB 软驱、虚拟软驱和传统 FDC 只要卷可访问，
均须作为可激活候选；只有卷不存在、访问被拒绝或产品配置明确禁用时才报告
inactive。正式适配器还必须分别表示“驱动器存在”与“介质已插入”，不可用
一个固定 `inactive` 返回值混淆两者。

上述策略还应显式表达：

- A/B 是否存在；
- 媒体是否已插入；
- 几何与只读状态；
- 读、写、格式化、弹出等结果；
- 没有、不可访问或被 `disabled` 的介质时的历史等价失败结果。

该 capability 后续可以由 BYOB 软盘映像、目录映射或无软驱 profile 提供。GFI
路由层和 `nt_rflop.c` 的设备后端必须保持两个独立边界，不能用自写
BIOS/DEM handler 绕过它们。
