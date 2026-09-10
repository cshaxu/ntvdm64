# m0-t198 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t198-closure-20260813.md

# M0 T198 Closure: Reprioritized BOP Runtime Evidence Package

T198 is closed by owner-directed reprioritization, not by product completion.
It retains the generic CPU5/bx-mantle mechanics, BOP ingress/composition work,
and bounded COMMAND endpoint evidence. It does not prove complete BOP closure,
complete COMMAND console/launch behavior, or a runnable NTVDM.

The final paired-endpoint observation stopped at `54:01`, so `54:04` has no
native integration closure. T199 replaces trace-led service sequencing with a
complete global BOP state audit and component-package implementation plan.


## m0-t198-s10-closure-20260813.md

# M0 T198 S10 Closure — Whole Boot-Namespace Provider Plane

S10 is complete. `bx-vdm` now owns one source-built i686 provider plane for
the complete selected boot-namespace family, rather than individual BOP
interceptors. Every selected COMMAND/DEM entry crosses common ingress and
provider selection, then produces a fixed typed result or a copied mantle
mechanical action.

The focused closure covers COMMAND `54:0C`/`54:0D`; DEM open `50:12`, normal
read `50:16`, seek `50:00`, close `50:02`, pathname first `50:09`, and pathname
next `50:0B`. Pathname first defers before an explicit DTA input. The DTA-backed
next case rejects a changed pending action ID without consuming its state, then
completes with the original copied action. No `adapter_runtime` input is linked
or named by the plane/test.

The detailed final evidence is [T198 S10 P4 witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s10-p4-path-next-witness-001.md);
earlier action and source-map evidence remains linked from Status. The closure
does not claim a running #UD composition: mechanical action execution remains
mantle-owned and is the subject of admitted S11 composition mapping.


## m0-t198-s100-closure-20260813.md

# M0 T198 S100 closure

S100 proves the actual DEMFASTREAD blocker is a shared handle ABI inversion:
OpenNT uses `AX` high word and `BP` low word, while the current contained
namespace lifecycle uses the reverse.  S101 is admitted to correct that
single shared codec and its focused regressions; no guest run was made.

- [S100 handle ABI map](../../artifacts/documentation-archive/20260910/etc/research/t198-s100-dem-handle-abi-map-001.md)


## m0-t198-s101-closure-20260813.md

# M0 T198 S101 closure

S101 corrects the contained DEM lifecycle's shared handle codec to original
OpenNT AX-high/BP-low order.  The current x64 `/MT` composition witness passes
across open, seek, normal read, fast read and close; FASTWRITE remains closed.

- [S101 handle ABI witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s101-dem-handle-abi-witness-001.md)


## m0-t198-s102-closure-20260813.md

# M0 T198 S102 closure

S102 made exactly one post-codec exact-input observation.  It remains bounded
at `50:42`; AX changed, but missing BP/flags prevent a sound provider-state
diagnosis.  S103 is limited to expanded default-off copied-state observation.

- [S102 observation](../../artifacts/documentation-archive/20260910/etc/research/t198-s102-post-handle-abi-native-observation-001.md)


## m0-t198-s103-closure-20260813.md

# M0 T198 S103 closure

S103 added only copied `EBP` and `EFLAGS` to the existing default-off DEM
`50:42` observation.  The current fixture and bridge source-compile under
MSVC x64 `/MT`; no native link or guest run was performed.  S104 owns the
single exact-input run needed to observe those fields.

- [S103 fixture record](../../artifacts/documentation-archive/20260910/etc/research/t198-s103-fastread-observation-fixture-001.md)


## m0-t198-s104-closure-20260813.md

# M0 T198 S104 closure

S104 made exactly one native execution, at r3.  The retained `50:42` event
has handle token `00004005h`, ZF set and CF clear, but still reaches the
declined marker and bounded fixture stop.  It is not a DOS-continuity result.
S105 now owns the necessary holistic OpenNT fast-I/O disposition map.

- [S104 native observation](../../artifacts/documentation-archive/20260910/etc/research/t198-s104-complete-fastread-native-observation-001.md)


## m0-t198-s105-closure-20260813.md

# M0 T198 S105 closure

S105 established a complete historical disposition for the DEM fast-I/O pair:
both `50:42` and `50:43` are `demNotYetImplemented` entries that preserve
registers and clear CF. The current `50:42` byte-copy route conflicts with
that source fact. S106 owns one bounded, family-level recovery; it is not a
per-callsite workaround.

- [S105 fast-I/O disposition map](../../artifacts/documentation-archive/20260910/etc/research/t198-s105-dem-fast-io-disposition-map-001.md)


## m0-t198-s106-closure-20260813.md

# M0 T198 S106 closure

S106 replaced the active source-derived `50:42` byte-copy route with one
bounded historical no-op plane for `50:42/43`. Both selectors now resume with
only CF clear; composition re-builds under MSVC x64 `/MT` without guest
execution. S107 owns the only next guest run.

- [S106 no-op-family witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s106-dem-fast-io-noop-family-001.md)


## m0-t198-s107-closure-20260813.md

# M0 T198 S107 closure

S107 made exactly one native run. It passed the historical fast-I/O pair and
advanced to the saved INT10 ROM vector at `C000:014A`; no DOS-continuity claim
is made. S108 owns the evidence-only machine-owner map.

- [S107 observation](../../artifacts/documentation-archive/20260910/etc/research/t198-s107-fastio-noop-native-observation-001.md)


## m0-t198-s108-closure-20260813.md

# M0 T198 S108 Closure: C000 Is a Regressed Stream-Initialization Outcome

S108 confirms that `C000:014A` is an empty machine ROM-vector target, but
does not admit a ROM/VGA response.  The existing S89/S90 native stream run
already avoids it through the original BOP `5F` state publication.  S107,
after S106's fast-I/O no-op, does not reach `5F` and exposes the former target.

The next active task is S109: recover the original `50:42` fast/slow caller
contract before any runtime correction.

Evidence: [S108 machine-owner map](../../artifacts/documentation-archive/20260910/etc/research/t198-s108-c000-int10-machine-owner-map-001.md).


## m0-t198-s109-closure-20260813.md

# M0 T198 S109 Closure: Fast-Read No-Op Reconciliation

S109 corrects S106's runtime interpretation.  The historical dispatcher
placeholder is real, but leaves AX unchanged; the x86 NTDOS caller consumes AX
as an actual fast-read byte count.  The selected CLI profile must therefore
use the existing source-derived read-side provider for `50:42`, while keeping
`50:43` unavailable.

S110 owns that bounded adapter-only repair without guest execution.

Evidence: [S109 reconciliation](../../artifacts/documentation-archive/20260910/etc/research/t198-s109-dem-fastread-contract-reconciliation-001.md).


## m0-t198-s11-closure-20260813.md

# M0 T198 S11 Closure — Synchronous Composition Map

S11 is complete. The current generic-UD event/outcome and generic mechanical
action ABI already express the entire synchronous provider transaction. No
Bochs core or mantle extension is required: `bx-vdm` may interpret the copied
event, call the mantle action executor with a copied record, and return the
opaque typed outcome.

The audit found two adapter-local obligations: an explicit initialized session
lifetime for the otherwise context-free alternative bridge, and propagation of
the existing transaction result after a write action. It rejected raw
callbacks, ambient state, a new CPU hook and service vocabulary in Bochs.

The field/call map and source evidence are retained in
[T198 S11 P1 map](../../artifacts/documentation-archive/20260910/etc/research/t198-s11-p1-synchronous-composition-map-001.md).
S12 is admitted to implement only that adapter composition and result fix.


