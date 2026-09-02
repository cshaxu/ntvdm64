# M0 T378 S1 — DOS-extender workload and owner admission

## Immutable artifact identity

The selected non-interactive workload is the original DOSX server carrier,
declared to the existing original COMMAND/guest `$Exec` route as:

```text
DOSX.EXE
```

The paired original XMS device is already selected by the same immutable
runtime configuration:

```text
CONFIG.NT: device=%SystemRoot%\system32\himem.sys
AUTOEXEC.NT: lh %SystemRoot%\system32\dosx
```

The workload does not create a replacement image or alter either byte stream.
The identity chain is:

- `src/mvdm-guest/dos/v86/dev/himem/HIMEM.SYS`, 4,768 bytes, SHA-256
  `08aa2c47d835460ed3067fa7d6f8a3b37edeca524ad102b0588fdd1bf389ce08`;
- `build/output/dos/HIMEM.SYS` and
  `O:\\ntvdm64\\system32\\HIMEM.SYS`, byte-identical to that mirror;
- `src/mvdm-host/dpmi/486/dosx.exe`, 36,616 bytes, SHA-256
  `c5af29a29abf167b243daabf877459e8278b8c9a339bf8e1e2576ead5f6ceeff`;
- `build/output/dos/DOSX.EXE` and
  `O:\\ntvdm64\\system32\\DOSX.EXE`, byte-identical to that original
  DOSX artifact.

The stage tool already lists both system32 assets as mismatch-refusing media.
S1 therefore admits no staging change.

## Complete immediate owner chain

The selected chain is package-shaped rather than BOP-selected:

1. `app` establishes the executable-relative original system root.  Original
   `cmdconf.c:ExpandConfigFiles` expands `%SystemRoot%` in CONFIG.NT and
   AUTOEXEC.NT; no project DOS loader constructs a device or extender image.
2. Original SoftPC `nt_msscs.c:InitialiseDosEmulation` calls original
   `scs_init`; `scs_init` calls `XMSInit` before guest command execution.
   Original `config.c` keeps at least one megabyte XMS specifically for HIMEM
   and DOSX.
3. Original `xms.486/xms.c`, `xmsa20.c`, `xmsblock.c`, `xmsdisp.c`,
   `xmsmisc.c` and `xmsumb.c` own startup, A20, allocation, UMB and INT 15
   behavior.  T371 proved the original `suballoc` callback contract.  The
   mapping manager remains only the established unavailable-VM allocation
   backing boundary.
4. The original DOS CONFIG device path loads immutable `HIMEM.SYS`.  The
   original DOSX artifact is then reachable by the already proved original
   COMMAND/DEM `$Exec` file route, whether initiated from AUTOEXEC.NT or by
   the declared `DOSX.EXE` command used for the bounded observation.
5. The immutable DOSX carrier owns its real-mode startup, selector/page/stack
   construction and DPMI request sequence.  Original host
   `nt_bop.c:MS_bop_3` calls original `dpmi32.c:DpmiDispatch`; its selected
   25-entry table and original supporting DPMI32 bodies own the host side.

## Boundary disposition

- The formal CPU40/x86 graph already contains every selected XMS and DPMI32
  original source body.  No second dispatcher or emulator is admissible.
- Numeric XMS addresses, DPMI selectors and in-machine BOP service bytes keep
  their original representation.  Only host-object/guest-memory identities
  retain session mapping-manager mediation.
- Kernel VDM, VDD/VCD, private CSR and an arbitrary protected-mode success
  fabrication remain source-shaped unavailable/later-owner boundaries.  None
  is needed to admit the pre-handoff XMS/HIMEM/DOSX path.
- T377's final `0x48` session-dispose result is post-shell and cannot be used
  to assign a DOSX/XMS/DPMI defect.  It is retained as independent later
  session-lifecycle evidence.

## S2 admission

S2 owns the whole original boot cohort from CONFIG.NT device processing through
HIMEM's XMS entry, A20/UMB/INT15 registration and its original failure/cleanup
direction.  It must first prove the actual configuration/device execution
route reaches the selected original owner under the current product; it may
not make a direct `52:xx` repair or synthesize device success.
