# M0 T198 S128: COMMAND `54:0E` Reuse/Admission Map

## Original owner and contract

Pinned OpenNT `base/mvdm/inc/cmdsvc.h` defines service `0Eh` as
`SVC_GETKBDLAYOUT`.  `dos/command/cmddisp.c` places it in the original
`CmdDispatch` table as `cmdGetKbdLayout`; its implementation is
`dos/command/cmdkeyb.c`.

The original input is the prior KB16-installed flag in `DX` and two guest
buffers at `DS:SI` and `DS:CX`.  Success writes a KB16 executable/options and
returns `DX=1`.  Its no-install branch returns `DX=0` and calls
`cmdInitConsole`.  The positive branch depends on console-layout discovery,
registry data, SystemRoot files, VDM guest pointers and historical console
state.  It cannot be directly linked into the CLI composition.

## Existing candidate and classification

`src/bx-vdm/bx_ntvdm_cmd_keyboard_layout_service.c` is a finite,
source-derived CLI profile provider: it accepts only a real-mode
`C4 C4 54 0E`, resumes after four bytes and writes low `DX=0`.  It neither
reads its two guest buffers nor provides a keyboard device, registry, KB16,
KEYBOARD.SYS, VDD or Win32 console operation.  This is a deliberately bounded
rehost of the original no-install outcome, not an original OpenNT object.

The candidate is not currently part of the T198 composition manifest and
`bx_ntvdm_boot_namespace_composition_v1_handle` never routes to it.  It is
only reached through the detached historical `bx_ntvdm_adapter_runtime.c`
chain, whose legacy plane gate must not be imported into the new composition.

The global route already has the needed structural ownership: COMMAND service
14 classifies as component `CONSOLE` with deferred disposition in
`bx_ntvdm_command_plane_v1`.  Therefore reuse is admissible only when the
candidate is invoked after common ingress, provider selection and COMMAND
plane classification establish that exact `54:0E` identity.  The candidate
itself must remain selector-specific; Bochs remains uninvolved and
selector-blind.

## Disposition

**Reuse with bounded adaptation.**  The next implementation slice may add the
existing candidate to the manifest and call it from the composed COMMAND
plane only after the three global classifications.  It must record the
contained no-install profile and the omitted original `cmdInitConsole` effect
as an intentional CLI capability substitution.  It may not add a keyboard
device, guest-buffer write, VDD/Win32 console bridge, direct `CmdDispatch`
link or native retry.  A focused adapter-only test must prove exact identity,
the `DX=0` typed result, normal four-byte resume and decline of an unselected
or malformed request.