## m0-t198-s110-closure-20260813.md

# M0 T198 S110 Closure: Restore Profile-Gated DEMFASTREAD

The regressing `50:42/43` no-op intercept is removed.  The existing
source-derived read-only `50:42` provider is restored through the common DEM
namespace plane, while `50:43` remains unavailable/pass-through.  The current
MSVC x64 `/MT` composition fixture builds, links and passes without guest
execution.

S111 owns exactly one native post-repair observation.

Evidence: [S110 provider restoration](../../artifacts/documentation-archive/20260910/etc/research/t198-s110-fastread-provider-restoration-001.md).


## m0-t198-s111-closure-20260813.md

# M0 T198 S111 Closure: Restored Fast-Read Reaches Stream Initialization

One locked source-built native run confirms that restoring the profile-gated
`50:42` provider re-enters the original BOP `5F` stream initialization path
and avoids the S107 `C000:014A` regression. The controlled fixture exit is not
a DOS completion result. S112 is source-only recorder/result reconciliation;
no retry is authorized.

Evidence: [S111 native observation](../../artifacts/documentation-archive/20260910/etc/research/t198-s111-fastread-restored-native-observation-001.md).


## m0-t198-s112-closure-20260813.md

# M0 T198 S112 Closure: `next=50:42` Is Declined Fallback Telemetry

S112 source-maps the S111 terminal marker. It is written only after the
composition declines and then returns a controlled STOP, so it must not be
reported as an accepted fast-read result. The independently observed BOP `5F`
remains valid evidence that the no-op regression no longer blocks stream
initialization.

S113 owns a bounded adapter-side post-commit observer; it authorizes no guest
execution.

Evidence: [S112 telemetry map](../../artifacts/documentation-archive/20260910/etc/research/t198-s112-fastread-telemetry-order-map-001.md).


## m0-t198-s113-closure-20260813.md

# M0 T198 S113 Closure: Post-Commit Fast-Read Observation

S113 adds a test-only post-commit marker that differentiates accepted typed
`50:42` resumes from the retained declined fallback telemetry. Fixture source
compilation and the existing x64 composition test pass without guest
execution. S114 owns one bounded native observation.

Evidence: [S113 observer](../../artifacts/documentation-archive/20260910/etc/research/t198-s113-fastread-postcommit-observer-001.md).


## m0-t198-s114-closure-20260813.md

# M0 T198 S114 Closure: Native DEMFASTREAD Commit Proven

One native run proves a real `50:42` provider commit: resume at `6355`,
AX=`000D`, CF clear, followed by BOP `5F` stream initialization. A distinct
fallback `50:42` remains; S115 will source-map it without another run.

Evidence: [S114 observation](../../artifacts/documentation-archive/20260910/etc/research/t198-s114-fastread-postcommit-native-observation-001.md).


## m0-t198-s115-closure-20260813.md

# M0 T198 S115 Closure: Fallback Is Not Yet a File Identity

S115 identifies the accepted 13-byte `50:42` as finite CONFIG.SYS. The
remaining fallback has only a first-event snapshot, insufficient to assign it
to a particular file or defect. S116 will add a bounded test-only attempt
ledger before any new run.

Evidence: [S115 source map](../../artifacts/documentation-archive/20260910/etc/research/t198-s115-fastread-fallback-source-map-001.md).


## m0-t198-s116-closure-20260813.md

# M0 T198 S116 Closure: Bounded Fast-Read Attempt Ledger

The fixed-capacity fixture ledger is source/test/compile closed and is linked
only by the declared native observation harness. It records no guest data or
production state. A future task may admit one run to attribute the remaining
fallback call.

Evidence: [S116 ledger](../../artifacts/documentation-archive/20260910/etc/research/t198-s116-fastread-attempt-ledger-001.md).


## m0-t198-s117-invalid-ledger-observation-20260813.md

# M0 T198 S117 Record: Ledger-Equipped Run Without Ledger Export

S117 rebuilt, linked and ran the declared harness once. The ledger object was
present, but the fixture omitted its output loop, so `run.log` contains only
the legacy first accepted/fallback summaries. The run is valid for build and
path continuity, but invalid for the task's per-attempt attribution purpose.
It is retained as evidence and is not retried under S117.

S118 owns the test-only export correction and non-guest verification.


## m0-t198-s119-closure-20260813.md

# M0 T198 S119 Closure: Exported Native Fast-Read Ledger

S119 performed its single admitted replacement run and exported all four
ordered fast-read attempts.  It confirms the finite CONFIG read, the shared
COMMAND stream token and one later declined lifecycle token without changing
provider semantics.  S120 now has the exact source-only question: map the
open/close lifecycle producing `4E534005` before any implementation action.

Evidence: [S119 native ledger](../../artifacts/documentation-archive/20260910/etc/research/t198-s119-fastread-ledger-native-observation-001.md).


## m0-t198-s12-closure-20260813.md

# M0 T198 S12 Closure — Adapter Session And x86 CRT Frontier

S12 completed the adapter-owned synchronous composition session. It binds an
immutable boot-namespace plane explicitly, translates generic copied events,
executes copied mantle actions, and returns generic typed outcomes. The
alternative core-facing bridge delegates only to that adapter session and
declines while unbound.

The attempted native CPU5 x86 link proved a hard tooling boundary: the complete
MinGW provider closure imports a separate CRT/helper and BCrypt universe, so
it cannot enter the MSVC/x86 Bochs executable. This is not a Bochs or BOP
semantic defect. The detailed frontier is retained in
[T198 S12 P3 evidence](../../artifacts/documentation-archive/20260910/etc/research/t198-s12-p3-native-cross-island-link-frontier-001.md).

The owner selected a new MSVC x64 `/MT` unified modern-runtime route. S13 first
proves the minimal machine lifecycle on that host architecture; it does not
reuse or extend the rejected x86/MinGW link route.


## m0-t198-s120-closure-20260813.md

# M0 T198 S120 Closure: Declined Token Is a Lifecycle Fact, Not a File Name

S120 establishes that `4E534005` represents accepted-close generation, not a
namespace-entry ordinal.  The S119 ledger lacks the exact `50:12/50:02` and
namespace-state data needed to select a repair.  S121 is limited to a
fixture-only lifecycle correlation record; it cannot change provider routing
or run the guest.

Evidence: [S120 lifecycle map](../../artifacts/documentation-archive/20260910/etc/research/t198-s120-fastread-declined-token-lifecycle-map-001.md).


## m0-t198-s121-closure-20260813.md

# M0 T198 S121 Closure: Fixture-Only DEM Lifecycle Ledger

S121 is source/test/compile closed.  It introduces no provider behavior and
does not run the guest.  A separately admitted S122 may make one exact-input
run to correlate `4E534005` with the namespace state.

Evidence: [S121 ledger](../../artifacts/documentation-archive/20260910/etc/research/t198-s121-dem-lifecycle-ledger-001.md).


## m0-t198-s123-closure-20260813.md

# M0 T198 S123 Closure: Zero-Byte Result-Only Transaction

S123 repairs the common composition seam, not a BOP-specific branch.  A
zero-byte DEM result now retains its typed `AX=0`/CF-clear resume and emits no
mantle WRITE.  The full adapter-only composition test passes in
`artifacts/build/t198-s123-zero-byte-composition-test-r1`; its fixture sets
the mantle action hook to reject actions, so this result proves the zero-byte
path did not manufacture one.  Existing nonzero write rejection remains
covered in the same fixture.

