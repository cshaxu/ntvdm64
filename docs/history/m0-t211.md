# m0-t211 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t211-closure-20260815.md

# M0 T211 Closure — OpenNT COMMAND owner-package recovery

T211 closes the COMMAND BOP owner package by original component boundary.
It does not claim that an interactive console, full host-child execution, the
historical NT broker, or normal CLI completion are available.

- S1 mapped all 17 callable COMMAND slots and the dispatcher sentinel.
- S2 closed bootstrap/environment/config (`01/02/04/05/0C/0D/0F/10`), with
  `10` explicitly deferred for the missing historical session broker.
- S3 closed launch/execution (`06/07/08/0A`) under its admitted bounded
  capability disposition.
- S4 closed console/keyboard (`09/0E`) using the OpenNT no-KB16 path.
- S5 closed lifecycle (`00/03/0B`) and retained `54:0B` as a package-local,
  fixed-width terminal record rather than an engine or CLI result.
- A fresh full-family ownership regression iterates `54:00..10`, verifies
  exactly one component owner per member and preserves the `54:10` deferred
  boundary.

Evidence: [S1 map](../../artifacts/documentation-archive/20260910/etc/research/t211-s1-command-owner-package-source-abi-disposition-map-001.md),
[S4 closure](../../artifacts/documentation-archive/20260910/strict-topology/history/raw/m0-t211-s4-closure-20260815.md), and [S5 map and family
regression](../../artifacts/documentation-archive/20260910/etc/research/t211-s5-command-lifecycle-source-abi-map-001.md).

The next task, T212, owns modern CLI lifecycle governance.  It may define
external cancellation, budgets, audit/result presentation and cleanup around
the existing engine only after mapping its ABI.  It must not reinterpret a
COMMAND terminal record as normal process completion without new evidence.


## m0-t211-s2-closure-20260815.md

# M0 T211 S2 Closure — COMMAND bootstrap/environment/config component

S2 closes the first complete component of the T211 COMMAND owner package.
It does not close COMMAND as a whole and does not turn a reached trace
selector into the next task.

## Delivered boundary

`bx_ntvdm_command_bootstrap_provider_v1` owns the common copied state and
the component membership of original `cmddisp.c` services:

`54:01`, `02`, `04`, `05`, `0C`, `0D`, `0F`, and `10`.

The COMMAND session now performs only fixed-width guest read/write mechanics
around that provider.  It no longer owns separate bootstrap, registration,
get-next or boot-input state.  The COMMAND plane gives all eight one
`BOOTSTRAP` component identity.

## Result dispositions

- `01/02/04/05/0C/0D/0F` retain their admitted source-derived or shared
  T209/T210 provider paths.
- `10 cmdGetStartInfo` remains a package-owned explicit deferred stop.  Its
  original `DosSessionId` source is a historical session/console broker with
  no approved CLI ABI; no `AL` value is invented.
- `54:10` cannot enter the host-child launch path, which is restricted to
  `54:08`.

## Evidence

- r7 (`boot-namespace`, MSVC x64 `/MT`) passes the integrated positive
  routes and verifies `54:10` is a no-delta controlled stop in both the
  COMMAND family sweep and its direct route.
- r8 (`command-bootstrap`, MSVC x64 `/MT`) passes exact eight-member
  classification and rejects `54:11`, malformed COMSPEC gather, SetInfo
  locator overflow, boot-path overflow, and missing `54:04` session context
  without a guest write.
- Documentation governance and `git diff --check` pass.

The supporting P1/P2/P3 record is
`etc/research/t211-s2-command-bootstrap-provider-p1-001.md`.

## Next component

T211 S3 owns the whole original COMMAND launch/execution component
(`54:06/07/08/0A`): source/ABI reconciliation, direct-host capability
disposition, source-derived limits and one family regression.  It may not
invent a replacement for the historical VDM broker or promote an endpoint
trace to a new task.


## m0-t211-s3-closure-20260815.md

# M0 T211 S3 Closure — COMMAND launch/execution component

S3 closes the complete T211 launch/execution owner component: original
COMMAND services `54:06`, `07`, `08`, and `0A`.

## Delivered boundary

`bx_ntvdm_command_launch_execution_provider_v1` owns the private stream and
execution-lifecycle state.  It also resolves its own shared `HOST_GLOBAL`
direct capability.  The COMMAND session only supplies copied/machine
transactions and may not select direct versus non-direct launch behavior.

## Dispositions

- `06`: opaque stream tokens only; non-direct returns CF with zero token
  halves.  Raw host handles are never guest-visible.
- `07`: retained declared DOS-domain branch; no unproven WOW/PIF rewrite.
- `08`: bounded direct terminal child only, with private duplicated handles
  and no child-to-DOS re-entry queue.
- `0A`: explicit unsupported result; ambient COMSPEC/environment recapture
  and historical broker semantics remain deferred.

## Evidence

- Source/ABI/disposition map:
  `etc/research/t211-s3-command-launch-execution-source-abi-map-001.md`.
- r2 component fixture covers all four identities and non-direct/unsupported
  result boundaries under MSVC x64 `/MT`.
- r3 and r4 whole bx-vdm composition fixtures pass under MSVC x64 `/MT`,
  including provider-owned profile enforcement.

S3 does not claim BaseSrv/CSR, Redirector redirection completion, WOW/PIF,
interactive console, or a host-child-to-DOS command queue.

T211 S4 next recovers the complete COMMAND console/keyboard component
(`54:09/0E`) by source disposition and family regression.


## m0-t211-s4-closure-20260815.md

# M0 T211 S4 Closure — COMMAND console/keyboard component

T211 S4 closes the complete original COMMAND console/keyboard component,
`54:09` and `54:0E`; it does not claim interactive console or keyboard-layout
support.

- One versioned `bx-vdm` provider now owns both members through the COMMAND
  package session.
- `54:09` records the bounded no-VDD console disposition; `54:0E` follows
  OpenNT's no-KB16 path (`DX=0` followed by console initialization).
- The detached legacy adapter-runtime keyboard dispatch was removed, so no
  second route can select keyboard behavior outside the COMMAND component.
- Component and generic-bridge composition fixtures source-built and ran with
  MSVC x64 `/MT`; they cover both members, exact `DX=0`, malformed/protected
  context refusal and `54:10` non-membership.

Evidence: [source/ABI map](../../artifacts/documentation-archive/20260910/etc/research/t211-s4-command-console-keyboard-source-abi-map-001.md),
build records `artifacts/build/t211-s4-console-keyboard-r1` and `r3`, and
delivery commits `2d191f1b` / `e9a0d767`.

The next active packet, T211 S5, recovers the separate COMMAND lifecycle
component (`54:00/03/0B`) by its original owner boundary.

