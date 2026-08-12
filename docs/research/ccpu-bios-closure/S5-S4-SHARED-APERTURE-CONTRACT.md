# S5 S4 Shared Aperture Contract

状态：设计采证门槛，2026-08-09。本文不是实现授权。它规定若要让 x64 S4 monitor
与 i386 历史 CCPU/SAS 真正观察同一 guest memory，唯一可研究的最小接缝是什么。

## 目标拓扑

```text
x64 monitor process                         i386 historical process
-------------------                         -----------------------
S4 checked access                            original SAS/CCPU access
  -> local view of backing                     -> Start_of_M_area local view
       \                                       /
        \-- one per-session shared backing --/
```

两个进程各自只持有本进程有效的映射指针；跨进程 record 不传递 pointer。它们读取和写入
同一个 backing store，因此 BOP 前的 NTDOS 字节、历史 DEM 的 guest write 和 BOP 后的
S4 fetch 都能在同一逻辑物理地址空间中观察到。

## 历史 owner 不变

历史 `ccpusas4.c::sas_init` 仍调用 `host_sas_init(size)`，并把其返回值保存为
`Start_of_M_area`。因此可能的现代 adapter 只可位于这个**已存在的 host allocation
边界**：在 i386 进程内建立 shared-backing view，返回该 view 的局部地址。SAS、CCPU、
BIOS、MS_bop_0、DemDispatch 和 DEM 都不改写，也不获得新接口。

这不是“替换 SoftPC memory”。它仅代替早期 Windows host allocator/VM reservation 的
失效实现；其行为必须与原 `host_sas_init` 的成功/失败、size 与 term lifecycle 对齐。

## 固定约束

1. session 创建方指定长度，长度非零且有上限；两端必须确认相同长度和 ABI version。
2. backing 必须按 byte-addressable physical address 0..size-1 一一对应；不得用
   segment:offset 或 DOS service 语义重解释地址。
3. S4 的 fetch/read/write 与历史 SAS 的访问必须都直接落在各自 view 的同一 offset；
   禁止 BOP 前后 copy、脏页同步、已知地址 patch 或 service-result 回填。
4. shared mapping 的创建、句柄传递、映射、unmap 与 close 都是 session lifecycle；失败
   必须在 BIOS/DEM invocation 前停止，不得降级到私有两份 RAM。
5. `host_sas_term` 只释放历史侧 view；x64 view 的生命周期由 session owner 管理。任何
   一侧 teardown 后，另一侧访问必须 typed stop，而不是悬垂 pointer。
6. 该 contract 不授予 ROM、device memory、A20、I/O、IRQ、V86、protected mode 或
   arbitrary host pointer access。

## 实现前必须先取得的证据

1. 用一个 default-disabled 双进程 fixture 只做 raw offset 的双向 byte/word 读写；它不得
   调用 CPU、BIOS、BOP、DEM 或加载 DOS。
2. 证明历史侧 `sas_init -> host_sas_init` 可以在不改变 SAS/CCPU 源码的前提下接收该 view，
   并在 `sas_term` 后失效。
3. 证明 S4 的 current ordinary-RAM route 可由本地 shared view 驱动，且越界行为仍为
   `MEMORY_FAULT`。
4. 证明 session ID、size、mapping readiness 与 teardown 在 transport record 中可验证；
   record 不携带 address、handle 值或二进制内容。

只有四项都通过后，才可以用同一个 aperture 做一次 `C4 C4 50 0F` 到原始
`BIOS[50h] -> MS_bop_0 -> DemDispatch -> demGetDrives` 的 bounded invocation。
该试验仍不等于 NTIO 或 DOS runtime 成功。

## 禁止替代

- 以 S4 callback 返回 `AX=5` 取代 `demGetDrives`；
- 在 transport record 传递 `Start_of_M_area`、S4 `ram` 或任何跨进程裸 pointer；
- 使用文件系统或 device shim 伪造这个最初 memory test；
- 将共享 memory 设计解释为 mantle/core/product 边界决定。

该工作是 OpenNT SoftPC host-allocation seam 的局部恢复研究，不改变项目的长期产品架构。