S124 may make one exact-input native lifecycle regression.

Evidence: [S123 transaction fix](../../artifacts/documentation-archive/20260910/etc/research/t198-s123-zero-byte-result-transaction-fix-001.md).


## m0-t198-s124-closure-20260813.md

# M0 T198 S124 Closure: AUTOEXEC EOF Reaches COMMAND Boundary

S124's single native run verifies `4E534005` as accepted zero-byte AUTOEXEC
EOF and reaches the next source-owned COMMAND `54:01` boundary.  DEM file
read/close lifecycle is closed for this bounded boot sequence.  T198 S125
owns a holistic COMMAND launch-plane audit, not incremental service repair.

Evidence: [S124 native observation](../../artifacts/documentation-archive/20260910/etc/research/t198-s124-zero-byte-eof-native-observation-001.md).


## m0-t198-s125-closure-20260813.md

# M0 T198 S125 Closure: COMMAND Provider Is Present but Detached

S125 proves that the reached `54:01` is halted only by the test observer after
the native composition declines it.  The required finite COMMAND provider
already exists, but it is absent from both the manifest and the composition
lifecycle.  S126 owns component-level integration, with no historical NT host
dispatcher link.

Evidence: [S125 COMMAND composition map](../../artifacts/documentation-archive/20260910/etc/research/t198-s125-command-launch-composition-map-001.md).


## m0-t198-s126-closure-20260813.md

# M0 T198 S126 Closure: Finite COMMAND Provider Assembled

S126 closes the detached finite COMMAND provider's composition/test/build
integration.  The next native observation may now verify actual `54:01`
delivery, without redesigning the service.

Evidence: [S126 COMMAND composition closure](../../artifacts/documentation-archive/20260910/etc/research/t198-s126-command-launch-composition-closure-001.md).


## m0-t198-s127-closure-20260813.md

# M0 T198 S127 Closure: Native COMMAND Launch Boundary

One locked native fixture now proves that the assembled `54:01` COMMAND launch
transaction resumes into real guest control flow.  Its next bounded boundary
is `54:0E` (`SVC_GETKBDLAYOUT`), not target completion.

Evidence: [S127 native observation](../../artifacts/documentation-archive/20260910/etc/research/t198-s127-command-launch-native-observation-001.md).


## m0-t198-s128-closure-20260813.md

# M0 T198 S128 Closure: COMMAND Keyboard-Layout Admission

`54:0E` is source-mapped to original `cmdGetKbdLayout`.  The original handler
cannot enter the non-invasive CLI composition because its positive path binds
console, registry, SystemRoot and VDM-pointer mechanisms.  The retained
finite `DX=0` provider is admitted for globally classified composition reuse,
with its omitted `cmdInitConsole` effect recorded as a CLI capability
substitution.

Evidence: [S128 keyboard-layout reuse map](../../artifacts/documentation-archive/20260910/etc/research/t198-s128-command-keyboard-layout-reuse-map-001.md).


## m0-t198-s129-closure-20260813.md

# M0 T198 S129 Closure: Composed Keyboard-Layout No-Install Provider

The existing finite `54:0E` source-derived provider now enters only through
global ingress, provider selection and COMMAND console-plane classification.
Its adapter-only closure proves the fixed CLI no-install result and decline
discipline; the following native observation may determine the next reached
guest boundary.

Evidence: [S129 keyboard-layout composition closure](../../artifacts/documentation-archive/20260910/etc/research/t198-s129-command-keyboard-layout-composition-closure-001.md).


## m0-t198-s13-closure-20260813.md

# M0 T198 S13 Closure — MSVC x64 Minimal Lifecycle

S13 establishes the one modern runtime foundation selected by the owner:
`bx-core` and `bx-mantle` now source-build as an MSVC x64 `/MT` process
closure, with the guest CPU independently retained at CPU5/Pentium-MMX.

The lifecycle witness proves the finite initialize/reset/cleanup route and
records the exact compilation, link and PE architecture evidence in
[T198 S13 witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s13-msvc-x64-minimal-lifecycle-witness-001.md).
It also resolves the prior x64 host-pointer truncation by projecting the
adopted configuration with `SIZEOF_INT_P=8`; no adopted Bochs source change
or new Bochs exception is involved.

S14 begins from this clean foundation with an MSVC x64 finite native HLT run.
It must keep the same source-only machine closure and may not attach `bx-vdm`,
OpenNT providers, MinGW objects, or product facilities.


## m0-t198-s130-closure-20260813.md

# M0 T198 S130 Closure: Native Post-Keyboard Boundary

One locked native observation proves the composed `54:0E` no-install response
resumes real guest control flow to `54:04` (`SVC_CMDGETCURDIR`).  No target or
console-completion claim follows.

Evidence: [S130 post-keyboard native observation](../../artifacts/documentation-archive/20260910/etc/research/t198-s130-post-keyboard-native-observation-001.md).


## m0-t198-s131-closure-20260813.md

# M0 T198 S131 Closure: COMMAND Current-Directory Admission

`54:04` is mapped to OpenNT `cmdGetCurrentDir`. Its retained finite provider
is admissible as a globally classified immutable-drive-snapshot root response;
the original host drive/environment composition remains excluded.

Evidence: [S131 current-directory reuse map](../../artifacts/documentation-archive/20260910/etc/research/t198-s131-command-current-directory-reuse-map-001.md).


## m0-t198-s132-closure-20260813.md

# M0 T198 S132 Closure: Composed Current-Directory Root Provider

The finite `54:04` root provider now enters through global classification and
the immutable drive snapshot. Its adapter-only closure proves both its checked
root write and its zero-write typed failure result; one later native
observation may determine the next real guest boundary.

Evidence: [S132 current-directory composition closure](../../artifacts/documentation-archive/20260910/etc/research/t198-s132-command-current-directory-composition-closure-001.md).


## m0-t198-s133-withdrawal-20260813.md

# M0 T198 S133 Withdrawal: Trace-Chaining Policy Correction

S133 was terminated by the owner before a result was accepted. Its planned
native observation is withdrawn and produces no evidence. T198 now advances
by complete original component packages; trace endpoints are integration and
ordering evidence only.

Replacement plan: [T198 component-package replan](../../artifacts/documentation-archive/20260910/etc/research/t198-component-package-replan-001.md).


## m0-t198-s14-closure-20260813.md

# M0 T198 S14 Closure — MSVC x64 Finite HLT

S14 repeats the existing neutral finite-run contract within the unified MSVC
x64 `/MT` core/mantle closure. A single guest `HLT` byte reaches the native
instruction loop and the existing controlled-stop/cleanup result without
adapter, provider, OpenNT, MinGW, GUI, device archive, or product input.

The bare real-mode fixture produces the original `HLT with IF=0` diagnostic;
the recorded zero exit confirms that it is a controlled observation rather
than a host failure. The exact evidence is retained in
[T198 S14 witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s14-msvc-x64-finite-hlt-witness-001.md).

S15 may now repeat the existing selector-blind generic `#UD` mechanical
bridge on the same x64 closure. It remains prohibited from recognizing BOP or
linking `bx-vdm` until that mechanical gate is independently proven.


## m0-t198-s15-closure-20260813.md

# M0 T198 S15 Closure — MSVC x64 Generic UD Mechanics

The strict r6 neutral-UD2 fixture proves the existing selector-blind generic
`#UD` bridge under the MSVC x64 CPU5 core/mantle closure. Earlier r1--r5
observations exposed fixture and cache-coherency defects; none is credited as
a BOP result. The r6 evidence is retained in
[the S15 observation](../../artifacts/documentation-archive/20260910/etc/research/t198-s15-msvc-x64-generic-ud2-observation-001.md).

