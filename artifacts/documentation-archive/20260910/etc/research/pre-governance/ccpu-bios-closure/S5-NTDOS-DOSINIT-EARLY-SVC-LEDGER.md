# S5 NTDOS DosInit Early SVC Ledger

状态：2026-08-09。本记录从固定 NT4 EN-US 的原始
`base/mvdm/dos/v86/doskrnl/dos/msinit.asm::DosInit` 提取 post-load 的早期服务顺序。
它是 source-level 调用顺序证据，不是 guest 执行或 runtime 准入。

## 服务序列

`DosInit` 在搬移 DOS data、读取 `BiosComBlock` 的 debug flag 后，按以下顺序发出：

| 顺序 | guest 编码/服务 | 历史分派链 | 原始 owner | 直接职责 | host seam 级别 |
| ---: | --- | --- | --- | --- | --- |
| 1 | `C4 C4 50 0F` / `SVC_DEMGETDRIVES` | `BIOS[50h] -> MS_bop_0 -> DemDispatch(0Fh)` | `demgset.c::demGetDrives` | 设置 `numio`，采集 A:--Z: 的物理 drive type | 高：OEM/native drive discovery |
| 2 | `C4 C4 50 1B` / `SVC_DEMSETDTALOCATION` | 同上 | `demgset.c::demSetDTALocation` | 把 DMA、PDB、extended-error、DOSWOW、SFT 的 guest 线性地址存入 DEM globals | 低：只要求正确 `GetVDMAddr` |
| 3 | `C4 C4 50 32` / `SVC_DEMSETHARDERRORINFO` | 同上 | `demerror.c::demSetHardErrorInfo` | 保存 VHE 与 device-chain 的 guest 地址 | 低：只要求正确 `GetVDMAddr` |
| 4 | `C4 C4 54 05` / `SVC_CMDSETINFO` | `BIOS[54h] -> MS_bop_4 -> CmdDispatch(05h)` | `cmdmisc.c::cmdSetInfo` | 保存 SCS sync、DOS-binary 与 floppy-access 的 guest 地址 | 低：只要求正确 `GetVDMAddr` |
| 5 | `C4 C4 50 46` / `SVC_DEMGETDPBLIST` | `BIOS[50h] -> MS_bop_0 -> DemDispatch(46h)` | `demgset.c::demGetDPBList` | 在 `ES:BP` 写入 fixed/removable drive 的 DPB 链，并返回新的 `BP` | 高：再次依赖 drive discovery 与 guest-memory 写入 |

若 `BiosComBlock` 指示 debuggee，服务 1 与 2 之间还会调用 `SVC_DEMSYSTEMSYMBOLOP` (`45h`)；
它只属于 debug profile，不是默认 EN-US normal startup 的前置条件。

## BOP 语义差异

`SVC` 使用 `BOP_DOS = 50h`，而 `CMDSVC` 的原始宏使用 `BOP_CMD = 54h` 后跟一个 service byte。
`MS_bop_0` 从 `CS:IP` 读取 service、调用 `DemDispatch`，然后递增 IP；`MS_bop_4` 以同一方式
调用 `CmdDispatch` 并递增 IP。因此二者都要求 monitor 保持原始取 byte、分派、IP 消费的职责，
不能由 host shim 或新 DOS runtime 偷代。

## 机器与 host 边界

服务 2--4 不要求提前实现 DOS namespace、磁盘 I/O、console 或 device policy；它们只是把
已存在 guest RAM 内的真实模式 segment:offset 转成稳定 host pointer。它们直接要求：

1. `GetVDMAddr` / SAS 地址换算能验证并映射已初始化的 guest RAM；
2. `DS:AX`、`DS:DX`、`DS:CX`、`DS:SI`、`ES:BP` 与 flags 在 BOP 前后由原始 monitor 语义保存；
3. host 保存的是 pointer/状态，不可借机重写 DOS data、BDA、IVT 或服务返回值。

服务 5 则发生在原始 `charinit` 已遍历 NTIO 提供的 device chain 之后，说明它不是单纯的
drive-count 查询。它会遍历 26 个 logical drive，通过 `demGetPhysicalDriveType` 只为 removable/fixed
drive 写入 DOS DPB，并以原始 `BP` 作为 guest allocation cursor。故它同时要求：

- NTIO 传入的 device-chain 和 DOS data 所在 guest RAM 已可读写；
- drive discovery 对服务 1 与服务 5 的结果保持同一 session 内一致；
- host 的 drive/media policy 发生在既有 OEM/native 调用边界，不能由 `demGetDPBList` 的 local
  substitute 或预写 DPB 取代。

## 对当前恢复顺序的结论

1. `demGetDrives` 是第一个真实的 host namespace seam；其 original owner 已在 no-SM0 source
   closure 中，现代 TEB/RTL compatibility 仍未获准入。
2. 紧随其后的三项服务是恢复 monitor guest-memory contract 的最小、可观察消费方；它们不证明
   任一现代 host shim 已完成。
3. `MS_bop_4 -> CmdDispatch(05h)` 必须纳入未来历史链闭包；只恢复 selector `50h` 不能称为
   NTDOS `DosInit` 已闭合。
4. `demGetDPBList` 是下一条实际会扩大 drive policy、guest write 和 device-chain 依赖的服务，
   应在 `demGetDrives` 的真实 host seam 获准入后再做 caller-first 采证。
