# S5 S4 Bridge Return Contract

状态：2026-08-09。本文固定 reconstructed S4 software monitor 在到达普通 `C4 C4`
BOP 时的最小返回合同。它是对历史调用返回形状的受控采证，不是 DOS service
实现，也不是把历史 DEM 的职责转移到本地 callback。

## 可写状态

bridge request 是进入 BOP 后的 real-mode CPU 快照。response 只能请求下列有限
写入：`AX`、`CX`、`DX`、`CS`、`IP` 与单独的 `CF`。

`CF` 使用单独的 `carry` 布尔值；bridge 没有任意 FLAGS image、任意通用寄存器、
descriptor、CRx、A20、内存映射或 CPU mode 的写权限。`carry` 只能为 0 或 1。
所以它不能借 BOP callback 绕过 monitor 的 CPU 语义。

response 先整体验证，之后才提交状态：未知 write bit 或非法 `carry` 都返回
`BRIDGE_REFUSED`，不修改任何 response 指定的寄存器或 FLAGS 位。这个顺序是
bridge 原子性的最小要求。

## 历史依据与测试范围

原始 `BIOS[50h] -> MS_bop_0 -> DemDispatch -> demGetDrives` fixture 已观察到：

```text
selector = 50h
service  = 0Fh
return   = AX low byte = nDrives, CF = 0, IP consumes service byte
```

S4 测试以 `C4 C4 50 0F` 验证该返回**形状**：BOP decode 后 request 的 `IP` 指向
service byte，受检 bridge 把 `AX`、`CF=0` 与 `IP+1` 一次提交，同时保留 `IF` 与
FLAGS 的保留位。另一个测试以 `carry=2` 验证拒绝没有部分提交。

该 callback 不调用 `DemDispatch`，不计算 drive count，也不构成正常 runtime 路径；
数值 `5` 只是先前历史 trace 的固定观测样本。默认构建与 `ntdos64-run` 均不链接
本 target。

## 当前不能越过的边界

历史 fixture 的 `MS_bop_0` 使用 CCPU/SAS 的 SM0 memory；S4 使用独立 ordinary
RAM。两者尚没有同一 guest-memory aperture。因此不能把“原始 dispatcher 已单独
返回”与“S4 已到达 BOP”宣称为同一条执行链，更不能通过 copy-in/copy-out 拼接出
伪共享内存。

下一步只有在能证明 shared SAS/guest-memory contract 的情况下，才可建立真实
`S4 -> original BIOS[50h] -> MS_bop_0 -> DemDispatch` 试验。否则本文件所述 bridge
仅保留为 bounded ABI fixture。