The next step is the MSVC x64 source closure of `bx-vdm`; Bochs remains
selector-blind and BOP/provider semantics remain in `bx-vdm`.


## m0-t198-s16-closure-20260813.md

# M0 T198 S16 Closure — 2026-08-13

S16 is complete. The existing boot-namespace composition closes as an MSVC x64
`/MT` C island and its retained session fixture exits zero. The closure retains
one standard platform library, `bcrypt.lib`, solely for the existing CLI BYOB
startup-contract data model. It contains no native-machine, Bochs product,
MinGW, CLI-engine, or historical OpenNT runtime object.

The x64 admission exposed only unsafe wide-string copies. They were repaired
as bounded copies without changing the BOP table or provider semantics. Native
attachment is intentionally deferred to the next S task.

Primary evidence: [S16 witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s16-msvc-x64-boot-namespace-composition-witness-001.md).


## m0-t198-s17-closure-20260813.md

# M0 T198 S17 Closure — 2026-08-13

S17 is complete. A real BOP-form `C4 C4 54 0C` event has crossed the CPU5
native `#UD` mechanics, the source-built `bx-vdm` composition session and the
mantle checked ordinary-RAM action, then resumed to the fixture HLT under one
MSVC x64 `/MT` process. No selector is recognized outside `bx-vdm`.

The evidence is deliberately bounded: it proves one COMMAND boot-file
transaction and finite terminal behavior, not DOS continuity or general BOP
runtime. The all-in-one probe's report emission exceeded the outer execution
limit only after it created the r4 executable; direct run and PE inspection of
that exact artifact completed successfully.

Primary evidence: [S17 witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s17-msvc-x64-native-bop-composition-witness-001.md).


## m0-t198-s18-closure-20260813.md

# M0 T198 S18 Closure â€” 2026-08-13

S18 is complete.  Every endpoint implemented by the existing immutable
boot-namespace plane now has a native MSVC x64 `/MT` witness: COMMAND
`54:0C/0D`; the dependent DEM `50:12 -> 50:16 -> 50:00 -> 50:02` read-only
lifecycle; and the DTA/search sequence `50:1B -> 50:09 -> 50:0B`.

The last fixture executes genuine four-byte BOP forms under the CPU5 finite
machine.  It derives DTA state only through the existing checked guest-RAM
read action, writes/reads the guest DTA through the existing mechanical action
contract, and reaches `HLT` only after the typed resumes.  No selector is
recognized by Bochs and no host directory, DOS filesystem, device, or boot
continuity behavior has been introduced.

Primary evidence: [S18 native matrix](../../artifacts/documentation-archive/20260910/etc/research/t198-s18-boot-namespace-native-matrix-001.md)
and [S18 DTA/search witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s18-native-dta-search-witness-001.md).


## m0-t198-s19-closure-20260813.md

# M0 T198 S19 Closure â€” 2026-08-13

S19 is complete.  It proves that `50:11` is the first live NTDOS guest-entry
request after NTIO execution, with destination derived from current `DI:0000`.
The original `demLoadDos` source is authoritative for its ownership and
non-return failure semantics, but the historical `DemDispatch`/CCPU/SAS/Win32
composition is not an admissible modern runtime input.

The map isolates one source-derived provider boundary: identity-checked NTDOS
profile bytes, one preflighted ordinary-RAM publication action, typed
`fault+4` resume, and a controlled non-return failure.  It explicitly rejects
ambient host paths, incremental file reads, a replacement DEM dispatcher, and
a second legacy adapter session.

Primary evidence: [S19 guest-entry map](../../artifacts/documentation-archive/20260910/etc/research/t198-s19-first-ntdos-guest-entry-map-001.md).


## m0-t198-s20-closure-20260813.md

# M0 T198 S20 Closure — 2026-08-13

S20 is complete. DEM `50:11` is now part of the one active `bx-vdm`
composition. It publishes an identity-checked NTDOS image at `DI:0000`
through the selector-blind mantle RAM action, resumes at the four-byte BOP
successor, and controlled-stops invalid requests without guest mutation.

The CPU5 fixture reaches HLT only after far-jumping into bytes published by a
real `C4 C4 50 11` transaction. No old DEM/CCPU/Win32 runtime, second session,
Bochs change, BIOS response, or DOS startup behavior entered the closure.

Primary evidence: [S20 witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s20-native-dem-load-dos-witness-001.md).


## m0-t198-s21-closure-20260813.md

# M0 T198 S21 Closure — 2026-08-13

S21 closes the NTIO pre-entry contract.  Historical source locks the ordering
and byte ownership, but its CCPU/SAS/path/VDD composition is excluded.  The
next mechanism is therefore a generic mantle pre-entry publication: read a
preserved physical range after reset, preflight all writes, publish opaque
bytes, restore the saved range, and change only real-mode CS:IP.

NTIO addresses and identity stay in `bx-vdm`; the mantle receives no OpenNT,
BOP, DOS, BIOS or host-path vocabulary.  The first permitted continuation is
the existing `50:11` provider or a bounded stop before later selectors.

Primary evidence: [S21 pre-entry map](../../artifacts/documentation-archive/20260910/etc/research/t198-s21-ntio-preentry-contract-map-001.md).


## m0-t198-s22-closure-20260813.md

# M0 T198 S22 Closure — 2026-08-13

S22 closes the versioned, selector-blind pre-entry primitive in `bx-mantle`.
It preflights the fixed one-MiB ordinary-RAM aperture, captures an opaque
range after reset, publishes an embedded opaque image, restores that range,
and changes only supplied real-mode CS:IP before finite CPU execution.

The MSVC x64 `/MT` CPU5/Pentium-MMX witness proves both restoration-before-
entry and pre-mutation out-of-aperture rejection.  No VDM, BOP, OpenNT, DOS,
BIOS, host-path, callback or provider semantics were admitted to mantle.

Primary evidence: [S22 witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s22-mantle-preentry-witness-001.md).


## m0-t198-s23-closure-20260813.md

# M0 T198 S23 Closure — 2026-08-13

S23 closes the exact source-built NTIO pre-entry composition.  Under one
MSVC x64 `/MT` CPU5/Pentium-MMX process, it copies the locked 0x8400-byte
NTIO image, preserves the source-locked pre-entry range, enters `0070:0000`,
and reaches original DEM `50:11` for the existing NTDOS publication.

It further proves the next top-level BIOS `BOP 12h` is resumed with the
existing documented memory-size result and that the reusable provider's
separate `15h/AH=88h` form is not accidentally admitted.  The next actual
unhandled boundary is DEM `50:3B`.  No continuous DOS boot claim is made.

Primary evidence: [S23 P2](../../artifacts/documentation-archive/20260910/etc/research/t198-s23-p2-native-ntio-first-boundary-001.md),
[S23 P4](../../artifacts/documentation-archive/20260910/etc/research/t198-s23-p4-post-dem-bop12-map-001.md), and
[S23 P5](../../artifacts/documentation-archive/20260910/etc/research/t198-s23-p5-native-bios12-continuation-001.md).


## m0-t198-s24-closure-20260813.md

# M0 T198 S24 Closure — 2026-08-13

S24 moves original DEM `SVC_ISDEBUG` (`50:3B`) into the global DEM-MISC
provider plane.  The MSVC x64 `/MT` focused closure proves the source-derived
non-debug branch preserves AH, returns AL zero, and resumes at `RIP+4`.
The exact NTIO CPU5 run observes that typed resume after original `50:11`.

