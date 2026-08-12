# T95 S6 unresolved-symbol ownership ledger

## Inputs and reproducibility

This is a read-only classification of observation-003's frozen
`link.log`, `link-invocation.json`, and `link-argument-vector.json` under
`artifacts/analysis/t95-s6-msvc-x86-cpu3-link-retention-003-20260811-001`.
The machine-readable companion is
`unresolved-symbol-ownership-ledger.json`, SHA-256
`16D5C6506FBA2E9289DE8F9D3996C846E09761A04FA5464A72621067E749C5E8`.
Its `value` collection has one record for each identity, including its full
linker identity, referencing object list, exact classification, provider
family/source, and reason.

The extraction command is the PowerShell equivalent of:

```
Get-Content link.log | match '^(?<obj>[^ :]+\.obj) : error LNK(2019|2001):
unresolved external symbol (?<sym>.+?)( referenced in function .+)?$'
```

It groups by `sym` and unique referencing `obj`; the result is exactly 72,
matching `LNK1120`. Provider searches used `rg -n -g '*.cc' -g '*.h'` from
`src/bochs`, including exact method names and `ia_opcodes.h` map entries.

## Classification result

| Classification | Identities | Disposition |
| --- | ---: | --- |
| mandatory native CPU provider | 5 | `inhibit_interrupts` and `set_INTR` are defined by `cpu/event.cc`; `prefetch`, `repeat`, and `repeat_ZF` by `cpu/cpu.cc`. These are a real native CPU-family closure gap. |
| mandatory bare-machine core provider | 0 | No unresolved identity is currently proven to require an additional bare-machine core provider. |
| default-but-not-required retained member | 31 | CPU feature handlers and virtual/slowdown timer members are retained by the declared common objects although the minimal lifecycle does not call them. |
| explicitly rejected product owner | 35 | Product plugin/device, GUI, configuration/UI, and `main.cc` startup ownership is retained by `devices.obj`, `siminterface.obj`, `pc_system.obj`, or `logio.obj`. |
| unresolved/ambiguous | 1 | `BX_CPU_C::GETSEC`, referenced from `fetchdecode.obj`; exact provider source was not proven by the recorded source search. This stops ownership closure. |

## CPU3 feature reconciliation

`ia_opcodes.h` supplies the static handler map and `fetchdecode.cc` retains
its function-pointer targets. The ledger therefore records the following as
**default-but-not-required retained members**, not missing feature grants:

- VMX: `VMXON`, `VMXOFF`, `VMCALL`, `VMLAUNCH`, `VMCLEAR`, `VMPTRLD`,
  `VMPTRST`, `VMREAD_EdGd`, `VMWRITE_GdEd`, and `VMFUNC`, with providers in
  `cpu/vmx.cc` or `cpu/vmfunc.cc`.
- SVM: `VMRUN`, `VMMCALL`, `VMLOAD`, `VMSAVE`, `SKINIT`, `CLGI`, `STGI`, and
  `INVLPGA`, provider `cpu/svm.cc`.
- MSR: `RDMSR` and `WRMSR`, provider `cpu/msr.cc`; ADX: `ADCX_GdEdR` and
  `ADOX_GdEdR`, provider `cpu/bmi32.cc`.

CPU3 fixes VMX/SVM/MSR off and does not admit ADX/SMX. Their linker appearance
means the selected `fetchdecode.obj`/static opcode-map object retains targets
outside the profile surface. It is evidence that the future object model must
eliminate or correctly guard retained code; it is never authorization to
enable any of those features. `GETSEC` is the explicit unresolved exception to
the provider mapping, so no further inference is made.

## Rejected ownership families

The ledger source-proves plugin control in `plugin.cc`; device plugin entry
points in their named `iodev` files (CMOS, DMA, floppy, hard-drive/image,
keyboard, PIC, PIT and VGA); GUI ownership in `gui/gui.cc`; configuration
option ownership in `config.cc`; and product startup ownership in `main.cc`.
Virtual and slowdown timers are in `iodev/virt_timer.cc` and
`iodev/slowdown_timer.cc`; they are not required by the registered empty-port
lifecycle, so remain default-but-not-required rather than an admitted device.

## Stop and non-claims

`GETSEC` has no proven provider in this audit, so the task stops here. No
object was added, and no compiler, linker, config/source/exception, adapter,
OpenNT, device, firmware, plugin, or executable action occurred. This ledger
does not make a link, runtime, firmware, guest, or product claim.

## Next gate

Coordinator must review the one unproven `GETSEC` ownership together with the
five mandatory CPU providers and the rejected retention paths. Any repair,
new object set, feature decision, or build observation needs a separate
admission.
