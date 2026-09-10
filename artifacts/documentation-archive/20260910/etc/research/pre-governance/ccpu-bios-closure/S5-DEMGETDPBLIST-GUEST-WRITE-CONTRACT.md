# S5 DemGetDPBList Guest Write Contract

状态：2026-08-09。本文记录固定 NT4 `DosInit` 在 device-chain 初始化之后对
`SVC_DEMGETDPBLIST` 的真实 caller、guest-memory 写入和 host-state依赖。它不是
新的 DPB 实现，也不授权在 loader 或 monitor 中预写 DPB。

## 原始调用位置与前置状态

`msinit.asm:501-524` 先遍历 NTIO 所提供的 character/block device chain，依次调用
原始 `charinit`；随后：

```text
mov bp, offset MSDAT001e
mov word ptr DPBHead, bp
mov word ptr DPBHead + 2, es
...
SVC SVC_DEMGETDPBLIST
```

因此，调用时 `ES:BP` 是 NTDOS 自己选择的、位于 DOS data allocation area 的 free
cursor。它不是 wrapper 可选择的 host buffer，也不是 profile 固定地址。服务返回后，
NTDOS 检查 `BP` 是否仍等于 `MSDAT001e`；不变时才把 `DPBHead` 置为 `FFFF:FFFF`。

历史分派仍是：

```text
C4 C4 50 46
  -> CCPU BOP decoder -> BIOS[50h] -> MS_bop_0
  -> DemDispatch(SVC_DEMGETDPBLIST) -> demGetDPBList
```

## 原始写入合同

`demgset.c:940-1012` 的实际规则：

1. 从 original `getES()`、`getBP()` 取得初始 far pointer，并通过 original
   `GetVDMAddr(ES,BP)` 取得 guest-visible backing；
2. 遍历 `PhysicalDriveTypes[0..25]`，该 array 必须来自先前原始
   `demGetDrives`，不可由调用者重建；
3. **仅** `DRIVE_REMOVABLE` 和 `DRIVE_FIXED` 产生 entry。CD-ROM、RAM disk、remote
   和 unknown 都不产生 entry，即使其中一部分参与此前 `nDrives` 的计数；
4. 每个 entry 只写 `DPB.DriveNum`、`DPB.Unit` 和 `DPB.Next`。源码注释明确说明这是
   为兼容性而保留的最小 DPB list，不填充完整几何/BPB 字段；
5. `Next` 写为同一 `ES` 与 `BP + sizeof(DPB)` 的 **16:16 packed guest far
   pointer**，不是 host C pointer；最后一个 entry 的 `Next` 为 `FFFFFFFFh`；
6. 回调只通过 `setBP(usDpbOffset)` 返回新的 free cursor。若没有 eligible drive，
   BP 不变且不写 entry。

`sizeof(DPB)`、字段偏移与 packing 由原始 `demdasd.h`/历史 i386 build ABI 决定。
未来 fixture 必须从同一 historical compile domain 取得这一大小，不能从 x64 host C
layout 或手写常量推导。

## 对 machine 与 host seam 的要求

这条路径首次同时要求：

- 原始 `GetVDMAddr` 能把 live NTDOS `ES:BP` 转成已验证、可写的 ordinary guest RAM；
- BOP 进出时 ES/BP 和 IP 的原始 CCPU/MS_bop_0 语义保持完整；
- `demGetDrives` 的 session-local drive classification 在同一 DEM state 内仍然有效；
- guest buffer 有足够容量容纳所有 eligible drives 的 `sizeof(DPB)` entries，且
  `usDpbOffset < FFFFh` 的原始 assertion 不得被 host-side wraparound 掩盖。

它**不**独立要求完整 guest block I/O、FDC/HDC controller、BPB reload 或 file namespace。
那些属于 `demGetDPB`/`demGetDpbI` 等后续路径，不能借 `demGetDPBList` 的存在提前纳入。

## 允许的下一项验收

只有在 original chain 已实际达到 `SVC_DEMGETDPBLIST` 后，才可建立 default-disabled
trace fixture。它必须：

1. 捕获 call 前后的 ES、BP、FLAGS、IP 和 `PhysicalDriveTypes` snapshot；
2. 以 historical `sizeof(DPB)` 解码同一 guest aperture 中已写 entry；
3. 验证 entry 只对应 removable/fixed 分类、尾指针为 `FFFFFFFFh`，无 entry 时 BP 不变；
4. 在坏 `GetVDMAddr`、跨界 buffer 或 offset overflow 时 fail closed，不以部分 DPB
   写入宣称成功。

在此之前，不得用模拟 DPB、预置 `DPBHead`、本地 `DemDispatch` switch 或 image backend
来制造该调用已经运行的假象。

## 证据来源

- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msinit.asm:501-540`
- `src/opennt/base/mvdm/dos/dem/demgset.c:39-45, 940-1012`
- `src/opennt/base/mvdm/dos/dem/demdasd.h:29-50`
- `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c:146-159`
- `docs/research/ccpu-bios-closure/S5-DEMGETDRIVES-HOST-SEAM-CONTRACT.md`
