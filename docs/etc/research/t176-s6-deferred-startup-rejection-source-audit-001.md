# T176 S6 — Deferred Startup Rejection Source Audit

## Scope and inputs

This is a read-only audit of the retained canonical T176 S5 observation
(`artifacts/analysis/t182-s1-share-normal-return-direct-observation-001`) and
the current source plus the retained T174 derivative. The artifact directory
keeps its historical filename; under `t176-s-task-normalization-001.md` its
governance identity is T176 S5.

The observation's `bochs.log` records `deferred startup plan rejected` before
any CPU/BOP marker. Its immutable profile declares an EXE target, a valid v4
machine handoff (`INT 6`, `0F 0B`, `CA80:0003`), and the exact source-built
`SHARE.EXE` target identity.

## Call-chain classification

`src/bochs/main.cc` calls
`bx_ntvdm_adapter_runtime_v1_prepare_deferred_startup_from_environment()`
immediately after native hardware initialization. The current T174 makefile
enables `BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN=1`; this is not a disabled
feature or a request to add a device.

The adapter function returns `-1` only when
`bx_ntvdm_adapter_runtime_v1_install_from_environment()` returns `-1`.
The two environment values were present in T176 S5, so its source consequence
is narrower: `bx_ntvdm_adapter_runtime_v1_install(profile, root)` returned
false. The later deferred-handoff take, #UD ingress, BOP dispatch, DOS,
COMMAND, CPU state and Bochs device paths have not run and cannot be blamed
for this result.

## Installation predicates

| Predicate family | Source owner | T176 S5 disposition |
| --- | --- | --- |
| Profile parse, schema, component identities, and loading NTIO/NTDOS/COMMAND | adapter BYOB profile/image loader | Unobserved within `install`; preflight confirms the declared files only. |
| Target launch declaration (`exe`, empty tail) and exact `TARGET.EXE` load | adapter launch/profile loader | Externally consistent with profile and observation JSON; inner acceptance unobserved. |
| Startup snapshot evidence-root preparation | adapter runtime | No contrary evidence; inner result unobserved. |
| Session resource descriptors and immutable payload shape | adapter runtime/session | Source-defined; inner result unobserved. |
| Host-drive snapshot capture and policy parsing | adapter host-capability boundary | Unobserved. Missing include/exclude switches means unrestricted projection, not automatic rejection. |
| Host-session initialization with 1 MiB aperture | adapter session boundary | Unobserved. |
| Optional machine-observation and trigger validation | adapter machine-profile boundary | Not selected by this profile, hence not a candidate. |

The implementation resets state and returns only boolean failure at every
candidate branch. It does not expose a reason code, nor does the Bochs caller
retain `GetLastError`. Therefore current evidence cannot identify one exact
predicate without a new observation or a diagnostic change.

## Result and bounded follow-up

The earliest proven blocker is **adapter installation observability**, before
the Bochs/CPU/BOP boundary. It is not evidence that `54:11`, normal DOS return,
or any BOP provider is incorrect.

T176 S7 is the bounded next action: introduce an adapter-local, default-off
installation diagnostic with stable failure categories for the existing
predicate families, preserve boolean public behavior and the existing Bochs
boundary, then source-build and statically test it. A new native observation
is explicitly outside S7; it requires a later admitted S after the diagnostic
is proven non-semantic.

## S7 result

The focused adapter-only build accepted the exact retained profile but reported
`LAUNCH_DECLARATION` when replayed through a PowerShell process whose empty
tail value was absent. The CLI's `build_adapter_environment` instead creates
an explicit child environment block containing `NTDOS64_ADAPTER_LAUNCH_TAIL`,
including an empty value. The T176 S4 controller had changed the parent
process environment using `SetEnvironmentVariable`; an empty value is removed
there and so cannot preserve the CLI contract for its child.

This proves the rejection is an **observation-controller environment-block
defect**, not an adapter profile/image failure and not a Bochs/BOP/DOS failure.
The narrow S8 correction must put the six declared variables directly in
`ProcessStartInfo.Environment`, preserving an empty `NAME=` tail while leaving
the parent environment untouched. It must not relax the adapter's requirement
that the launcher contract be explicit.
