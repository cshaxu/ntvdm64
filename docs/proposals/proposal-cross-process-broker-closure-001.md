# Cross-process broker closure

## Objective

After the applicable single-process WOW16 owner contract closes, recover the
required BaseSrv-style registration, command-queue, notification, and
disconnect-cleanup behavior through public IPC.

## Boundary

The broker owns no guest, COMMAND, SoftPC, or WOW provider behavior and does
not recreate CSRSS or kernel VDM.

## Exit evidence

Prove the selected brokered lifecycle rows with fixed-width copied IPC values
and explicit disconnect cleanup.
