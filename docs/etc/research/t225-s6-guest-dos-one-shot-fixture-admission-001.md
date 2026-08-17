# T225 S6 Guest DOS One-Shot Fixture Admission

## P0 Admission

T225 S4 proves that original guest DOS `$Exec` owns COM image classification,
MCB allocation, PSP construction, image copy, entry registers and parent
return. T225 S5 establishes that current native machine mechanics already
reach typed DEM BOP resume; it therefore rejects a new Bochs repair.

S6 admits the next owner-scoped prerequisite: a reproducible guest artifact
and entry fixture that promotes source-built NTDOS and COMMAND from
identity-only validation inputs to declared guest execution inputs. This is
not an adapter loader. The fixture must let the original NTDOS initialization
and guest `$Exec` code perform all process work.

## Scope

- Establish the exact source-built identities, placement rules and entry order
  for NTIO, NTDOS, COMMAND and one bounded COM child artifact.
- Map each required host BOP to its already-existing owner package/disposition
  in the T199/T221 global ledgers; no individual trace hit creates a service.
- Define one Direct and one Readonly fixture configuration using the shared
  profile ABI. Overlay stays interface-compatible but deferred; Virtual is retained only as retired implementation and is not a product profile.
- Implement only missing fixture/staging composition after the map proves its
  owner and lifecycle. No guest process semantics may cross into bx-vdm.

## Exclusions

No MZ executable, parent-return implementation, virtual boot-volume behavior,
new Bochs device, raw host child process, ambient host environment import, or
trace-driven BOP leaf patch is admitted. A native run comes only after the
whole fixture and its required owner-package regressions are closed.

## Exit Gate

A source/artifact/entry/BOP dependency ledger must name every fixture input and
its disposition. It must prove that Direct and Readonly have no Overlay or
Virtual fallback and that a missing BOP retains its package-defined failure.
Only then can staging or a bounded native integration run be admitted.

## P1 Existing Whole-Fixture Dependency Ledger

### Declared guest inputs

| Role | Source-built identity | Guest responsibility | Fixture disposition |
| --- | --- | --- | --- |
| NTIO.SYS | 33,792 bytes; `cfc8be16…b3ab4937` | BIOS-facing startup and NTDOS handoff | existing native input |
| NTDOS.SYS | 27,858 bytes; `95766232…7f93bc84` | DOS initialization, file services, `$Exec`, PSP/MCB and termination | must be promoted from identity-only to executed input |
| COMMAND.COM | 50,384 bytes; `908a77ac…c732c43` | command bootstrap and parent-side return handling | must be executed unchanged through namespace input |
| SHARE.EXE | 882 bytes; `69dabbdb…3996fc` | bounded original child: `INT 21h/AH=4Ch` normal termination | declared COM target; no fabricated smoke program |

The identities and source provenance come from the DOS/WOW16 closure ledger and
T198 S92/S93.  They are sufficient artifact evidence, but not execution
proof.

### Required host BOP packages

| Guest stage | BOP family | Existing package disposition | S6 action |
| --- | --- | --- | --- |
| NTIO -> NTDOS | DEM load, memory and GSET (`50:11`, `12`, `50:0D/0F`, `50:1B/32/46`) | T199/T202 package routes; current native evidence reaches typed resumes | verify the current composition admits the same package set |
| NTDOS file and EXEC | DEM file I/O, lifecycle (`50:12/00/42/16/02`, `50:36/3C`) | T225 S2/S3 Direct/Readonly source-built regression | bind only the existing typed provider; no loader seam |
| COMMAND bootstrap | `54:02`, `54:0F` | T198 S95/S96 and later COMMAND package evidence: one shared bootstrap state machine | verify no detached or historical route remains |
| COMMAND launch/lifecycle | `54:01`, `54:04`, `54:05`, `54:0E`, return `54:11` | global COMMAND package ledger; endpoint evidence is not whole-package closure by itself | reconcile every selected profile route before run |
| Child termination | `50:3C -> 54:11` | original guest DOS/COMMAND owner; S2 handles only host-side notification cleanup | retain as the single end-to-end acceptance path |

