# T188 S8 two-object build 001

Date: 2026-08-12  
Packet: M0 T188 S8  
Disposition: passed build closure; no guest was invoked.

## Fixed input

The only admitted build root was
`artifacts/build/current/t188-copied-state-derivative-r3`, generated and
sealed by T188 S7. Its base engine hash is
`EB55EFA8CBBB4E9B640D3AC2BFAF054AE551EB0EB793A597733E660F329CCA57`.
The makefile hash is
`A5037891EAE2515A04E377825947A3D1204386336FC97C946E9FF57C76FAB03F`.

## Command and result

In the documented MSVC x86 environment, the sole command was:

```text
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x86 && nmake /f ntdos64-t188-copied-state.mak ntdos64-t188-copied-state.exe
```

It exited zero. The build compiled exactly `cpu/exception.o` (with only
`BX_NTVDM_ENABLE_ADAPTER_STATE_DIAGNOSTIC=1` added to its existing flags) and
`adapter/bx_ntvdm_adapter_runtime.obj`, then linked
`ntdos64-t188-copied-state.exe`. No archive, device, configuration, full
Bochs, guest, or CLI target was built.

The retained compiler warning is `exception.cc(1561): C4244`; the retained
linker warnings are LNK4042 (duplicate `main`), LNK4098 (MSVCRT/UCRT), and
LNK4217 (`illegalp` UCRT import). They are inherited link-model observations,
not repaired or reclassified by this packet.

## Output identity

| Output | SHA-256 |
| --- | --- |
| `cpu/exception.o` | `41EE4A2AF0C277BF308E6F1424B3CE543222DBDFF4909E165B2CDBD22235A6AA` |
| `adapter/bx_ntvdm_adapter_runtime.obj` | `E3180898E8D8E1974D1CFE5B3F86F4FB61B937D4DE09FEA328DAFB1B7BE3B8C3` |
| `ntdos64-t188-copied-state.exe` | `766D3A7AAB980B05C683DD99530801F61578F9F886DD0A1122BF810D6EA8E938` |
| `ntdos64-t188-copied-state.map` | `0D600ABF5B9AAE75FF022465A5EF6176C56F75340103612AC3399C2EACA01C30` |

The map resolves `_bx_ntvdm_adapter_runtime_v1_copy_diagnostic_state` from the
replaced runtime object. Thus the default-off Bochs trace and its copied,
adapter-owned state ABI are present in the admitted engine without importing a
new dependency island.

## Boundary and follow-up

This is build evidence only. It makes no claim that the BOP is dispatched,
that a transaction is consumed, or that the guest can run continuously.
T187's observation controller failed to persist a terminal outcome after an
access-denied cleanup attempt. Therefore the next bounded S first closes a
one-shot controller that records an outcome despite cleanup failure; only a
later separately admitted S may run this frozen engine once.
