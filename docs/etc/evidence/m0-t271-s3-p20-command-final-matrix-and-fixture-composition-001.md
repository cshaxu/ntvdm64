# M0 T271 S3 P20 — COMMAND final matrix and fixture-composition recovery

## Final source-owner audit

The active `opennt-bop` COMMAND path now enters the true source subset
`softpc.new/host/src/nt_bop.c:MS_bop_4`, which loads its service byte and calls
the original `command/cmddisp.c:CmdDispatch`. The active 17-slot
`apfnSVCCmd` table and `cmddata.c` global storage remain imported source bodies.
`runtime_command_misc_dispatch_source_command` only applies the necessary
modern range guard and calls that original table; it does not select or
replace a COMMAND service.

The final retained project bodies have one explicit disposition: app startup
binding is in `app`; generic frame/CCPU mechanics are in `adapter-bop` and
`adapter-softpc`; public Win32/VDM API shapes are in `adapter-win32`; child
and event capabilities are in `opennt-host`; and source-specific pointer,
worker and terminal glue is a registered private overlay. Redirector remote
pipes, WOW broker/loader, real console input and DOS EXEC/PSP return remain
named future owners, never fabricated COMMAND success.

`command_native_session` and the outer generic route remain current BOP-entry
composition for the original `MS_bop_4` body; they are not a project-defined
COMMAND dispatcher. Their all-family route elimination is reserved for T271/S5.

## Fixture corrections

Two final-matrix failures were fixture integration defects, not product
semantic failures. `t231-s5-command-config-direct-fixture` now enables only
the existing test dialog suppressor around its deliberate terminal-error
assertion; product dialogs remain public Win32 dialogs. The machine pending
and native cmdExec32 fixtures now explicitly bind the official
`adapter-bop` route through the `adapter-bochs` opaque callback, rather than
assuming CPU linkage calls a test-defined generic-UD symbol.

## Verification

Outside the sandbox, formal Ninja at `build/M0-T271-S3/r003` linked and ran
all focused COMMAND fixtures: `t231-s2` through `t231-s10`, `t234-s2`
dynamic-environment/directory/PIF, and `t236-s1` / `t236-s2` child lifecycle,
machine pending/resume and native cmdExec32. Each exited zero. The final
native fixture observed actual Bochs `#UD` through the opaque adapter route,
pending worker completion, resume, `AL=37`, and guest command-tail CR
normalization. The final application target also links.

This proves S3 source/package closure only. It does not prove continuous guest
DOS child EXEC/PSP return, Redirector IPC, WOW16, multi-VDM CSR/BaseSrv broker,
or real console-input injection.
