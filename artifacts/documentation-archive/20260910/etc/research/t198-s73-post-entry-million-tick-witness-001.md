# M0 T198 S73: Post-Entry Million-Tick Witness

## Question

With the S71 NTIO inputs and composition unchanged, what is the first boundary
after the no-VDD `50:36` continuation when the finite observation bound is
raised from `8192` to `1000000` ticks?

## Inputs and procedure

Only `t198_s23_native_ntio_boundary_fixture.cc` changed: its copied finite-run
tick budget is `1000000`; IPS was already `1000000`. The fixture still uses
the same NTDOS/COMMAND inputs and one-byte HLT `TARGET.COM`; it does not add
`QUIT.COM`, guest media, devices, or a provider.

`tools/probe/Invoke-T198S48NtioCleanRebuildProbe.ps1` rebuilt the complete source
closure in `artifacts/build/t198-s73-post-entry-million-ticks-r1` for MSVC x64
`/MT`, then ran `command-r1.exe`. The wrapper reports exit `2` because the
fixture deliberately exposes an observed generic-UD stop; the native run log
is the result record.

## Observation

The run reaches `50:3C` and returns finite status `4`, which
`bx_ntvdm_finite_run.h` names `COMPLETED_UD_STOP`:

```text
t198-s23 status=4 ... observed-stop=1 next=50:3c ...
terminal=1:0032:5331 snapshot=1:00dea:4
```

The previous `50:36` marker is no longer the terminal. The fixture's existing
generic observer records the first uncomposed BOP and requests typed stop;
therefore this is a controlled observation boundary, not a build error,
guest crash, DOS exit, or CLI result.

## Interpretation

OpenNT `dossvc.h`/`demdisp.c` name `50:3C` `SVC_PDBTERMINATE` /
`demTerminatePDB` in `demsrch.c`. Its original owner is the DEM search/VDD
cleanup plane. Earlier retained source analysis already distinguishes its
finite search-session cleanup from unavailable VDD and device cleanup.

The next task must audit whether the already source-derived, selector-aware
DEM session-lifecycle provider can re-enter the current common composition
without importing the detached historical adapter runtime. It must not add a
one-off raw-byte handler or enable VDD, floppy, FDISK, ambient search, or
Bochs semantics.
