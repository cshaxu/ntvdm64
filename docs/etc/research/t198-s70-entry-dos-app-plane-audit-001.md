# M0 T198 S70: `50:36` DEM DOS-App Entry Plane Audit

## Observed input

The clean source-built run at
`artifacts/build/t198-s70-entry-dos-app-audit-r1` reaches `C4 C4 50 36` at
`0032:5a70` with:

| Contract field | Value |
| --- | --- |
| DS:SI (app entry) | `0bb8:0100` |
| AX:DI (initial stack) | `0bb8:fffe` |
| DX (PDB) | `0bb8` |
| BX (initial AX) | `0000` |

## Original owner and global plane

OpenNT `dossvc.h` identifies service `36` as `SVC_DEMENTRYDOSAPP`.
`msproc.asm` and `lmstub.asm` issue it immediately before installing the
guest application stack and far-returning to `DS:SI`.  `demmisc.c` confirms
that the non-debug service has one host-visible operation:
`VDDCreateUserHook(PDB)` after the first call.

The active DEM plane correctly classifies this endpoint as an original-noop
owner rather than as namespace, raw media, or a generic service to emulate.
The retained `vdd_create_user_notify` provider is exact for `50:36` and
expresses the current finite profile's empty VDD registration list as a
four-byte resume.  It does not launch a host process, emulate V86, alter the
guest app entry/stack, or add VDD behavior.

S71 may compose this existing no-VDD disposition directly before generic DEM
ingress.  The following guest `$Exec` code remains owner of actual app entry.

