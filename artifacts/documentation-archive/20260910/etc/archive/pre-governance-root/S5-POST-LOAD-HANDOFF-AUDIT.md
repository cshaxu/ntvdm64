# S5 NTDOS 加载后历史交接审计

状态：已完成第一个 post-load host owner 识别；未开始 NTIO 后续执行。

## 已证实的返回路径

`demLoadDos` 的原始职责止于以 live `DI` 作为 load segment 将 `NTDOS.SYS` 读入 SAS。
它在 `base/mvdm/dos/dem/demmisc.c` 中使用 `GetVDMAddr(getDI(), 0)`，循环调用
`ReadFile`，关闭 handle 后返回；它不会建立新的 BOP、SVC、BIOS 或 DOS 替代路径。

完整 default-disabled fixture 已证明该函数返回到原 `MS_bop_0`。后者在
`overlay/base/mvdm/softpc.new/host/src/nt_bop.c` 中按历史顺序：

```text
DemDispatch(11h) -> setIP(IP + 1) -> IDLE_disk()
```

对于 service `11h`，`IDLE_disk()` 不会被 date/time 例外跳过。它是
`base/mvdm/softpc.new/base/inc/idetect.h` 的宏，唯一目标是原
`base/mvdm/softpc.new/host/src/nt_unix.c:HostIdleNoActivity()`：将
`IdleNoActivity` 清零，并且只在 `NowWaiting` 时 `PulseEvent(IdleEvent)`。

该原 owner 已被 S5 fixture 显式链接，`BIOS[50h]` 调用成功返回。因此当前证据仅表明：

1. 原始 service byte 已消费，IP 从 `0478h` 到 `0479h`；
2. 原始 NTDOS load 和它的原始 post-dispatch idle notification 都返回；
3. 没有自写 BOP、DEM、DOS loader 或 idle handler 参与。

## 不能从该结果推出的结论

这不是 NTIO 已继续执行、更不是 COMMAND.COM 已启动的证据。fixture 直接调用
`BIOS[50h]`，随后终止；它不会进入 CPU dispatch loop，也不会让 NTDOS 的 `E9 A9 51`
入口取得执行权。`HostIdleNoActivity` 的返回只证明一个 notification seam 当前可闭合，
不证明现代 event/timeslice 行为的完整正确性。

## 下一项高 ROI 工作

先建立一个仍为 trace-only 的“service-11 返回状态”记录：冻结 CPU/SAS 可观察状态、确认
原始 CCPU/SoftPC dispatch loop 在该点应当恢复的 owner 与调用方式，并列出恢复执行前仍缺少的
真实 monitor 语义。该任务不得为了让 NTDOS 前进而增加指令解释、BOP/SVC handler、IVT/BDA
scaffold 或设备响应。

只有获得该 return-to-monitor contract 后，才可判断是继续修复原 SoftPC monitor，还是记录为
尚缺的源语义；不能由这个加载成功 fixture 推断出可直接执行 guest 的新路径。
