# Original COMMAND exit owner

Frozen source reconciliation SHA-256 F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196. `dos/command/cmdexit.c` is byte-identical to both frozen OpenNT editions but remains pending: matching source and linking do not prove all product behavior.

Original guest COMMAND source `dos/v86/cmd/command/tcmd2b.asm` branches on `permcom`: nonpermanent commands use `free_com`, while permanent COMMAND issues SVC_CMDEXITVDM (except singlecom), whose source comment says never returns. This is source context only, not proof the installed immutable COMMAND.COM was built from that exact file. `softpc.new/base/bios/bios.c` binds MS BOP54 to MS_bop_4; `nt_bop.c` reads the service byte and calls CmdDispatch; `dos/command/cmddisp.c` table entry zero is cmdExitVDM; `cmdexit.c` invokes TerminateVDM. `nt_reset.c` selected TerminateVDM calls original terminate path, whose host termination calls ExitVDM and ExitProcess; current worker-lifecycle adaptation is owned elsewhere and must be evaluated with NTSRV and console handoff.

Historical `build/t448-s11-regression/wow-trace-r2/ntvdm.exe.map` identifies `_cmdExitVDM` in original-mvdm-command:cmdexit.obj, `_CmdDispatch` in cmddisp.obj and `_TerminateVDM` in nt_reset.obj. This proves historical link ownership, not current binary identity or execution. `tests/component-integration/fixtures/t231_s9_command_exit_direct_fixture.c` checks a separate historical typed controlled-stop composition, not the current production caller and broker. Historical user acceptance of cross-carrier exits is useful scenario evidence but does not prove the exact installed guest source identity or all abnormal and concurrent teardown.

Remaining: verify current PE/map identity and real installed guest dispatch, permanent/nonpermanent paths, worker completion, NTSRV receipt and frontend handoff for normal and management shutdown with nested carrier, plus server-disconnect and repeat-exit boundaries. No specific source mismatch or runtime fault proven. No test was run for this review.

The currently present `build/.ninja/pkg-uuid-x86/worker/ntvdm.exe.map`
also names `_cmdExitVDM` and `_TerminateVDM` in the corresponding objects.
Its map SHA-256 is `6AD07BD20F70CAD6F882600AC7BEAF5B9ECF86A73E85CE2FB309A2C2B4F402E0`;
the adjacent EXE hashes `FFA2C1505562EAA042368DE0B6FDF9236BE67AA246FC559971BD06547AF63148`,
while package-c `system32/ntvdm.exe` hashes
`01A5B459072CA23F49AA43285580BD9017395C936F9DA8B2F18EE7223A52D1F6`.
The mismatch means this map cannot be used as evidence of the deployed image's
exact linkage, even though it is a useful current graph observation.