### Current gaps and decision

The artifacts and component packages exist, but the current evidence does not
prove one *current* composition links all of them as a guest-executed path.
S6 P2 must therefore audit current manifest membership and runtime installation
for every row above, including Direct and Readonly profile selection. Any
missing route is repaired only as its complete owner package; a trace marker
cannot authorize a leaf handler. Overlay and Virtual remain explicitly
unselected and must fail through their declared package policy rather than
falling back to Direct.

## P2 Current Composition Reconciliation

The current runtime closes the prior detached-composition gap:

- `bx_ntvdm_composition_runtime_v1` exact-loads NTIO, NTDOS, COMMAND, target
  and optional terminal image from one validated BYOB selection before guest
  entry.
- It captures the selected real host-drive and volume snapshots, binds one
  DEM mutation-profile consumer and one COMMAND mutation-profile consumer,
  then installs `bx_ntvdm_boot_namespace_composition_v1` and the native BOP
  composition in one resettable lifetime.
- The tracked `t198-s50-bx-vdm-composition-manifest.json` includes the
  composition runtime, both package sessions and their provider families.
- T199 S50's source-built x64 `/MT` global BOP package regression proves that
  every ingress identity has a package-owned success, unavailable, deferred,
  no-op or terminal disposition. It is routing/disposition closure, not a
  false claim that every historical host feature succeeds.

For this fixture, the Direct runtime selects actual host roots through the
CLI drive policy. Readonly uses its explicit namespace/provider policy. The
composition must reject any absent profile mode rather than silently choosing
Direct; deferred Overlay and retired Virtual have no S6 runtime selection.

The remaining S6 implementation question is therefore narrow and testable:
construct a profile whose declared COMMAND and SHARE inputs use the pinned
source-built identities, then prove the existing runtime installs it as one
composition lifetime. The later native run is allowed only after that
source-built installation regression passes; it must observe, not extend, the
already package-owned BOP routes.

## P3 Current Package Regression

A fresh scratch root `build/t225-s6/boot-namespace-composition-r1` ran the
existing `Invoke-T198S16BootNamespaceVdmX64Probe.ps1` with fixture
`boot-namespace` and MSVC x64 `/MT`. It source-built the current DEM and
COMMAND package membership and generated
`t198-s16-bx-vdm-boot-namespace.exe` successfully (process exit zero).

This establishes that the current package composition links as one executable;
it does **not** execute the four declared source-built guest images and is not
an `$Exec` or parent-return result. The scratch evidence has been summarized
here and may be discarded under the scratch-build rule.

The next S6 implementation item is a dedicated installation fixture that
validates the exact NTIO/NTDOS/COMMAND/SHARE profile under Direct and Readonly
mode, including rejection of the deferred Overlay route and retired Virtual route. Its native
execution remains separately gated.

## P4 Legacy Recipe Replacement Decision

The existing S94 compile-only entry is intentionally not used: it requires the
absent historical `artifacts/build/t198-s74-dem-pdb-termination-r1/link.rsp`.
That dependency cannot be restored with prebuilt objects or a product shell.

`tools/probe/Invoke-T220S5SourceCompositionClosure.ps1` is the admissible
replacement build basis. It rebuilds the current CPU5/P bx-core/bx-mantle seed
and all tracked composition-manifest sources under MSVC x64 `/MT`, and its
explicit forbidden-input list excludes the S74 root, `bochs.exe`, `main.cc`,
GUI/plugin/device archives. The next implementation adds a new exact-image
fixture recipe on this basis; it must not repair S94 or inherit its response
file.

## P5 Direct/Readonly Runtime-Selection Admission

The retained shared mutation ABI still represents four historical values, while
the product profile contract is Direct, Readonly and deferred Overlay. S6
admits a composition-only correction: read one explicit runtime mode, default
it to `direct`, accept only `direct` or `readonly`, and reject invalid,
deferred `overlay`, and retired `virtual` values before profile/image/host
capability binding.

