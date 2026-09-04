# M0 T390 S5 P1 — pure-DOS normal Console key observation

## Purpose

Determine whether the selected pure-DOS child can consume an ordinary Console
key through the original host and guest input path. This does not use a host
command parser, BOP injection, guest-memory write or guest-media change.

## Fixed run

The unchanged staged CPU40/x86 product and the same valid pure-DOS PIF/media
were run in the non-debug console-owning container for 30 seconds. The observer
waited for the existing BIOS keyboard-idle witness, then supplied the normal
Console key pair for `v` as the first character of `ver`.

The durable report shows:

* the original Console event worker receives the key;
* original keyboard-controller intake accepts scan `31h`;
* original ICA receives the pending IRQ1 request;
* CPU40 records the pending hardware interrupt while guest `IF=0`;
* no original port-60 read follows, so the observer intentionally stops after
  the first character rather than queueing an artificial line.

The run times out. It produces no visible Console text, no DOS `CON`
completion and no `VER` result.

## Interpretation

The result confirms that PIF selection, BaseVDM command delivery and host
Console worker startup are no longer the blocker. It does **not** prove prompt
readiness: the witness is an original BIOS idle edge and can occur while normal
startup code still has interrupts disabled. The next P within S5 must trace the
original child-EXEC `CLI/STI/IRET` continuation and CPU40 interrupt condition;
it must not force IF, synthesize IRQ1, drain port 60 from the host, or modify a
guest input buffer.
