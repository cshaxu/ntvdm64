# Proposal: CPU40/NTDOS permanent COMMAND bootstrap recovery

## Question

How can the original NTDOS permanent `COMMAND.COM` bootstrap proceed from its
source-shaped DOS pathname through the existing Direct host/media namespace
without changing the original DOS/DEM file, error, or command-image contract?

## Evidence baseline

T353 proves that `54:0C` returns and that the original `ProcessConfig ->
doconf` pre-scan executes. Its immutable CPU40/x86 observation then reaches
the unique original `sysinit1.asm::comerr -> badfil -> stall -> 50:3D`
terminal. This is a permanent command-name/open failure, not an unimplemented
`50:12` or `50:3D` provider.

## T354 S4 decision

The product will not create a temporary drive letter, DOS-device alias,
`subst` mapping, junction, virtual boot volume, or guest overlay to conceal a
long installed package path. The original `cmdconf.c` source writes the
literal `shell=` value and original NTDOS stores that value in its fixed
64-byte `sysconf.asm::commnd` field. A package is therefore admissible only
when its public short path makes that unchanged value no more than 63 visible
bytes. Otherwise app shows a clear installation-path error and returns its
media-admission outcome before original host startup.

## Scope

- Audit one whole original NTDOS permanent-command cohort: pathname source,
  DOS name resolution, original `INT 21h` open/seek/read/close/handle flow,
  DEM host boundary, command image location and handoff;
- retain the original `comerr`/`badfil`/`demExitVDM` failure direction;
- recover only original source, same-shaped binding, or a registered minimal
  overlay selected by that complete source audit;
- use the fixed CPU40/x86 media layout and session mapping constraints.

## Exclusions

No individual BOP success fabrication, guest rewrite/rebuild, virtual boot
volume, media mutation, CPU30, Bochs, x64 runtime, BaseSrv/CSRSS, WOW,
`cmdExec32`, child/parent `EXEC`, device/INSTALL completion or graphics
workload claim.

## Completion record

An admitted packet must prove either the original command open/load progresses
through the selected image handoff or a further exact source-owned terminal
after a fresh formal CPU40/x86 link and one fixed-container durable
observation. It must distinguish permanent-command bootstrap from later
interactive COMMAND execution.
