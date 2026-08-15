# M0 T198 S125: COMMAND `54:01` Launch-Plane Composition Map

## Result

S124's `54:01` stop is a fixture observation stop after the active composition
declines the BOP.  It is neither a Bochs invalid-opcode failure nor an OpenNT
dispatcher result.

The current `bx_ntvdm_command_launch_plane_v1` implements only `54:05`
SCS registration.  Its explicit predicate requires service 5, so it cannot
accept `54:01`.  The current native composition manifest likewise excludes
`bx_ntvdm_cmd_get_next_service.c`.

## Original And Replacement Boundaries

OpenNT maps `54:01` to `cmdGetNextCmd`.  It consumes `DS:DX` CMDINFO, uses
`GetNextVDMCommand`, environment/directory/redirection state and NT thread/UI
control.  Directly linking it would reintroduce invasive NT host composition
and is rejected.

The repository already has the approved source-derived replacement component:
`bx_ntvdm_cmd_get_next_service.c`.  It has one coherent contract:

- copied fixed-width CMDINFO gather;
- immutable one- or two-slot BYOB launch plan;
- immutable namespace target/QUIT mapping;
- existing `54:05` registration and host-drive snapshot inputs;
- checked multi-write plus CF-clear four-byte resume;
- paired constrained `54:11` result state.

It owns no command queue, process creation, Win32 APIs, redirection, directory
sync or Bochs semantics.  Its older focused closures (T181/T194) already
prove the component, but it is currently **detached** from this native
composition.

## Exact Next Step

S126 must integrate the entire existing component into
`bx_ntvdm_boot_namespace_composition_v1`: add owned launch-plan/get-next
state, invoke prepare -> mechanical gather -> complete -> checked multi-write
-> commit as one lifecycle, and add its source to the declared manifest.  It
must retain the existing `54:05` registration prerequisite and decline absent
or exhausted slots.  It must not rehost `cmdGetNextCmd`, import its host
dependencies, add a stand-alone `54:01` shortcut or alter Bochs.

## Evidence

- `src/opennt/base/mvdm/dos/command/cmddisp.c:17`
- `src/opennt/base/mvdm/dos/command/cmdmisc.c:25-218`
- `src/opennt/base/mvdm/dos/v86/cmd/command/tcode.asm:540-560`
- `src/bx-vdm/bx_ntvdm_command_launch_plane_v1.c:1-4`
- `src/bx-vdm/bx_ntvdm_cmd_get_next_service.c`
- `tools/build/t198-s50-bx-vdm-composition-manifest.json`