The post-resume HLT is intentional test-fixture behavior: S24's NTDOS input
is a mock whose first byte is HLT.  It is not a Bochs, firmware, memory or
OpenNT failure claim.  S25 therefore replaces that mock with the locked
source-built NTDOS image before further BOP discovery.

Primary evidence: [S24 P1](../../artifacts/documentation-archive/20260910/etc/research/t198-s24-p1-dem-isdebug-native-resume-001.md),
[P2 correction](../../artifacts/documentation-archive/20260910/etc/research/t198-s24-p2-native-hlt-source-triage-001.md),
and [P3 terminal snapshot](../../artifacts/documentation-archive/20260910/etc/research/t198-s24-p3-terminal-snapshot-001.md).


## m0-t198-s36-closure-20260813.md

# M0 T198 S36 Closure — Chained-Step Predecessor Witness

S36 is source-built and runtime-verified under the single MSVC x64 `/MT`
CPU5/Pentium-MMX fixture closure.  The registered default-off selector-blind
history mechanic records the first trace entry and every chained subsequent
instruction without exposing instruction objects or guest memory.

At the existing first non-BOP generic `#UD`, the copied history proves the
immediate predecessor is `0000:0A82`, followed by `0000:0A84` on consecutive
native instruction counts.  The full evidence and boundary limitation are in
[the S36 witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s36-chained-step-history-witness-001.md).

The follow-up is source-led low-RAM control-flow provenance, not a BOP,
adapter, device or firmware implementation change.


## m0-t198-s37-closure-20260813.md

# M0 T198 S37 Closure -- Low-RAM Transfer Attribution

S37 maps the current `50:0F` continuation against OpenNT `msinit`, DEM and SVC
source.  It rejects the stale theory that the current `0000:0A84` frontier is
an unhandled drive BOP, and rejects any device or firmware inference.

The exact first CS-to-zero transfer and the responsible write remain unproven;
the next bounded step is selector-blind position-only observation.  See
[the S37 attribution](../../artifacts/documentation-archive/20260910/etc/research/t198-s37-low-ram-transfer-attribution-001.md).


## m0-t198-s38-closure-20260813.md

# M0 T198 S38 Closure -- Code-Segment Transition Witness

S38 adds one default-off, mantle-owned copied query to the already admitted
instruction-position history.  The exact CPU5 x64 finite witness narrows the
frontier to a retained transition `9346:4D53 -> 0000:0000`, before the known
segment-zero `0A82 -> 0A84` sequence.

It does not identify the transfer instruction or add a machine, BOP or host
capability.  See [the S38 witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s38-cs-transition-witness-001.md).


## m0-t198-s39-closure-20260813.md

# M0 T198 S39 Closure -- NTIO Pre-entry Contract Attribution

S39 proves that the current finite fixture restores an undefined bare-RAM
four-byte virtual-interrupt span and omits the other documented NTIO pre-entry
profile inputs.  The S38 control-flow result is therefore startup-input
contaminated, not evidence for a missing Bochs device or BOP service.

The next package must install the complete fixed profile snapshot atomically
through adapter-to-mantle mechanics.  See
[the S39 attribution](../../artifacts/documentation-archive/20260910/etc/research/t198-s39-preentry-contract-attribution-001.md).


## m0-t198-s40-closure-20260813.md

# M0 T198 S40 Closure -- Atomic NTIO Pre-entry Inputs

S40 restores the selected profile's complete evidenced NTIO pre-entry inputs
through the existing opaque mantle mechanical-action write. The old `0A84`
all-`FF` frontier is gone; the new reproducible frontier is `0000:0047`,
after the still-unexplained `9346:4D53 -> 0000:0000` transfer.

No BOP, device or firmware behavior was added. See
[the S40 closure evidence](../../artifacts/documentation-archive/20260910/etc/research/t198-s40-preentry-input-closure-001.md).


## m0-t198-s41-closure-20260813.md

# M0 T198 S41 Closure

S41 captured copied SS:SP at the selector-blind CS transition. The exact
transfer frame remains unproven. See
[S41 evidence](../../artifacts/documentation-archive/20260910/etc/research/t198-s41-stack-position-witness-001.md).


## m0-t198-s42-closure-20260813.md

# M0 T198 S42 Closure

S42 closes the bounded provenance observation at the existing selector-blind
code-segment transition. The successful CPU5 x64 `/MT` native run identifies
the predecessor as `SS: CALL FAR [037A]` and retains its far return address.
It does not add any BOP, OpenNT, DOS, device, firmware, or adapter behavior.

S44 later corrects the initially over-strong reading of the next history
position: S42 did not directly capture the far-call operand. See S42's
correction note and the S44 witness.

The detailed record is [S42 control-transfer provenance
witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s42-control-transfer-provenance-001.md).


## m0-t198-s43-closure-20260813.md

# M0 T198 S43 Closure

S43 attributes the S42 far call to original NTDOS `DEVIOCALL2` and its
DOSDATA `CALLDEVAD` scratch slot. The target is upstream guest device-chain
state, not an omitted BOP or a Bochs facility. The remaining evidence gap is
the slot's raw terminal value and its immediately preceding source writer.

See [S43 DEVIOCALL2 pointer attribution](../../artifacts/documentation-archive/20260910/etc/research/t198-s43-devio-call-pointer-attribution-001.md).


## m0-t198-s44-closure-20260813.md

# M0 T198 S44 Closure

The existing terminal snapshot shows `CALLDEVAD = BF1F:FFFF`, correcting the
prior null-pointer inference. The active question is now real-mode execution
continuity at that source-derived target, not host/device construction.

See [S44 terminal witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s44-call-dev-ad-terminal-witness-001.md).


## m0-t198-s45-closure-20260813.md

# M0 T198 S45 Closure

S45 ties the original `BF1F:FFFF` strategy entry to the retained upstream
Bochs sequential-fetch path and its `EIP=0x10000` diagnostic. A neutral,
generic reproduction is required before any core repair.

See [S45 real-mode IP-wrap source audit](../../artifacts/documentation-archive/20260910/etc/research/t198-s45-realmode-ip-wrap-source-audit-001.md).


## m0-t198-s46-closure-20260813.md

# M0 T198 S46 Closure

S46 isolated the NTIO `CS:FFFF` concern into a neutral CPU fixture. The full MSVC x64 `/MT` CPU5/Pentium-MMX closure reproduces the exact upstream-retained prefetch diagnostic `EIP [00010000] > CS.limit [0000ffff]`; no OpenNT, BOP, adapter, provider, device or CLI input is present.

The finding admits a narrowly generic core repair, not an adapter workaround. See [S46 neutral real-mode IP-wrap witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s46-neutral-realmode-ip-wrap-witness-001.md).


## m0-t198-s47-closure-20260813.md

# M0 T198 S47 Closure

S47 replaces the adopted upstream full-width sequential advances with one
code-address-width helper. The neutral `CS:FFFF` fixture now crosses to
`CS:0000`, executes HLT at `0004`, and reports the normal post-HLT `0005`.
The HLT and generic UD2 STOP regressions also pass after early-stop lifecycle
cleanup.

See [S47 real-mode IP-width repair](../../artifacts/documentation-archive/20260910/etc/research/t198-s47-realmode-ip-width-repair-001.md).


## m0-t198-s78-closure-20260813.md

# M0 T198 S78 Closure: Minimum Reached ROM/IVT Provisioning Map

## Scope

