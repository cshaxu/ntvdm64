# r23 Adapter Multi-Write Fixture Evidence

## Scope

This is a closure test for the smallest currently admitted mechanical path.  It
does not boot NTDOS, mount a guest filesystem, or prove any DOS/WOW service
beyond the source-derived `DemGetDPBList` transaction shape.

The generated reset ROM establishes `ES=0100h`, `BP=0200h`, executes the
four-byte adapter boundary, then halts.  Its profile admits a fixed 26-slot
drive inventory containing two qualifying entries.  Adapter code produces four
opaque writes (two per packed 33-byte DPB) and a BP resume result.  Bochs sees
only the copied opaque transaction.

## Reproduction

The r23 container was configured from the repository snapshot with CPU level
5, disabled plugins and `--with-nogui`.  Only this declared target was built:

```powershell
make -j1 ntdos64-observation-fixture.exe
```

No `make all` and no `bochs.exe` target were invoked.  The fixture was then
run with:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File \
  tools\probe\Invoke-Bochs26NativeResetFixture.ps1 \
  -BuildRoot artifacts\build\bochs-2.6-native-nogui-ucrt-r23-multiwrite-cpu5 \
  -AdapterMultiWriteFixture
```

## Observed result

`artifacts/build/bochs-2.6-native-nogui-ucrt-r23-multiwrite-cpu5/ntdos64-reset-fixture/bochs.log`
contains:

```text
ntdos64 adapter multi-write transaction committed writes=4 bytes=c
```

The generated fixture then reaches its intentional `HLT` and the native CPU
loop stops.  This proves the r23 private-build closure compiled and linked the
adapter transaction producer and generic Bochs consumer, and that all four
preflighted writes committed before the checked resume.

## Boundary conclusion

The only imported-Bochs behavior added by this proof is generic: take a
copied-boundary descriptor array, validate ordinary writable RAM for every
range, copy the opaque payload after complete validation, and apply the
already-validated CPU result.  Drive selection, packed DPB layout, selector,
service ID, inventory and BP policy remain exclusively in
`src/bx-ntvdm-adapter`.
