# M0 T310 S4 P5 — original PIC/PIT/IOS source-composition evidence

## Scope

This evidence closes the bounded original `softpc.new/base/system` and
`softpc.new/base/support` composition step.  It selects the original source
manifests, rather than treating PIC/PIT/DMA/IOS as Bochs facilities or as
individual replacement shims.

The selected original C translation units are:

- `base/system/sources` — `at_dma.c`, `cmosnt.c`, `ica.c`, `idetect.c`,
  `illegalp.c`, `qevnt.c`, `rom.c`, `timer.c`, `timestrb.c`, `dummy_nt.c`,
  and `unexp_nt.c`;
- `base/support/sources` — `terminat.c`, `time_day.c`, `ios.c`, `main.c`,
  `get_env.c`, and `xt.c`.

This includes the original PIC (`ica.c`), timer/PIT (`timer.c`), CMOS,
DMA, ROM and IOS port-routing implementation.  It neither enables every
optional device nor claims that the selected SoftPC backend can yet create a
runnable machine.

## Minimal source bindings

Two source-preserving bindings were necessary:

- `base/system/idetect.c` now includes the same selected `vdm.h` declaration
  through the declared ABI include path instead of a relative path whose
  original private-tree depth no longer exists.  This is registered as
  `MVDM-HOST-DIV-024`.
- The adapter-owned historical `ntrtl.h` declaration subset now carries the
  unchanged `CSHORT`/`TIME_FIELDS` layout and
  `RtlTimeToTimeFields`/`RtlTimeFieldsToTime` declarations required by the
  original CMOS body.  This is `ADAPTER-WIN32-015`; it calls no newly authored
  CMOS behavior and leaves implementation in public `ntdll`.

## Formal verification

The following commands were run outside the sandbox:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\build\New-T310OriginalSoftpcNinja.ps1 -Architecture x64 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build\M0-T310\S2\softpc\x64 original-softpc-candidate
ninja -C build\M0-T310\S2\softpc\x64 original-softpc-forced-closure.dll

powershell.exe -ExecutionPolicy Bypass -File tools\build\New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build\M0-T310\S2\softpc\x86 original-softpc-candidate
ninja -C build\M0-T310\S2\softpc\x86 original-softpc-forced-closure.dll

"C:\Program Files\Git\bin\bash.exe" -lc "node tools/audit/Export-T310ForcedLinkLedger.mjs . x86 s4-p5; node tools/audit/Export-T310ForcedLinkLedger.mjs . x64 s4-p5"
```

Both candidate graphs archive all selected original CCPU, BIOS, keymouse,
system, support and host-root sources under MSVC `/MT`.  The whole-archive
audit deliberately remains non-runnable.  Its physical unresolved count is
231 on x86 and 230 on x64, reduced from P3's 264/263 after direct inclusion of
the original system/support bodies.

The P5 ledgers classify all but four forms on each architecture.  The four
remaining forms are source-located—not candidates for a synthetic helper:

- `dwWNTPifFlags` — original `softpc.new/host/src/nt_pif.c`;
- `IdleDisabledFromPIF` — original `softpc.new/host/src/config.c`;
- `ExternalWaitRequest` — original `softpc.new/host/src/nt_unix.c`;
- `host_fdisk_term` — original `softpc.new/host/src/nt_fdisk.c`.

An attempted direct selection confirms that they constitute the next bounded
host-control/fixed-disk workset: `config.c` needs the existing private
`pmvdm.h` declaration carrier, `nt_fdisk.c` reaches a wider historical file
control declaration set, and `nt_unix.c` has pre-ANSI/legacy CRT declaration
conflicts.  They are retained as named original-source candidates; they are
not replaced in this P5 package.

## Result

P5 establishes direct original composition for the PIC/PIT/CMOS/DMA/IOS
machine-control group on x86 and x64.  The required default-SoftPC and future
x86/x64 × SoftPC/Bochs matrix policy remains a later S5 runtime verification
obligation; this archive/link evidence does not claim that any backend is
runnable.