S78 was admitted to classify the real-mode transfer from `073B:0939` to
`C000:014A` before expanding any BOP service/provider behavior.

## Result

The captured `window=15:ffffffff` is a 15-byte generic-UD window whose bytes
begin `FF FF FF FF`; it is not a `15h` BOP. The current minimal machine has
ROM backing initialized to `FF` but no BIOS, expansion-ROM or initial-IVT
image provisioning stage. The reached `C000:014A` byte therefore lies outside
the declared machine input closure.

Historical OpenNT confirms that ROM/IVT ownership was a product-composition
concern: `rom.c` loads named external resources and `reset.c` initializes the
IVT. The DOS-resident keyboard image supplies separate vectors but does not
source-map the reached C000 address. No approved matching ROM payload exists
in the checked source/artifact inputs.

## Architecture Disposition

Any future valid bytes require a mantle-owned opaque machine-image
provisioning boundary applied before CPU reset. It cannot put BIOS, DOS,
SoftPC, BOP or host-service interpretation into `bx-core` or `bx-vdm`.
Because the byte input is absent, S78 adds no code, descriptor, provider,
device, or synthetic ROM stub.

## Evidence And Verification

- [S78 closure map](../../artifacts/documentation-archive/20260910/etc/research/t198-s78-minimum-rom-ivt-closure-map-001.md)
- [S77 C000 provenance audit](../../artifacts/documentation-archive/20260910/etc/research/t198-s77-c000-rom-window-provenance-audit-001.md)
- Documentation inventory export and governance verification passed.
- `git diff --check` passed.

## Transfer

S79 is admitted to decide whether the existing approved checkout has a
reproducible, source-authorized ROM/IVT producer or input route. It may not
import or synthesize bytes.


## m0-t198-s79-closure-20260813.md

# M0 T198 S79 Closure: No Current Source-Authorized C000 Image Producer

## Result

Neither the approved OpenNT checkout nor the pinned Bochs 2.6 checkout yields
a reproducible, source-authorized C000 ROM input for the reached execution
path. OpenNT retains only resource names and loader code. Bochs can build its
F000 system BIOS with unavailable legacy tools, but has only prebuilt VGA BIOS
files for C000 and their source is absent from the pinned checkout.

The prebuilt Bochs VGA image also requires its excluded VGA/device lifecycle,
so it cannot be treated as an opaque one-file fix. No image, device, BOP
provider, or Bochs change was added.

## Evidence

- [S79 producer audit](../../artifacts/documentation-archive/20260910/etc/research/t198-s79-machine-image-producer-audit-001.md)
- [S78 closure map](../../artifacts/documentation-archive/20260910/etc/research/t198-s78-minimum-rom-ivt-closure-map-001.md)

## Transfer

S80 must map the original BOP `5F` display-mode initialization contract before
selecting any future machine/video direction.


## m0-t198-s8-closure-20260813.md

# M0 T198 S8 Closure — Generic Mechanical Action Boundary

## Result

S8 is complete.  The finite boot-namespace family now has one native,
selector-blind mechanical action ABI for checked copied reads and writes.  It
is owned by `bx-mantle`; `bx-vdm` is consequently able to translate into it
without a reverse mantle-to-VDM dependency.

## Evidence

- `BX-MANTLE-068` registers the only mantle addition before implementation.
- The CPU5/Pentium-MMX MSVC/x86 r2 fixture proves a valid read, a 65,535-byte
  write/read, and rejection of an out-of-aperture multi-range write without a
  partial first write.
- The i686 C layout fixture proves the fixed 66,592-byte record layout.
- The source scan rejects service and host-composition vocabulary, and the
  r2 link manifest excludes product startup, device archives, OpenNT and CLI.

The detailed command, hashes and limitations are in
[T198 S8 mechanical action boundary witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s8-mechanical-action-boundary-witness-001.md).

## Limit

No provider, BOP execution, legacy adapter runtime, host filesystem or CLI
capability is linked.  The old VDM pending descriptor remains local source
evidence, not the mantle ABI.

## Transfer

S9 must determine the exact whole-family linkage closure for the selected
finite boot namespace before enabling its first route.  It must preserve the
global ingress and provider-plane structure rather than attach individual
services to the new action consumer.


## m0-t198-s80-closure-20260813.md

# M0 T198 S80 Closure: Display-Mode Composition Explains C000 Path

## Result

The C000 transfer is strongly explained by original OpenNT display-mode
composition. NTIO's BOP `5F` provides its resident keyboard/video table to
the historical host, which writes a screen-state byte. The resident INT 10
stub chooses one of three original paths: fullscreen jumps to the saved native
INT 10 vector; windowed calls SoftPC video BOP `42`; stream-I/O handles the
reached teletype form in guest-resident code first.

The observed `AX=0E43h` matches that stream-I/O fast path. No exact listing
maps `073B:0939`, so the conclusion remains semantic rather than offset-level.
No profile state, BOP provider, ROM, VGA device or guest byte was changed.

## Evidence

- [S80 display-mode/vector chain audit](../../artifacts/documentation-archive/20260910/etc/research/t198-s80-display-mode-vector-chain-audit-001.md)
- [S79 image-producer audit](../../artifacts/documentation-archive/20260910/etc/research/t198-s79-machine-image-producer-audit-001.md)

## Transfer

S81 may add only a default-off adapter-side observation for the original BOP
`5F` table and its before/after resident state. It must not change the state
or attach a provider.


## m0-t198-s81-closure-20260813.md

# M0 T198 S81 Closure: BOP 5F Is Not Reached

## Result

The existing passive BOP `5F` observer is already part of the source-built
native fixture and reports zero in both retained complete S74/S76 runs. The
same runs later observe the C000 no-image stop. No BOP-`5F` provider, guest
write, mode selection or Bochs change is justified.

The fresh r7 attempt source-built the full CPU5 core but was terminated before
the adapter link/run stage; it is retained only as an incomplete attempt, not
as runtime acceptance evidence.

## Evidence

- [S81 passive observation](../../artifacts/documentation-archive/20260910/etc/research/t198-s81-bop5f-passive-observation-001.md)
- [S80 display-mode chain](../../artifacts/documentation-archive/20260910/etc/research/t198-s80-display-mode-vector-chain-audit-001.md)
- `artifacts/build/t198-s74-dem-pdb-termination-r1/run.log`
- `artifacts/build/t198-s76-bios-int15-invalid-r1/run.log`

## Transfer

S82 must map the source-built guest initialization/control-flow path from
entry to `073B:0939` and determine why the `SpcKbd`/BOP-`5F` seam is absent.


## m0-t198-s82-closure-20260813.md

# M0 T198 S82 Closure: C000 Transfer Is Patched SpcKbd INT10 State

## Result

The traced `073B:0939` instruction is part of the source-built resident
SpcKbd INT10 path. Its static image has a zero far-pointer operand, while the
runtime holds `C000:014A`; the pointer was dynamically patched before the
observed transfer. Historical source identifies the corresponding
`host_int10` mechanism and its IVT-10 capture path.

This eliminates a random-control-flow or generic Bochs interpretation. It
does not prove why the BOP-`5F` passive observer did not see the writer's
historical setup path.

## Evidence

- [S82 SpcKbd provenance audit](../../artifacts/documentation-archive/20260910/etc/research/t198-s82-spckbd-native-int10-provenance-001.md)
- [S81 passive BOP-5F observation](../../artifacts/documentation-archive/20260910/etc/research/t198-s81-bop5f-passive-observation-001.md)
- `artifacts/toolchain-runs/ntio-tools16-opennt-v1/.../ntio.map`

