# OpenNT Full-Set Build Campaign

## Scope and Rule

Status: descriptor-campaign reference, reconciled 2026-08-08. The direct
guest source-build portion of batches 1--3 is complete under the OpenNT-tools16
baseline; the remaining work is runtime admission and separately classified
host/WOW scope, not a return to retired substitute toolchains.

The campaign covers every declared output in
`opennt-mvdm-artifact-inventory.json` (75 declarations, including the single
directory marker `dest=.`), plus output names that must be resolved from an
install or parent makefile rule. A missing `dest=` expression is not an
exclusion: `dos/v86/cmd/backup`, `graphics`, and `qbasic` remain in scope until
their concrete payload names and source closures are recorded.

No candidate is called retail-identical, runnable, or distributable merely
because it links. Each completed row needs a fresh-stage source closure, exact
tool commands and hashes, byte count, packaging location, and an explicitly
separate runtime result.

## Campaign Batches

1. **Bootstrap and existing closure.** `buildidx`, `getsize`, `stripdd`,
   `stripz`, plus `ntio.sys`, `ntdos.sys`, `himem.sys`, `redir.exe`, `command.com`,
   `append.exe`, `share.exe`, `setver.exe`, `loadfix.com`, `fastopen.exe`, and
   `quit.com`. Their unchanged-source OpenNT-tools16 builds are recorded;
   runtime validation is pending.
2. **Real-mode command and device payload.** `ansi.sys`, `keyboard.sys`,
   `country.sys`, `debug.exe`, `edlin.exe`, `exe2bin.exe`, `kb16.com`,
   `mem.exe`, `nlsfunc.exe`, the install-resolved editor/QBASIC payload, and
   `graphics.com` with `graphics.pro`. Every named source-built member has a
   current OpenNT-tools16 stage; `GRAPHICS.PRO` is static data and the four
   editor/QBASIC executables remain explicit binplace-only external inputs.
   Runtime NLS, keyboard, console, and utility behavior remain unverified.
3. **Protected-mode and namespace payload.** The DPMI `dosx.exe` output,
   `dpmi32`, `xms486`, guest `redir.exe`, and host `vdmredir`. `DOSX.EXE` and
   guest `REDIR.EXE` have current source-build records. This batch owns the
   interface evidence for DPMI/redirector behavior; it cannot claim either
   feature usable before the corresponding non-invasive host contract exists.
4. **Monitor/host-support tree.** `bde`, `dbg`, `dem`, `oemuni`, `sim32`,
   `suballoc`, `vdmdbg`, `vdmexts`, `ntvdm` (both descriptors), and the
   SoftPC components `bios`, `ccpu386`, `comms`, `Cvidc`, `dasm386`, `debug`,
   `disks`, `dos`, `keymouse`, `support`, `system`, `unix`, `video`, `convert`,
   `dat2obj`, `rename`, `resource`, `src`, and `vdmdebug`; also `forcedos`,
   `graftabl`, `pifedit`, and `win`. These are host/support research outputs,
   not automatically part of a DOS payload package.
5. **WOW16, driver, and sample tree.** The mouse, keyboard, display, comms,
   sound, printing, and related WOW16 driver makefiles; the Win16 subsystem
   makefiles (`commdlg`, `ddeml`, `gdi`, `kernel31`, `mciole`, `mmsystem`,
   `ole`, `rasapi16`, `regedit`, `shell`, `system`, `timer`, `toolhelp`,
   `user`, `wfwnet`, `win87em`, `winoldap`, `winsock`, `winspool`, `wowdeb`,
   `write`, and tests); `wow32`, `wowfax`, `wowfaxui`; and VDD sample targets
   `adlibvdd`, `vcdex`, `fax32`, `vdd`, and `vsbd`. The `MOUSE.DRV` family is
   tracked here as a WOW16 dependency, not mislabeled as a DOS `.SYS` file.

## Current Execution Order

The direct guest source/tool closure is recorded in
`HISTORICAL-TOOLCHAIN-REBASE-2026-08-07.md` and
`NT4-GUEST-ARTIFACT-CLOSURE.md`: 22 unchanged-source builds, seven static
companions, and four transparently external `binplace` inputs. It does not
admit any guest runtime feature. The next payload-facing proof is the bounded
P0 bootstrap/command trace after the M70 backend gate, followed only by
trace-reached P1 features. Batches 4 and 5 retain whole-system research scope,
but remain separately classified and are not default DOS runtime work.

The authoritative per-descriptor list, including source paths and unexpanded
`TARGETNAME` expressions, remains
`opennt-mvdm-artifact-inventory.json`; this document defines the order and
acceptance rule rather than duplicating a lossy output-name guess.

The current local-stage result is separately audited in
`OPENNT-STAGED-ARTIFACT-AUDIT.md`. A build log, stage number, or matching
manifest hash is not by itself a loader-valid image claim.
