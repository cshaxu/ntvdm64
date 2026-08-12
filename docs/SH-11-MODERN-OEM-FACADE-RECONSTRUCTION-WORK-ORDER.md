# SH-11 Modern OEM Facade Reconstruction Work Order

状态：待实现。此任务从 S5 原始 `demLoadDos` 的真实 caller 进入，不是通用
DOS filesystem、BOP 或 DEM 重写任务。

## 事实边界

历史输入是 `base/mvdm/oemuni/file.c` 与 `process.c`，声明面为
`base/mvdm/inc/oemuni.h`。它们共 29 个 Oem entry point。原文件把临时 Unicode
转换缓冲区放在 `NtCurrentTeb()->StaticUnicodeString`，并依赖 NT4 私有 PEB/RTL
布局；现代 Windows 不保证这些字段或布局存在。

当前 S5 已证实：

```text
NTIO C4 C4 50 11
  -> BIOS[50h]
  -> MS_bop_0
  -> DemDispatch(11h)
  -> demLoadDos
  -> CreateFileOem     [当前 host seam]
```

`demLoadDos` 仍须拥有 load segment、DOS 文件名拼接、读循环、`TerminateVDM`
决定、guest-memory 写入与文件生命周期。Facade 只保持原 `*Oem` API 边界。

## 禁止输入

不得迁入或链接：

- `local/archived/legacy-adapters/contained_oemuni.c`；它含有自定义 drive root、
  host 路径 containment、环境表和 DOS 策略，不能作为历史 facade 的实现来源；
- 自写 `demLoadDos`、BOP、DEM service、DTA/FCB/SFT/JFT 或 DOS error mapper；
- host-global current directory、隐式 host drive discovery、物理卷自动暴露；
- 从 NTVDMx64、Bochs、PCjs 或其他 comparison tree 复制代码。

## 目标结构

新增目录为 `src/opennt/local/compat/host/oem_facade_v1/`。它只允许 modern Win32
documented API、固定 `WINAPI/APIENTRY` 调用约定和明确的 per-session profile。
完整 facade object target 必须 `EXCLUDE_FROM_ALL`，并替换 trace target 中原
`ntdos64-opennt-oemuni-provider-objects`；不得进入 root runner 或 CTest。

profile 必须在调用前明确提供：允许的 BYOB resource roots、初始 DOS cwd、可选
临时目录和 process delegation policy。每个路径转换都必须先经过 profile，不得
把 DOS path 原样解释为 host path。对未实现或未配置能力，返回对应 Win32 failure
和 `GetLastError`，而不是 fallback 到宿主全局状态。

## 29-entry 交付矩阵

第一轮应提供完整符号面；下列分组是实施和验证顺序，而不是可以省略的 API。

| 组 | 原函数 | 第一轮策略 |
| --- | --- | --- |
| F1 文件 | `CreateFileOem`, `Set/GetFileAttributesOem`, `DeleteFileOem`, `MoveFileOem`, `MoveFileExOem` | documented OEM conversion + profile-mapped Win32 file operation；保留原 read access 重试语义 |
| F2 目录/查找 | `FindFirst/NextFileOem`, `GetFullPathNameOem`, `Get/SetCurrentDirectoryOem`, `Create/RemoveDirectoryOem` | 只在 profile-mapped cwd/root 内运行；DEM `FileFind*` native search 仍另属 SH-05 |
| F3 volume | `GetDriveTypeOem`, `GetDiskFreeSpaceOem`, `GetVolumeInformationOem` | profile-selected logical root；未配置 drive fail closed |
| F4 process/path | `GetSystem/WindowsDirectoryOem`, `SearchPathOem`, `GetTempPathOem`, `GetTempFileNameOem`, `GetShortPathNameOem`, `CreateProcessOem` | 资源根/临时根/host PE delegation 必须分别显式配置；未准入项返回 failure |
| F5 environment | `Get/SetEnvironmentVariableOem`, `ExpandEnvironmentStringsOem` | per-session environment；不污染宿主 process environment |
| F6 optional product | `OutputDebugStringOem`, `GetComputerNameOem`, `RemoveFontResourceOem` | debug output 可安全转发；其余依 profile 决定或 fail closed |

实际 exported set 必须以 `oemuni.h` 与 `file.c/process.c` 的符号表复核；新增或缺少
一项都不得默认为无关。

## 最小验收序列

1. facade object target 可独立以 i386 构建，无 `NtCurrentTeb()->StaticUnicodeString`、
   `NtCurrentPeb`、旧 RTL string allocation 或 archive facade 链接。
2. S5 direct diagnostic 经原 `DemInit -> demLoadDos` 将 BYOB `NTDOS.SYS` 的开头
   `E9 A9 51` 读入历史 SAS 的 `DI:0`，并正常返回。
3. S5 selector fixture 经完整 `BIOS[50h] -> MS_bop_0 -> DemDispatch -> demLoadDos`
   获得同一加载结果与原 IP 消耗；此后才审计 `IDLE_disk`。
4. 每个 F1--F6 组均有原始 caller 或明确 fail-closed fixture；单独 conversion test
   不足以准入。
5. `SOURCE-MANIFEST.json`、LOCAL governance ledger 与 CMake 都记录新文件，并证明
   默认 runner 没有链接该 facade。

## 第一实现切片

为不重演 partial-shim 误判，第一切片可以只把 F1 `CreateFileOem` 实际接通到
`demLoadDos`，但同一提交必须有完整 29-entry facade 的 API inventory、每一项的
明确 `implemented` 或 `fail-closed` 状态以及后续 group gate。它不得把 F1 成功称为
完整 filesystem support。
