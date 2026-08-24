# M0 T261 S8 — Final OpenNT-host exception-closure audit

## Decision

S8 is closed for its admitted exception ledger.  This is a source-layout and
reached-provider closure, not a claim that the historical VDMREDIR/VDD/ICA
product shell is runnable.

## Requirement-by-requirement result

1. **Drive policy and private namespace:** P1 deleted the include/exclude
   drive feature and its private namespace.  The original DEM path is the
   provider; no virtual boot volume or drive-policy fallback replaced it.
   Evidence: `m0-t261-s8-p1-host-namespace-drive-policy-removal-001.md`.
2. **XMS host mechanics:** P2 moved the reached original `xms.486` package to
   `opennt-bop/xms` and the selector-blind CCPU/SAS/A20/checked-RAM seam to
   `adapter-softpc`.  `src/opennt-host/xms/xms_shim.{c,h}` is absent.  The
   compatibility header's stale `xms_shim` include-guard spelling was also
   retired; it is now `NTDOS64_OPENNT_BOP_XMS_COMPAT_H`.
3. **Top-level direct-access error:** P3 re-rooted original
   `softpc.new/host/src/nt_error.c`; its three unavoidable product-shell
   deviations are exactly `HOST-DIV-012` through `014`.
4. **VDMREDIR ownership:** P4/P5 placed selector/service and guest-register
   mail-slot bodies under `opennt-bop/vdmredir`, while host-only named-pipe
   helpers live under `opennt-host/vdmredir`.  The former
   `redir_session_shim` has no production use.
5. **Mailslot bodies:** P5 recovers the reached original dispatcher/list and
   `VrPeek/Get/Delete/Make/Read/WriteMailslot` bodies.  Its bounded guest
   copy and session mapping substitutions remain BOP-owned, statement-level
   divergences; they are not hidden host replacements.
6. **Named-pipe bodies:** P6 recovers original classification, conversion,
   record/list, synchronous read, and synchronous write control flow.  The
   remaining registered differences are only VDD cancellation list absence,
   session-owned timeout retirement, static-DLL naming, x64 compiler/path
   safety, and static session teardown (`HOST-DIV-015` through `023`).
7. **Known non-goal boundary:** `VrInitialize`/`VrUninitialize` continue as a
   registered static single-session gate (`HOST-DIV-022`).  The original
   implementation needs VDD user hooks, NetBIOS/DLC, ICA interrupt queues and
   guest load-info publication.  It is intentionally not relabeled as source
   recovery and belongs to a later whole Redirector/VDD lifecycle package.

## Negative and provenance checks

- Production scans find no `host_namespace`, `redir_session_shim`,
  `xms_shim`, `opennt_dem_drive_policy_facade`, or
  `top_level_nosupport_shim` source path.  The only prior `xms_shim` token was
  the now-retired local include guard, not a live shim.
- `src/opennt-host` contains no `*_shim.{c,h}` file.
- Each source `DIVERGENCE(HOST-DIV-012..023)` occurs once and each ID has one
  matching `src/opennt-host/README.md` row.
- The re-rooted `nt_pif.c`, `nt_pif.h`, and `nt_uis.h` hashes match their
  `refs/opennt/base/mvdm/softpc.new/host/...` originals exactly.

## Verification

- Formal MSVC x64 `/MT` Ninja graph at `build/t261/s8-r012`: passed.
- `t251-s3-redir-ingress-fixture.exe`: passed after recompiling the recovered
  `vrnmpipe.c`; its local pipe round trip covers `VrWriteNamedPipe` then
  `VrReadNamedPipe`.
- Documentation inventory export, documentation-governance verification,
  T260 S8 component-manifest gate, T260 S8 production-tree gate, and
  `git diff --check`: passed.

## Consequence

S9 may now clean selector-aware observation and fixture-only residue from the
adapter production roots without carrying an open `opennt-host` shim-recovery
debt.  It must retain the explicitly registered VDMREDIR/VDD lifecycle
boundary rather than converting it into a local host reimplementation.