The selected immutable mode must initialize the existing shared profile before
DEM and COMMAND owner registration. No endpoint receives a caller-selected
mode, and no mode changes after binding. This is not an Overlay implementation:
its interface value remains reserved and explicitly unavailable at the present
runtime boundary. Virtual is retained only for source compatibility and is
retired from product selection.
## P6 Direct/Readonly Selection Source Closure

`bx_ntvdm_composition_runtime_v1` now reads `NTDOS64_MUTATION_MODE` at
installation time. Missing or `direct` selects Direct; `readonly` selects
Readonly. Invalid, deferred `overlay`, and retired `virtual` strings fail before image, host
namespace, DEM or COMMAND binding. At that point the copied-input installation ABI remained Direct-only; P7 now admits one versioned engine request field so the native CLI can select the same two modes without a string/environment side channel.

The changed translation unit compiled successfully with the generated CPU5/P
pinned configuration under MSVC x64 `/W4 /WX /MT`. A prior full T220 closure
attempt ended during the CPU seed and has no success record; it is not used as
proof. The dedicated Direct/Readonly installation fixture remains the next
required runtime test.

## P7 CLI And Typed-Engine Selection Admission

Both CLI paths now have one explicit `--mutation-mode direct|readonly` option.
When omitted, they select Direct. They reject every other spelling before
process launch or engine execution; deferred Overlay and retired Virtual cannot
fall through to Direct merely because retained ABI values already exist.

- `ntdos64-run` creates one sorted child-only environment and writes
  `NTDOS64_MUTATION_MODE` beside its existing immutable BYOB inputs. PE
  dispatch rejects a supplied DOS mutation option rather than ignoring it.
- `ntdos64-native` carries the mode through a version-2 fixed-width engine
  request. The engine contract accepts only the Direct and Readonly numeric
  values, and `bx-vdm` validates them before profile/image/host binding.
- The existing no-mode copied-input entry remains an explicit Direct wrapper,
  preserving old callers. The new `with_mode` entry is the only native-engine
  selector and does not expose host handles, BOP IDs, DOS semantics, or Bochs
  objects.

### Focused verification

`Invoke-T200S12NativeEngineContractProbe.ps1` rebuilt and ran its contract
fixture under MSVC x64 `/MT` in
`build/t225-s6/engine-contract-profile-r1` with exit zero. It proves Direct
is the clear-time default, Readonly is accepted, and numeric mode `3` is
rejected. Both changed CLI translation units also compile under MSVC x64
`/W4 /WX /MT` in `build/t225-s6/cli-profile-compile-r1`.

The historical T200 S13 lifecycle recipe no longer represents the current
machine-stage closure: it omits the current cancellation/machine-stage
objects and fails link before execution. It is retained only as a stale recipe
observation, not as a failure of the changed mode contract. P8 must provide
the current exact-image Direct/Readonly installation fixture before a native
guest integration run is admissible.
## P8 Product Profile Policy Amendment

The product profile contract is now **Direct, Readonly and Overlay**. Direct
and Readonly are the only enabled runtime selections in S6. Overlay remains a
declared, ABI-compatible extension point: its existing source and evidence are
retained, but no CLI/engine route may select it or silently substitute Direct.

Virtual is retired from the product profile contract. Its existing source, ABI
constant and historical evidence remain temporarily retained for compatibility and
future removal planning; S6 must not add Virtual behavior. If retained Virtual
code demonstrably blocks Direct, Readonly or Overlay progress, a separately
evidenced minimal removal is permitted. Every current product boundary rejects
Virtual explicitly. This amendment replaces
prior S6 references that treated Overlay and Virtual as equivalent deferred modes.

The new `tests/bx-mantle/t225_s6_four_image_profile_fixture.c` compiles under
the existing CPU5/P MSVC x64 `/W4 /WX /MT` configuration in
`build/t225-s6/four-image-profile-fixture-syntax-r1`. It proves only that the
fixture uses the retained profile ABI correctly: Direct and Readonly are the
two required admissions, while deferred Overlay and retired Virtual reject.
It is not yet a linked composition or guest-execution result.
