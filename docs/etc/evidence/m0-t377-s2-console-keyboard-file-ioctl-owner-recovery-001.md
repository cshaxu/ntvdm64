# M0 T377 S2 — Console, keyboard, file and IOCTL owner recovery

## Result

The selected CPU40/x86 formal graph now uses the complete selected original
owner translation units for the S1 console/keyboard and file/seek/IOCTL
workloads.  This is source/binding closure only: it does not claim that KB16,
MEM or GRAPHICS has yet run in the fixed container.

## Keyboard and console

- The formal manifest selects the original `softpc.new/base/keymouse`
  `keyba.c`, `keybd_io.c`, `ppi.c`, `mouse.c` and `mouse_io.c` set, together
  with original host `nt_input.c` and `nt_keycd.c` inputs.  It therefore
  preserves the original keyboard-controller, BIOS-vector and console-input
  control-flow owners rather than replacing them with a project dispatcher.
- `keybd_io.c` owns the original `UpdateKbdInt15` implementation.  The former
  unreachable adapter extraction (`mvdm_int15.*`) and its isolated fixture
  have been removed.  No current production source now selects that duplicate
  implementation.
- Console API compatibility remains confined to the existing named
  `adapter-mvdm-host-out/win32` source-facing binding.  It supplies modern
  public Win32 equivalents at the original host boundary; no synthetic guest
  keyboard event or project-defined input queue is introduced.

## File, seek and IOCTL

- The formal manifest selects the complete original DEM cohort:
  `demfile.c`, `demhndl.c`, `demioctl.c`, `demdasd.c`, `demdata.c`,
  `demdisp.c`, `demgset.c`, `demfcb.c`, `demdir.c`, `demerror.c`,
  `demlock.c`, `demmisc.c`, `demmsg.c`, `demsrch.c` and `demlabel.c`.
- Host-file identities in original AX:BP/AX:SI carrier pairs retain the
  existing session host-resource mapping-manager binding (registered as
  `MVDM-HOST-DIV-103`).  The selected original synchronous pathname/read
  boundary retains its bounded guest-memory lease (`MVDM-HOST-DIV-195`).
- `demioctl.c` is again byte-identical to its OpenNT MVDM source.  Its table
  remains the original `apfnSVCIoctl` table; the prior default-off IOCTL
  recorder and its fixture were removed rather than retained as a second
  provider.  Its `GetVDMAddr` structure accesses are synchronous CPU40
  guest-memory accesses inside the original MVDM owner; they neither publish
  a durable host identity nor cross an asynchronous session boundary.

## Verification

`cmd.exe /c build\\M0-T371\\S2\\formal-x86\\run-ninja-parallel.cmd
original-softpc-process.exe` rebuilt the affected formal graph and linked the
final product successfully after the cleanup.  The selected original TU set
was already in the formal product; S4 will add its focused positive/negative
tests.  S5 alone owns the fixed-container observations.

## Disposition

S2 is source/binding closed.  Timer/IRQ and video controller recovery remains
T377 S3, and no selected workload runtime result is asserted here.
