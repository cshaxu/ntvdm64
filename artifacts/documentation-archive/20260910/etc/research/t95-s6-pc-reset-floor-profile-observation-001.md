# T95 S6 PC Reset-Floor Profile Observation 001

## Result

The one admitted run of the original all-disabled-ATA profile is a valid
negative result. Its profile hash is
`DE88B82A698F8CB9AC66AAAB50FF62D879A46733DE815CAD60196D5527447101`.
It starts r4 without adapter or guest input, reports `PCI support: yes,
enabled=no`, and initializes neither PCI/440FX/PIIX3/ACPI/PCI IDE nor VBE.

It then panics at `main.cc:1044` because the unconditionally called
`bx_gui->update_drive_status_buttons()` reaches the hard-drive stub when all
ATA channels prevent `harddrv` plugin loading. Nogui asks for interactive
panic action, so the 30-second watchdog terminates the process. Native reset
and benchmark activation occur, but controlled stop does not.

## Decision

The all-disabled-ATA profile is rejected. The next coherent profile must keep
one empty original ATA controller solely to retain the original hard-drive
plugin required by main's lifecycle call; it may not attach host or guest
media. No source patch or second run occurred under this admission.