## Transfer

S83 must audit bridge binding/lifetime and observe the `host_int10` memory
delta with checked read-only mechanics before any provider or machine decision.


## m0-t198-s83-closure-20260813.md

# M0 T198 S83 Closure: SpcKbd Pointer Changes After First BOP Ingress

## Result

The bound adapter receives `50:11` while the exact SpcKbd `host_int10`
far-pointer operand is still `0000:0000`. The later native INT10 jump holds
`C000:014A`, copied from the fixture's IVT-10 input. Thus the pointer mutation
is a later source-owned guest lifecycle event, not a pre-bind adapter failure
or random Bochs control transfer.

## Evidence

- [S83 patch lifecycle record](../../artifacts/documentation-archive/20260910/etc/research/t198-s83-spckbd-patch-lifecycle-001.md)
- `artifacts/build/t198-s83-spckbd-lifecycle-probe-r6/run.log`
- `tools/probe/Invoke-T198S83SpcKbdLifecycleProbe.ps1`

## Transfer

S84 may inspect the absent BOP-`5F` observation versus the bridge's routing
order. No BOP provider, guest write, machine image, display mode or Bochs
semantic change transfers with this closure.


## m0-t198-s84-closure-20260813.md

# M0 T198 S84 Closure: BOP-`5F` Is Consumed Before Passive Recording

## Result

The missing passive BOP-`5F` marker is explained by test routing order. The
bound composition first recognizes the source-proven `AX=BEEF` service,
resumes it with CF set, and returns before the test bridge's later passive
recorder. The guest then follows original `jc isk_int9` and patches
`host_int10` from IVT 10h.

## Evidence

- [S84 observer-order audit](../../artifacts/documentation-archive/20260910/etc/research/t198-s84-bop5f-observer-order-001.md)
- [S83 lifecycle bracket](../../artifacts/documentation-archive/20260910/etc/research/t198-s83-spckbd-patch-lifecycle-001.md)

## Transfer

S85 must compare the narrow source-derived x86 continuation with the original
`MS_bop_F/kb_setup_vectors` host contract. No new `5F` provider, machine
image, display policy, ROM mapping or Bochs change is admitted.


## m0-t198-s85-closure-20260813.md

# M0 T198 S85 Closure: BOP-`5F` Continuation Does Not Rehost Its Host Contract

## Result

The current BOP-`5F` continuation correctly supplies only the original x86
carry branch. Original NTVDM also used the handoff to bind keyboard/timer/INT
tables, publish invasive display state into K.SYS and restart ICA interrupts.
Those effects belong respectively to a minimal Bochs machine closure and an
explicit CLI profile; they must not be added as adapter BOP semantics.

## Evidence

- [S85 host-contract delta map](../../artifacts/documentation-archive/20260910/etc/research/t198-s85-spckbd-host-contract-delta-001.md)
- [S84 BOP-`5F` observer-order audit](../../artifacts/documentation-archive/20260910/etc/research/t198-s84-bop5f-observer-order-001.md)

## Transfer

S86 may design the default non-invasive CLI display contract and its checked
K.SYS publication boundary. It must retain native/fullscreen and windowed
profiles as unavailable; it may not add Bochs devices, ROM images, PIC
behavior or an implicit profile choice.


## m0-t198-s86-closure-20260813.md

# M0 T198 S86 Closure: First CLI Profile Selects Stream State Explicitly

## Result

The first non-invasive CLI profile selects `STREAM_IO=2` explicitly. The
future adapter transaction derives K.SYS's state-byte offset from the
source-defined NTIO `kio_table`; it never accepts an arbitrary guest address.
This is a continuity state only, not a stdout/text-output implementation.

## Evidence

- [S86 CLI stream profile design](../../artifacts/documentation-archive/20260910/etc/research/t198-s86-cli-stream-profile-design-001.md)
- [S85 host-contract delta](../../artifacts/documentation-archive/20260910/etc/research/t198-s85-spckbd-host-contract-delta-001.md)

## Transfer

S87 may implement the fixed two-range checked read/write transaction for the
exact BOP-`5F` BEEF form and prove its rejection cases. Output, BOP `42`, VGA,
ROM, PIC and additional profiles remain outside that transfer.


## m0-t198-s87-closure-20260813.md

# M0 T198 S87 Closure: Checked SPCKBD Stream-State Publication

## Result

The exact real-mode `BOP 5F`/`AX=BEEF` stream-state capability is source-built
and tested in `bx-vdm`.  It reads only K.SYS `DS:SI+34`, validates the returned
same-segment target, then emits the existing typed ordinary-RAM write of
`STREAM_IO=2` and the original CF/`RIP+3` continuation.

## Evidence

- [S87 transaction record](../../artifacts/documentation-archive/20260910/etc/research/t198-s87-spckbd-stream-state-transaction-001.md)
- `artifacts/build/t198-s87-spckbd-stream-state-r1/run.log`
- `artifacts/build/t198-s87-spckbd-stream-state-r1/continuation-run.log`

## Transfer

No default was introduced and the live composition remains unchanged.  The
next separately admitted S must add a closed profile declaration and bind it
to this capability before claiming runtime reachability.  Output, BOP `42`,
VGA, ROM, PIC, keyboard/timer mechanics and any additional display profile
remain excluded.


## m0-t198-s88-closure-20260813.md

# M0 T198 S88 Closure: Immutable CLI Stream Profile Composition

## Result

The first stream profile is explicit and closed: v7 accepts only
`guest_display_state: "stream-io-v1"`, which becomes the fixed value `2` in
the adapter composition.  The selected composition executes the checked S87
BOP-`5F` state publication and rejects a failed selected transaction rather
than pretending it resumed.

## Evidence

- [S88 profile/composition record](../../artifacts/documentation-archive/20260910/etc/research/t198-s88-profile-stream-composition-001.md)
- `artifacts/build/t198-s88-profile-stream-r1/profile-run.log`
- `artifacts/build/t198-s88-profile-composition-r4/run.log`

## Transfer

S89 may rebuild the CPU5 native NTIO fixture with the current profile and
composition objects to observe the real guest BOP-`5F` path.  No stream output,
BOP `42`, ROM, VGA, PIC, keyboard/timer mechanics, host display API, or Bochs
change transfers with this closure.


## m0-t198-s9-closure-20260813.md

# M0 T198 S9 Closure — Whole Boot-Namespace Composition Map

S9 is complete.  It identifies one source-built i686 closure for the selected
finite boot namespace and rejects `adapter_runtime` as a composition input.
The map covers COMMAND boot paths, DEM normal readonly lifecycle and pathname
search as one facade rather than an endpoint collection.

The only discovered prerequisite is the existing DEM DTA registration input
for pathname search.  It is a real, explicit composition input and not a
reason to fabricate guest state.  The full route/object/action matrix, source
scan, i686 command result and warning limitation are retained in
[T198 S9 composition closure map](../../artifacts/documentation-archive/20260910/etc/research/t198-s9-boot-namespace-composition-closure-map-001.md).

T198 S10 is admitted to implement the provider-plane executor over this
closed family.  It must use common ingress and plane records and preserve the
explicit DTA input; it must not link the retained runtime.


## m0-t198-s90-closure-20260813.md

# M0 T198 S90 Closure: Correct CS:SI SPCKBD Table Addressing

The native stream-state handoff is now source-derived correctly: read the
kio-table word from `CS:SI+34`, write value `2` at `DS:returned-offset`, and
retain CF/RIP continuation.  The current CPU5 native run bypasses the former
C000 transfer and reaches `BOP 50:3D`; no display output is claimed.

