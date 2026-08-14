# T200 S23 — real-mode entry arming closure

## Question

After the S22 machine stage has completed its mechanical preentry transaction,
can it apply exactly the source-derived real-mode entry delta without exposing
a native CPU object or starting execution?

## Source and ABI basis

OpenNT `base/mvdm/softpc.new/host/src/nt_msscs.c:200-221` restores the
four-byte state at `0x714` and then transfers through only `setCS` and
`setIP`. T200 S17 mapped that contract to `CS:IP=0070:0000` in the existing
startup plan.

The adopted Bochs implementation is `src/bx-core/cpu/init.cc:67-72`:
`apply_real_mode_entry` loads CS, sets RIP/EIP, and invalidates the prefetch
queue. It does not set general registers, flags, other segment registers,
descriptor tables, or execute the CPU loop.

S23 introduces the fixed-width mantle entry record with just CS and EIP. Its
validator rejects EIP above `0xffff`; no other CPU field exists in the record.
The adapter maps the already preflighted startup-plan entry into this record.

## Procedure and result

The machine-stage fixture was run with:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T200S22MachineStageProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s23-entry-arming-r1
```

It starts the S22 stage, arms `0070:0000`, copies the typed entry back from
the mantle, then attempts invalid `EIP=0x10000`. The invalid operation is
rejected and the observed entry remains `0070:0000`. The test resets the
stage and confirms it is inactive.

The complete current composition was source-compiled at
`artifacts/build/t200-s23-current-composition-r1`, and the engine lifecycle
fixture passed twice at `artifacts/build/t200-s23-engine-entry-r1` using the
same S22 engine probe with those current objects. Both records are MSVC x64
`/MT`; no CPU loop or guest instruction ran.

## Limitation

This arms a native CPU but intentionally does not call `cpu_loop`, schedule a
watchdog, observe a BOP, or claim that NTIO now runs. The next package must
make a bounded product execution contract explicit before any first guest
instruction is admitted.
