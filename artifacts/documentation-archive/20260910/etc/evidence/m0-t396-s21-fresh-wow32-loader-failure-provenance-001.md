# M0 T396 S21 — fresh WOW32 loader failure provenance 001

## Question

After S20 removed the historical mixed-object `session` layout, what current
source owner produces the fresh product's ordinary `0xc0000409` result before
`WOW32.DLL` is loaded?

## Inputs and procedure

The input was S20's complete current-source x86 composition:

```text
build/M0-T396/S20/formal-x86/original-softpc-process.exe
SHA-256 EF60D6A993041DF34635C7C867E53D560F624190DE2136CC857AA94399DECF44
```

It was staged by the existing fixed-runtime manifest at `O:\\t396` and run
through `console-startup-observer.exe`, without a debugger, with the selected
`system32\\WOWEXEC.EXE` target.  The normal run exited in 6484 ms with
`0xc0000409`, loaded 15 modules, and reported `loaded-wow32=no`.

Windows Error Reporting independently recorded fault offset `0x0005cb1b`,
exception `0xc0000409`, and fast-fail data `7`.  A FileSystemWatcher copied
that ordinary WER minidump after its write had stabilized; it is disposable
build output at `build/M0-T396/S21-fresh-normal.mdmp` (2,565,892 bytes), not a
tracked runtime input.

The dump's crashing thread has `EIP=0x0105cb1b`.  With its ASLR base
`0x01000000`, this resolves to S20 map address `0x0045cb1b`: CRT `_abort()`'s
`int 29h`.  Its EBP return chain, rebased to the preferred map base, is:

| Return map address | Current owner |
| --- | --- |
| `0046b048` | `nt_reset.c::TerminateVDM` + `0x8` |
| `004795ce` | `nt_bop.c::MS_bop_1` + `0x2e` |
| `00415c39` | `c_main.c::c_cpu_simulate` |
| `0046ac88` | `nt_cpu.c::host_start_cpu` |
| `0048a6fa` | `main.c::host_main` |
| `0046b67d` | `ntvdm_entry.c::mvdm_softpc_original_entry` |
| `004019aa` | `machine_shell.c::app_machine_shell_run` |
| `00401700` | `entry.c::main` |

The first two source frames are decisive.  `TerminateVDM()` in the selected
original `softpc.new/host/src/nt_reset.c` calls `terminate()` and its original
close path reaches CRT `_abort`.  `MS_bop_1()` in
`softpc.new/host/src/nt_bop.c` invokes `TerminateVDM()` immediately when its
first `SafeLoadLibrary("WOW32")` returns `NULL`; it does so before the
`W32Init`, `W32Dispatch`, comm, or notification export lookups.  The normal
module inventory's `loaded-wow32=no` agrees with that branch.

## Interpretation and confidence

This is an ordinary-product, source/map-correlated attribution with high
confidence: the active boundary is the original WOW loader's
`SafeLoadLibrary("WOW32")` failure, not the retired v4 four-byte caller/object
mix, a guest execution fault, a BOP semantic substitution, or the debugger's
separate `0x78` result.  The source-defined termination is preserved; S21
does not change it.

## Follow-up

S22 owns only the reason that the selected, already staged WOW32 provider is
not found by the original `SafeLoadLibrary` route: its effective search/name
contract, process directory state, and the smallest source-shaped binding
boundary.  It may not claim a load, alter the original termination branch,
substitute a DLL target, or change source/media/CPU/BOP behavior without a
newly admitted recovery packet.
