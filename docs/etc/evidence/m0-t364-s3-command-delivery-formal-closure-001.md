# M0 T364 S3 — COMMAND delivery formal closure

## Question

Are the selected original COMMAND delivery owner and its bounded Base VDM
binding actually in the CPU40/x86 product rather than merely passing a
standalone fixture?

## Procedure

Used the existing formal product root without reconfiguration:

```text
ninja -C build/M0-T362/S2/x86 original-softpc-process.exe
ninja -C build/M0-T362/S2/x86 -t query original-softpc-process.exe
```

## Result

The build reported `no work to do`.  The final `process_link` input list
contains all selected owners exactly once:

- `obj/app/entry.obj` and `app-machine-shell.lib` for app/session assembly;
- `original-mvdm-command.lib` for original `cmddisp.c`/`cmdmisc.c`/COMMAND
  state and BOP dispatch;
- `basesrv-bindings.lib` for the same-shaped BaseClient/BaseSrv local boundary;
- `session.lib` and `broker.lib` for copied wake/record ownership;
- original CPU40/SoftPC host, BIOS, system, disk, video, C-VID, comms and DOS
  libraries, with the selected original CCPU archive.

The queried product does not list a CPU30 archive, a Bochs archive, or a
standalone COMMAND/BOP provider.  `original-mvdm-command.lib` plus
`basesrv-bindings.lib` is therefore formal link proof only; it does not claim
that a later guest command, DOS EXEC, child return, WOW or native child has
executed.

## Follow-up

S4 may use one frozen product observation.  It must retain the T363 package
root, product, observer and declared `/C EXIT` input.  The result may only
prove the command-delivery boundary or identify one immediate original owner;
it cannot select a leaf repair.
