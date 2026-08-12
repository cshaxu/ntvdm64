# S5 S4 SAS Aperture Audit

状态：2026-08-09。本文审计 S4 reconstructed monitor 与历史 NT4 CCPU/SAS 是否已经
共享同一 guest-memory aperture，并给出恢复真实 `S4 -> BIOS[50h]` 试验前的硬门槛。

## 结论

当前**没有**共享 aperture，因而不得把两条独立 fixture 拼接成运行时链：

```text
S4 monitor                    historical i386 fixture
-----------                   ------------------------
monitor->ram                  Start_of_M_area
  independent uint8_t *         original SAS allocation
```

现有 S4 bridge record 只有 selector、下一字节、物理地址和 CPU snapshot；它没有 memory
provider identity、地址空间所有权或映射生命周期。向历史侧 copy service byte、调用 BIOS，
再只拷回寄存器会丢失 DEM/BIOS 对 guest memory 的写入及其失败顺序，不能视为共享。

## 历史端证据

1. `base/mvdm/softpc.new/base/ccpu386/ccpusas4.c:368` 中原 `sas_init` 以
   `host_sas_init(size)` 的返回值设定 `Start_of_M_area`；随后其普通物理地址路径以
   `Start_of_M_area + addr` 访问。
2. `base/mvdm/softpc.new/host/src/sim32.c:426-441` 的 `CPU_40_STYLE` host owner 由
   `InitIntelMemory(size)` 建立该地址空间，并将结果返回给 SAS。
3. `local/compat/host/historical_bios_bridge_v1.c` 仅从
   `ntdos64_ccpu_sm0_ram()` 获得历史 aperture，在其中写 service byte，然后调用
   `BIOS[50h]`；它没有、也不应拥有 S4 的 RAM。
4. `reconstruction/monitor/reconstructed_monitor_s4.h` 的 monitor 直接保存
   `uint8_t *ram` 与长度，且当前 bridge response 只允许有限 CPU-register/CF 写回。

这些事实说明历史 SAS 已提供一个可观察的真实内存 owner；当前缺的是经过证明的
S4 memory-provider 与该 owner 的单一绑定，而不是另一个 BOP 或 DEM handler。

## 可接受的下一项设计工作

仅可设计一个 provider contract，要求：

1. S4 的 every fetch/load/store 通过同一个 checked physical-memory provider；不得直接
   把 host buffer 视作唯一实现。
2. 历史 i386 closure 只在 provider 明确暴露的同一 session aperture 上运行；跨位宽/跨进程
   时使用受检、定长 memory request，而不是 host pointer 或 copy-back。
3. provider 必须定义访问范围、读/写失败、ROM/device mapping 与 session lifetime；BOP 前后
   所有 guest side effect 都留在同一 aperture。
4. 在此合同和一个最小 read/write 双向 fixture 被证实前，`historical_bios_bridge_v1`、
   `demGetDrives` 和 S4 都只保留为相互独立的 bounded evidence。

## 明确禁止的捷径

- 为 `50h/0Fh` 预写或回填 DOS data、DPB、BDA、IVT 或 service result；
- 在 bridge 中复制整块 RAM 或按已知地址同步；
- 为了共享 host pointer 而把 i386 CCPU 对象直接链接进 x64 monitor；
- 让 bridge 自行解释 service byte、推进 IP、调用 DEM 或修改 CPU mode。

本审计把下一项高 ROI 工作限定为 memory-provider 接缝采证；它不授权实现新的 DOS、
BIOS、设备或 PC/AT 策略。
