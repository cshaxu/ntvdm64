# M0 T285 S2 — original `nt_bop.c` binding audit

## Question

Can the reached control surface of the exact original
`mvdm-host/softpc.new/host/src/nt_bop.c` be given a complete, architecture-
correct binding disposition without enabling any BOP selector or changing an
OpenNT source body?

## Inputs

- The hash-pinned T285/S1 workset and its x86/x64 selector-disabled static
  archive evidence.
- Exact original `nt_bop.c`, its original include declarations and reached
  implementation anchors (`nt_error.c`, `nt_unix.c`, `nt_vdd.c`, `nt_umb.c`,
  DEM, COMMAND, XMS, DPMI, monitor, VDD, Redirector, WOW and debugger
  packages).
- Current named boundaries: `adapter-mvdm-host-in`, the seven
  `adapter-mvdm-host-out/*` families, `adapter-bochs`, and `session`.

## Procedure

1. Enumerated every callable external contract reached by the original BOP
   functions, including the dynamically loaded provider tables and durable
   translation-unit identities.
2. Recorded its original caller/order, declaration anchor, one final owner,
   exact no-enable or original failure behavior, and the applicable mapping
   restriction in the companion binding ledger.
3. Checked that the only machine-facing paths resolve through the existing
   same-shaped SoftPC family and typed `adapter-bochs` boundary; the ingress
   adapter itself remains selector-blind.
4. Rebuilt the unchanged original static BOP island for x86 and x64 using the
   formal Ninja/MSVC graph under `build/M0-T285/S2/`.

## Observations

- The denominator is 21 source contract groups.  There are no implicit
  provider calls: three are existing same-shaped adapter contracts, one is a
  future overlay boundary, and seventeen have an explicit later owner.
- DEM, COMMAND, XMS, DPMI, WOW, Redirector, VDD, debugger and monitor
  selectors remain disabled.  Their dispatch order and original failure paths
  are recorded rather than replaced by a generic route.
- `MS_bop_7` retains its source-defined no-Redirector outcome: set CF and
  `ERROR_INVALID_FUNCTION`, then advance IP.  `MS_bop_9` is reserved for its
  source-defined direct-access-error contract, not a blanket CLI Ignore.
- All guest byte/string reads are bounded synchronous uses of the session
  `guest_memory` mapping-manager instance.  Host DLL/export/callback identity
  cannot be stored in original 32-bit fields; a later admitted provider must
  use the session `host_resource` or `completion_callback` instance.
- No imported source body, selector table, Bochs source, or provider behavior
  changed in this packet.

## Interpretation and follow-up

The original BOP control plane is now a complete owner map, not a partially
implemented dispatcher.  S3 can recover DEM dispatch/data through its named
same-shaped SoftPC/guest-memory boundary; later source packages own the
remaining calls.  The static compilation proof establishes source composition
only—not a BOP route, a guest trace, or a runtime provider claim.
