# S1 Historical Host Closure

状态：第一条 direct table invocation 已验证，2026-08-09。

## 结论

`ntdos64-opennt-original-bios50-date-fixture` 在独立的 i386 clang-cl
OpenNT 构建岛中编译为 `COFF-i386`，并以退出码 `0` 运行。它证明了一个不经
`c_cpu_simulate()` 指令循环的历史 host-side 调用：

```text
fixture direct call
  -> original BIOS[0x50]
  -> original MS_bop_0
  -> original Sim32GetVDMPointer
  -> original DemDispatch(0x14)
  -> original demQueryDate
```

fixture 在受控 SAS RAM 的 `CS:IP = 0:3` 写入 service byte `0x14`，直接调用
原始 table entry，随后断言：

- 原始 `MS_bop_0` 将 IP 从 `3` 推进为 `4`；
- 原始 date service 给出合理的 `AL` weekday、`DH` month、`DL` day 和 `CX` year；
- 归档 CCPU foundation 的 `unexpected_calls` 保持零。

该结果把历史 `BIOS[] -> MS_bop_0 -> DemDispatch` 的 host-side 边界与任何本地
guest instruction decoder 分开。它不是 CCPU 已恢复、NTIO 已启动、DOS 已运行或
跨位宽 bridge 已实现的证据。

## 受控输入与构建边界

- fixture: `src/opennt/local/trace/fixtures/original_bios50_date_fixture.c`
- target: `ntdos64-opennt-original-bios50-date-fixture`
- compiler profile: `clang-cl --target=i686-pc-windows-msvc` + VS x86 SDK/CRT
- output: `artifacts/build/current/opennt-r2-bridge-ninja-i686/`
- OpenNT input: `src/opennt/SOURCE-MANIFEST.json` 中固定的 NT4 revision

`BIOS[]` 是完整静态函数指针表。即使 fixture 只调用 ordinal `0x50`，链接器仍会
看到其余未调用 table entry 的旧 host 依赖。因此该 default-disabled trace target
使用 `/OPT:REF /FORCE:UNRESOLVED`：前者裁掉未达 section，后者只将未调用 entry
保留为 link diagnostics。它不能被解释为完整 BIOS closure，且任何 runtime target
不得使用这一做法绕过实际调用的 unresolved symbol。

## 下一步

S2 以此确定的 host-side事实为输入，定义 versioned bridge record：历史 i386 一侧
继续在自身地址空间内调用 `BIOS[selector]`；现代软件 monitor 只能传递定宽寄存器
快照、selector、受检查 memory request、disposition 和 redacted event，不能接收
函数指针、SAS pointer、`Video` 或 CRT object。
