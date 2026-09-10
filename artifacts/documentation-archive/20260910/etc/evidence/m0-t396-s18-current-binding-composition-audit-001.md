# M0 T396 S18 — current-binding composition audit 001

## Question

Did the frozen S16 product and the v4 image differ in an undeclared CCPU or
SoftPC composition that could explain their apparently different WOW runtime
results?

## Inputs and procedure

- S16 product: `build/M0-T396/S16/s11-relink/ntvdm32-s16.exe`, SHA-256
  `dd1d77e7824ff99a719bc94440fd4fc6a59bce7b846f19d9ed46aaa8d4f639a2`.
- v4 candidate: `build/M0-T396/S17/kernel-selector/ntvdm32-kernel-entry-v4.exe`,
  SHA-256 `35ab98611f7b7e793c422ed361228039cc10a05e931b1f7fa459f5d354aa4b3d`.
- S11 complete `original-ccpu386.lib` and `softpc-bindings.lib`, and the v4
  replacements of those two archives.

Each archive member was extracted and compared by SHA-256, rather than by
member name: v4 repacks members as `memberNNN.obj`.  The current formal
`c_main.obj` and machine-observation object were also hashed against their
v4 members.  The current-binding image was staged with the same selected
WOW32 companion, WOWEXEC workload, pure-DOS profile and S17 derived KRNL386
contract, then observed with the ordinary console-owning observer and the
declared `system32\WOWEXEC.EXE` product command.

## Observations

| Archive | S11 members | Equal v4 members | Sole non-S11 v4 member | Current-object match |
| --- | ---: | ---: | --- | --- |
| `original-ccpu386.lib` | 132 | 131 | `member097.obj` | `formal-x86/obj/ccpu/c_main.obj` |
| `softpc-bindings.lib` | 13 | 12 | `member000.obj` | `formal-x86/obj/adapter-softpc/mvdm_softpc_machine_observation.obj` |

No other member hash differs.  The v4 product command was:

```text
O:\t396\ntvdm32.exe -f -o --command system32\WOWEXEC.EXE
```

It exited `0xc0000409` after 7156 ms, with 15 loaded modules and
`loaded-wow32=no`.  The manifest records the v4 product hash above and the
S17 source-shaped KRNL386 derived hash
`d404576899cc11d23f368f2b30554e6108657cd37809837b341fd37e6b1a7af4`.

## Interpretation

The v4 image is a fully declared current-binding control for the only two
then-current CCPU/SoftPC objects; it is not a hidden broad archive mixture.
On the actual WOWEXEC target route it reproduces S16's native fast-fail
terminal.  The earlier 28-module ARPL observation used the distinct direct
KRNL386 target, so it cannot establish a WOWEXEC advance or a composition
explanation.  S18 closes without a CPU, BOP, COMMAND or media repair.

## Follow-up

Audit the source-defined native fast-fail path that follows the actual WOWEXEC
route's reached DOSX `53:00` return.  It must begin from the current-binding
terminal and may not substitute the direct KRNL target or infer progress from
the diagnostic route.
