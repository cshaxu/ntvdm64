# M0 T396 S3 original WOW `-a` contract frontier

Date: 2026-09-07  
State: evidence; S3 remains active

## Question

Does the standalone Win16 declaration provide the two original process-parser
inputs that select the WOW bootstrap and its first guest kernel target, and if
so what is the first reproducible frontier after that correction?

## Inputs and source audit

The audited source owners are:

* `mvdm-host/softpc.new/host/src/nt_reset.c`, which derives `VDMForWOW` from
  the original `-w` process option;
* `mvdm-host/dos/command/cmdmisc.c`, whose original
  `GetWowKernelCmdLine` parses `-a <kernel>` from that same process command
  line; and
* `adapter-mvdm-host-out/basesrv/source/mvdm_image_classification.c`, which
  resolves a declared image token before app publishes the copied BaseVDM
  record.

The resolver turns the observer's `system32\\krnl386.exe` token into a quoted
absolute path.  `app_launch_declaration_publish` then separates that resolved
path into `target_application` and the command tail.  The S3 correction passes
this already-resolved `target_application` as `-a` after the original `-w`
selector.  It does not parse a guest command, replace `GetWowKernelCmdLine`,
install an app loader, or alter any guest byte.  This replaces the prior
app-side `system32\\krnl386.exe` compatibility-search fallback with the
original parser contract.

## Procedure

The formal x86 parent was rebuilt from
`build/M0-T396/S3-formal/` by explicitly compiling the two changed app
objects and relinking `original-softpc-process.exe`.  Its SHA-256 is:

`9EDF93EB588FB93852009D721FF972D4CC9F13DCC83C71F31F08E254644FBB14`.

The existing console-owning observer then copied only that product executable
to the hash-verified short-root stage `O:\\ntvdm64\\y` and ran:

```text
console-startup-observer.exe <staged ntvdm32.exe> O:\ntvdm64\y \
  <workspace report> -f -o --command system32\krnl386.exe \
  --observation-timeout-ms 30000
```

The immutable stage manifest hash was
`13222076ABF93AA2158172F16B62B5A348D7A29B0B082B1144EDDCF7AD6E9A84`.
The observer was non-debug, supplied no Console input, and did not request a
child diagnostic environment variable.

## Observation

The observer exited rather than timing out:

```text
container=console-owning-nondebug
result=exited
exit=0xc0000005
timeout-ms=30000
fixed-system-root=O:\ntvdm64\y
```

The product's unhandled-exception record reports access target `0x00000004`.
ASLR changed the absolute base between the two captured rows, but both resolve
to the same image RVA `0x000673EA`:

```text
base=00450000 address=004B73EA
base=002C0000 address=003273EA
```

The formal map assigns that RVA to original
`original-softpc-video:ega_prts.obj`, function `ega_crtc_outb` (function start
RVA `0x00067310`).  The nearby original source is the EGA CRTC overflow-register
handling path, before it calls `set_screen_height_hi_recal` and
`flag_mode_change_required`.  No BOP report was emitted; therefore this run
does not reach DOSX's `51h` loader route, does not load `WOW32.DLL`, and makes
no `W32Init` or `W32Dispatch` claim.

## Interpretation and limits

Confidence is high that the `-a` carrier is now source-shaped: its value is
resolved before splitting, its parser owner is original COMMAND, and the
runtime frontier changes reproducibly from the prior post-DOSX attribution to
the earlier original EGA initialization path.  Confidence is deliberately low
on the *cause* of the EGA access violation: the exception address maps inside
the function but does not by itself distinguish an earlier memory corruption,
a host-video initialization ordering defect, or a presentation adapter
defect.

Consequently S3 must not repair display/graphics behavior as an incidental
WOW32 change.  The existing display-backend proposal is the proper admission
path if a separately scoped video/presentation repair is approved.  The
present S3 evidence establishes the exact first terminal result and preserves
the no-fabricated-loader stop condition.

## Follow-up

Audit the original EGA host initialization and its current presentation
binding against the separately proposed display-backend work.  Any functional
video change requires a revised/admitted packet; until then, retain this as
the Win16 bootstrap frontier and do not treat staged `wow32.dll` presence as
success.

## NTVDMx64 comparison disposition

The approved read-only comparison tree
`O:\\repos.external\\ntvdmx64\\ntvdmpatch` does retain an archived
`patches/old-src/ega_crash.patch`.  It changes a later
`nt_ega_lo_graph_std` paint path: it clips out-of-bounds rectangles and avoids
zero-count decrement loops.  It does **not** modify `ega_crtc_outb`,
`recalc_screen_params`, or the host adaptor-initialization sequence where this
run terminates.  It is therefore neither evidence of the present cause nor a
candidate import.  The comparison is recorded solely to avoid rediscovering a
known but non-matching display patch.

## Address-attribution correction

The preceding first interpretation incorrectly compared the reported *runtime*
virtual address directly against the link map's preferred-base virtual address.
The PE preferred image base is `0x00400000`; the observer reported a loaded
base of `0x00450000` and runtime address `0x004B73EA`.  The correct RVA is
therefore `0x000673EA`, whose preferred-base map address is `0x004673EA`.

That location is original `host_applInit` in
`softpc.new/host/src/nt_reset.c`, not `ega_crtc_outb`.  Its disassembly is the
source statement `psz = *++temp_argv`: the observed access target `0x00000004`
means that the original argument-vector carrier was null at this point.  This
correction supersedes the earlier EGA attribution and invalidates the display
patch comparison as causal evidence.  The S3 conclusion remains unchanged:
no BOP `51h`, `WOW32.DLL`, `W32Init`, or `W32Dispatch` was reached.  The next
owner boundary is instead the host-only argument carrier from app through
original `ntvdm.c` and `host_main` to `host_applInit`.
