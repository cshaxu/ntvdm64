# T95 S6 CPU-profile authority reconciliation

## Question and procedure

This read-only reconciliation compares the accepted S6 v2 MSVC projection's
`BX_CPU_LEVEL=3` with the historical native-container use of `-CpuLevel 5`.
It does not select a CPU level or alter a configuration, manifest, source,
exception, recipe, or CPU ledger.

The searches were:

```powershell
rg -n -i -C 2 "CpuLevel|CPU[_ -]?LEVEL|cpu level|cpu5|CPU3|cpu3" docs tools tests src/bochs/config.h src/bochs/config.h.in src/bochs/configure src/bochs/configure.in
rg -n -i -C 2 "Configure-Bochs26NativeContainer.*CpuLevel 5|CpuLevel 5|cpu5|CPU level 5|CPU-level-5" docs tools artifacts/build
rg -n -C 2 "CPU3|BX_CPU_LEVEL.*3|CPU level|first-profile|fixture|stock firmware" docs/design docs/QUEUE.md docs/STATUS.md docs/etc/research/t95-s6-msvc-config-projection.md docs/etc/research/t95-s6-msvc-format-token-projection-v2.md docs/etc/research/t95-s6-msvc-projected-syntax-003.md tools/t95-s6-msvc-config-projection-manifest-v2.json
```

An initial broad use of PowerShell path globs with `rg` exited 1 because those
literal patterns are invalid paths on this host.  It made no change and did not
invoke a build; the narrowed searches above supplied the cited evidence.

## Authority and provenance

| Setting | Provenance | Layer and authority | Classification |
| --- | --- | --- | --- |
| CPU3 | `tools/t95-s6-msvc-config-projection-manifest-v2.json:9` overlays `BX_CPU_LEVEL` from `6` to `3`; `docs/etc/research/t95-s6-msvc-config-projection.md:23-27` calls this the selected CPU3/x86 deny-by-default profile. `docs/etc/research/t95-s6-msvc-projected-syntax-003.md` records only a successful MSVC `/Zs` observation. | S6 analysis configuration for declaration/syntax closure. It is accepted for that analysis scope, not a build artifact, firmware run, or product feature grant. | resolved as analysis-only CPU3. |
| CPU5 | `tools/Configure-Bochs26NativeContainer.ps1:5-6` permits levels 3/5 and defaults to 3; its options forward the selected value as `--enable-cpu-level=$CpuLevel` at line 132. Historical r24's immutable `ntdos64-native-container.json` records `--enable-cpu-level=5`. | Historical MSYS2/UCRT native-container and stock-ROM/floppy observation evidence, outside the current MSVC projection. The script default proves CPU5 was explicit rather than inherited. | resolved as historical CPU5 firmware-profile evidence. |
| CPU3 mechanics fixture | `docs/design/STOCK-FIRMWARE-STARTUP-PROBE.md:25-35` says the r15 CPU3 container is the minimal mechanics baseline. | A custom/reset mechanics baseline; its scope excludes stock firmware startup and OpenNT behavior. | resolved, distinct fixture layer. |
| CPU5 stock firmware fixture | The same design record at lines 25-35 says the stock BIOS cannot reach the boot sector at CPU3 because it executes `cpuid(1)`; it expressly permits a **separate** CPU5 container only for that firmware-reached instruction requirement. r16 at lines 79-89 records the CPU5 stock-ROM boot observation. | A bounded stock-firmware observation profile. The document explicitly says it is neither device admission nor an OpenNT semantic change, and that CPU3 remains the mechanics baseline. | resolved, distinct fixture layer. |
| First-profile OpenNT runtime CPU level | `docs/design/GOAL.md` requires a declared first-profile OpenNT caller before enabling a feature; `docs/QUEUE.md` leaves S5 at the S6 native-fixture gate. No cited S6 syntax record runs guest OpenNT, and the CPU5 records state they do not start NTIO/NTDOS/COMMAND. | No current authority selects CPU3 or CPU5 for a unified first OpenNT profile. | owner-decision-required. |

## Source configuration consequences

The imported `configure.in:574-614` accepts CPU levels 3, 4, 5, or 6, writes
`BX_CPU_LEVEL`, and applies source constraints: SMP requires at least 5 and
x86-64 requires 6.  The checked-in `config.h(.in):697-702` further states that
MSRs require level >=5 and FPU-off is only valid through level 4; lines
733-736 derive APIC on for CPU level >=5.  These explain why CPU3 and CPU5 are
not equivalent compile-time declarations in one target; they do not select one
for this project.

The v2 projection deliberately uses CPU3 with FPU/MSR/APIC disabled.  The
historical CPU5 container's configure options also disable SMP/x86-64 and the
product devices, but CPU5 itself changes the original source-derived APIC and
FPU/MSR constraint surface.  Treating its stock firmware success as permission
to lift the S6 projection would silently change that profile and is rejected.

## Result and next gate

The apparent conflict is **resolved as two separately accepted evidence
layers**, but **ambiguous/owner-decision-required** for any future single
first-profile runtime: CPU3 is accepted only for the minimal MSVC syntax
observation; CPU5 is accepted only for historical stock-firmware observation.
Neither is the selected first OpenNT profile.

Guest evidence does not settle that decision.  The only level-imposing fact is
the stock BIOS `cpuid(1)` path; it is firmware evidence, not guest OpenNT
runtime behavior.  Choosing a unified level would require an owner decision
after a reached first-profile caller identifies whether stock firmware is in
scope.  No guessed guest behavior may bridge that gap.  Until then, no object
model, CPU ledger revision, configuration projection, or build target is
admitted.
