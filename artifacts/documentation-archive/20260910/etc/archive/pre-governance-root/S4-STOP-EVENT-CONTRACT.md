# S4 Stop Event Contract

状态：已实现并经 root MinGW fixture 验证，2026-08-09。

## 目的

S4 的停止记录只用于复核 software monitor 的 fail-closed 边界；它不是调试器、日志管道、
BIOS/BOP 接口或 host capability。每次 `monitor_run` 只保留本次运行最近的 terminal stop。

## 固定字段

- typed stop reason；
- 停止前的 real-mode `CS:IP`；
- 若映射有效则记录 physical fetch address；
- 至多四个从停止前 `CS:IP` 取得的 opcode byte；
- bridge disposition：未尝试或被拒绝。

该记录刻意不含 RAM snapshot、BOP service payload、DOS binary 内容、host path、设备状态或
任何 BIOS/DEM 内部对象。instruction budget 正常耗尽时清空旧记录，避免调用方把上一次停止
误认为本次进展。

## 行为边界

`BOP_EXIT`、`BRIDGE_REFUSED`、`UNSUPPORTED_BOP`、`UNKNOWN_OPCODE` 与 memory fault 都生成
该记录。bridge 成功不是停止理由，因此不生成“成功事件”；后续指令仍由 monitor 正常执行。
记录是只读副本，不能回写 CPU state、memory、mapping 或 historical bridge。

## 验证

`ntdos64-reconstructed-monitor-s4-test` 已断言：NOP budget exhaustion 没有 stop record；D6
bridge refusal 记录原始 `CS:IP`、physical fetch、opcode window 和拒绝 disposition；C4 `FE`
exit、C4 `0x03FE` typed refusal 与 unknown opcode 都有相应 stop reason。该 target 为
`EXCLUDE_FROM_ALL`，不进入默认 CTest 或 `ntdos64-run`。
