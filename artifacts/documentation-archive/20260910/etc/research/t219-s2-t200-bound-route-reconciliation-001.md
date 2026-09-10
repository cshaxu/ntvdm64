# T219 S2: T200 actual bound-route reconciliation

## Question

Does every endpoint in the 203-entry T200 inventory still have the route and
disposition attributed to it at T200 closure?  In particular, which routes
were added by subsequent shims, which fallbacks are now stale, and which
host-backed paths should be owned by the common mutation/profile ABI?

## Inputs and method

- The canonical identity input is
  [T219's regenerated inventory](t219-s2-t200-canonical-endpoint-inventory-001.json),
  exported from the retained T200 source-inventory exporter.  It contains 203
  identities: 73 DEM, 1 WOW, 12 XMS, 25 DPMI, 17 COMMAND, 16 debugger, 50
  redirector, and 9 top-level/machine entries.
- The complete per-entry result is the machine-readable
  [bound-route ledger](t219-s2-t200-bound-route-reconciliation-001.json).
  Its exporter checks that it emits exactly 203 distinct selector/service/
  handler entries.  A row states the current composition route, its relation
  to T200, later shim, terminal or stale fallback, and profile-ABI action.
- Read-only source review traced the executable ingress order in
  `src/bx-vdm/bx_ntvdm_vdm_generic_ud_bridge_v1.c`,
  `bx_ntvdm_boot_namespace_composition_v1.c`, and
  `bx_ntvdm_native_bop_composition_v1.c`; package classification and final
  decisions in the DEM/COMMAND facades and sessions; plus XMS, DPMI,
  redirector, debugger, top-level and machine facades.
- `git diff 57108ff..HEAD -- src/bx-vdm` was used to identify route-changing
  work after the T200 closure commit.  It found the later DEM whole-provider,
  mutation/profile, COMMAND session/environment/lifecycle, startup-overlay
  and XMS-session additions.  This establishes source presence, not runtime
  success.

## Current binding, by complete family

| Family | Entries | Actual executable bound route | Reconciled conclusion |
| --- | ---: | --- | --- |
| DEM `50:00..48` | 73 | generic UD bridge → boot composition → `dem_package_session`; the session orders special planes, the installed direct whole provider, retained boot/readonly paths and finally CLI-unavailable | T200's “mixed fixture/profile composition” is stale for the direct whole provider.  The direct file/namespace/FCB/search partitions are now real source routes, but not full original DEM closure. |
| WOW `51` | 1 | boot composition top-level terminal facade | Still deferred; no WOW provider was introduced. |
| XMS `52:00..0B` | 12 | native composition → `xms_package_session` → mantle A20/extended-memory capability for `00..05`, `0A`, `0B`; stop for `06..09` | T200's selected-range description is stale: move, page-size and free-query are now bound.  This is still a partial mechanical seam, not XMS-family closure. |
| DPMI `53:00..18` | 25 | native composition → `dpmi_package_session` → typed stop | Confirmed explicitly unavailable for the CLI profile. |
| COMMAND `54:00..10` | 17 | boot composition → `command_package_session` → package providers for bootstrap, environment, CWD, launch, console and lifecycle | T200's endpoint-local description is obsolete.  The package session now owns every callable slot, but many results remain deferred/limited; this is not COMMAND package closure. |
| debugger `56` | 16 | boot composition → debugger facade → typed stop | Confirmed deferred non-debug profile. |
| redirector `57:00..31` | 50 | boot composition → redirector facade → `CF=1, AX=1, RIP+4` | Confirmed reachable unavailable family; it is not a network implementation. |
| top-level/machine | 9 | boot composition top-level/machine facades, with selector-specific terminal/idle/config or mechanical handoff | Confirmed selector-specific only; there is no general top-level provider. |

## Post-T200 shim and fallback findings

1. **Later DEM direct-host seams:** T202 added the `dem_whole_provider`,
   handle/namespace/FCB/search partitions and file session.  These replace
   T200's assumption that the normal executable path reaches only boot or
   readonly fixture leaves.  A failed installed partition still falls through
   to `dem_cli_unavailable_provider`; that fallback is source-visible but too
   broad to count as an original per-service failure recovery.
2. **Later startup-image overlay:** declared CONFIG/AUTOEXEC/COMMAND image
   routing is now an overlay inside the direct DEM whole provider.  The old
   boot namespace remains useful for declared-image/fixture paths but must not
   be called the default host-file provider.
3. **Later COMMAND session shims:** the package session, boot input,
   COMSPEC/initial-environment seam, launch/lifecycle and console capability
   seams supersede the T200 endpoint-local picture.  They retain package
   ownership, but do not establish a full historical COMMAND composition.
4. **Later XMS mechanical expansion:** XMS `52:01`, `04`, and `0A` are now
   bound in addition to the subset stated at T200.  UMB and INT15-dependent
   services still stop rather than silently succeeding.
5. **Stale lifecycle fallback:** DEM `50:3C` still terminates only finite
   boot-namespace state.  It has no copied PDB identity for direct file/search
   resources.  It must not be represented as a direct-host child cleanup
   route; it is the outstanding lifecycle gap discovered by T219 S1.
6. **Shared profile ABI:** direct DEM file/search/FCB/namespace routes and
   COMMAND initial environment/COMSPEC paths still carry package-local policy
   decisions.  The ledger marks the affected rows `migrate-to-shared-profile-abi`.
   DEM CWD/drive and COMMAND current-directory already consume the session
   host-context provider, although residual snapshot/synthetic fallbacks must
   be removed or made explicitly virtual/fixture-only.

## `$Exec` correction and next package

T218 remains correct at its architectural level: `$Exec` is guest DOS work,
not a Bochs or adapter loader.  Its next host boundary was, however, too
coarsely described as a new copied-PDB resource ABI.  Original `demfile.c`
uses `pusCurrentPDB` plus `Sim32GetVDMPointer` to find the guest PDB/JFT/SFT;
`50:36`'s `DX` only observes DOS-app entry and cannot establish ownership of
resources acquired by the parent beforehand.  Therefore the next package
must first recover the complete **DEM process-resource ownership contract**
(including the smallest permissible guest-PDB access seam) before it can
implement `50:3C` cleanup or any COMMAND parent-return claim.

This is a successor to the closed T218 objective, not a reopening of T218.
It remains a whole DEM owner package and must be admitted under a new later
T after T219 closes.  No route in this audit authorizes a trace-selected BOP
patch or native trace.

## Confidence and limitations

High confidence for source binding: the ledger is generated from the frozen
203-entry identity set and the current composition ordering.  Medium confidence
for runtime reachability: no native execution was performed, and a conditional
provider may decline based on its documented preconditions.  The ledger calls
those conditions out instead of treating a catalogued identity or a typed
result as proof of successful historical behavior.

## Follow-up

Close T219 S2 after governance checks, then admit a new whole-package task for
the corrected DEM process-resource contract.  It must source-map
`demfile.c`/`demmisc.c` PDB/JFT/SFT access, classify the smallest bridge under
the four-rung recovery rule, and decide profile-owner migration before changing
any runtime path.