Evidence: [S89/S90 native handoff record](../../artifacts/documentation-archive/20260910/etc/research/t198-s89-s90-native-spckbd-cs-si-closure-001.md).


## m0-t198-s91-closure-20260813.md

# M0 T198 S91 closure — 2026-08-13

T198 S91 source-classifies the newly reached DEM `50:3D` boundary as original
`SVC_DEMEXITVDM` / `demExitVDM`.  It is an error path that displays a historical
`config.nt` diagnostic and terminates the VDM; it has no ordinary guest resume
or normal DOS-result semantics.  The current adapter catalogue keeps it in the
DEM miscellaneous component with an explicit deferred disposition, and its
miscellaneous dispatcher deliberately does not attach a provider.

No guest memory read, provider, controlled-stop alias, CLI result, Bochs/mantle
change, or additional native execution was added.  The S90 native observation
therefore remains a stream-handoff reachability witness ending immediately
before the fatal path, not a successful startup or normal COMMAND return.

T198 S92 next maps the presently available source-built SHARE normal-return
candidate and its startup prerequisites before any new observation or terminal
result transport is admitted.  Detailed evidence is in
`../etc/research/t198-s91-dem-exit-vdm-classification-001.md`.


## m0-t198-s92-closure-20260813.md

# M0 T198 S92 closure — 2026-08-13

T198 S92 re-establishes the normal-return candidate as a present,
hash-verified source-built SHARE.EXE artifact, not a missing or fabricated
fixture. Its original expected route is `INT 21h/AH=4Ch -> 50:3C -> 54:11`,
but no current trace reaches that route.

The active native S89/S90 fixture cannot answer the question because it embeds
only exact NTIO/NTDOS and uses synthetic two-byte COMMAND and one-byte target
payloads. Repeating it would not test the source-built SHARE profile. S93
therefore owns only replacement of those fixture inputs with generated,
identity-locked source-built arrays and an explicit observation plan. It does
not yet run the guest, add a provider, change Bochs, or add terminal-result
transport.

Detailed evidence is in
`../etc/research/t198-s92-normal-return-candidate-prerequisite-map-001.md`.


## m0-t198-s93-closure-20260813.md

# M0 T198 S93 closure — 2026-08-13

T198 S93 closes the native fixture-input fidelity blocker. The fixture now has
a compile-only conditional branch for hash-verified source-built COMMAND.COM
and SHARE.EXE arrays; the ordinary legacy branch remains intact for its
historical witnesses. A real v7 profile declaration, rather than a hidden
default, selects the already-admitted stream state and TARGET.EXE placement.

The r3 preparation evidence compiles the profile selector, the source-built
fixture branch, and both generated arrays under MSVC x64 `/MT`, without a full
link or guest execution. No BOP/provider, Bochs/OpenNT behavior, terminal ABI,
or host capability changed. T198 S94 now owns at most one separately bounded
current-object link/run observation.

Detailed evidence is in
`../etc/research/t198-s93-source-built-normal-return-fixture-preparation-001.md`.


## m0-t198-s94-closure-20260813.md

# M0 T198 S94 closure — 2026-08-13

The one exact-input native observation completes with `54:02` at `0BB8:1B10`.
The bridge stopped this composition-unhandled BOP deliberately; it is not a
guest terminal result. Original source identifies COMMAND `cmdComSpec`, and
the existing bounded bootstrap pair remains detached from composition. No
second run or provider change occurred. T198 S95 owns whole-component mapping.


## m0-t198-s95-closure-20260813.md

# M0 T198 S95 closure

## Result

S95 closes the source-only map of the complete existing COMMAND bootstrap
component: `BOP 54:02` (`SVC_CMDCOMSPEC`) and `BOP 54:0F`
(`SVC_GETINITENVIRONMENT`).

The original OpenNT dispatcher, guest callers and host owners prove one
first-call component with shared COMSPEC/environment state.  The current
project contains a bounded source-derived subset, but the current
`bx-vdm` composition manifest does not compile or route it; the detached
historical runtime must not be restored as the runtime root.

The map also identifies two repeat-state mismatches in the retained subset and
records the only justified S96 seam: one classified, contained command
bootstrap component in `bx-vdm`, using existing checked mechanical read/write
actions.  It requires no Bochs or mantle change and makes no guest-running
claim.

## Evidence

- [COMMAND bootstrap component map](../../artifacts/documentation-archive/20260910/etc/research/t198-s95-command-bootstrap-component-map-001.md)
- [S94 source-built normal-return observation](../../artifacts/documentation-archive/20260910/etc/research/t198-s94-source-built-normal-return-observation-001.md)

## Deferred

S96 must implement and source-build/test the mapped component before a new
native source-built guest observation is admitted.  Ambient host environment,
filesystem policy and all other COMMAND services remain out of scope.


## m0-t198-s96-closure-20260813.md

# M0 T198 S96 closure

## Result

S96 repairs and composes the complete contained COMMAND bootstrap pair.
`54:02` and `54:0F` now share explicit first-profile state in the current
`bx-vdm` composition, are guarded by global COMMAND classification, and use
only the existing fixed checked read/write mechanics.

The x64 `/MT` current-composition and focused service witnesses pass.  The
change has no Bochs/core/mantle modification, no ambient host capability, no
guest run and no broader COMMAND admission.

## Evidence

- [S96 composition witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s96-command-bootstrap-composition-witness-001.md)
- [S95 component map](../../artifacts/documentation-archive/20260910/etc/research/t198-s95-command-bootstrap-component-map-001.md)

## Deferred

S97 may run one newly rebuilt exact-input source-built guest fixture and must
classify its first unresolved boundary.  It must not infer process completion
or normal DOS return merely from a bounded stop.


## m0-t198-s97-closure-20260813.md

# M0 T198 S97 closure

## Result

S97 made exactly one rebuilt source-built native observation after S96.  The
first unresolved boundary is DEM `50:42`, not COMMAND `54:02`; no normal
return or process-exit meaning is claimed.

## Evidence

- [S97 native observation](../../artifacts/documentation-archive/20260910/etc/research/t198-s97-post-bootstrap-native-observation-001.md)
- retained one-run artifact: `artifacts/build/t198-s97-post-bootstrap-native-r1`

## Deferred

S98 must map the full original/current DEM `50:42` contract without a guest
run or new provider.  Only that evidence can justify a targeted composition
repair or explicit unavailable disposition.


## m0-t198-s98-closure-20260813.md

# M0 T198 S98 closure

S98 proves that S97's `50:42` stop is caused by a current DEM-plane component
classification regression: the compiled fast-read provider is gated on
`NAMESPACE`, while service 42 is currently classified `RAW_MEDIA`.

The next S99 may restore only the prior evidence-backed `42h` namespace
classification and test reachability/negative `43h` behavior without running
the guest.

- [S98 component map](../../artifacts/documentation-archive/20260910/etc/research/t198-s98-dem-fastread-component-map-001.md)


## m0-t198-s99-closure-20260813.md

# M0 T198 S99 closure

S99 corrects the S98 hexadecimal-literal misread.  `66u` is `0x42`, so the
current DEM plane already routes FASTREAD through the namespace component.
The source-built composition witness proves `50:42` reaches the existing
checked provider with valid namespace state, while `50:43` remains unopened.

- [S99 correction and witness](../../artifacts/documentation-archive/20260910/etc/research/t198-s99-fastread-state-correction-001.md)

S100 must map the S97 call's actual handle/token lifecycle; no guest run or
provider change is admitted yet.

