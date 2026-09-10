# T222 S1 P2 — Shared DEM Direct/Readonly drive-view provider design

## Decision

T222 will not add another `dem_*_service` recognizer.  It will introduce one
package-local `dem_drive_view_provider_v1` state owner inside the existing DEM
package session, and migrate the existing boot-drive, drive/volume snapshot,
CWD, DTA, DPB, media, clock and computer-name helpers behind it.

The provider owns only adapter-local fixed-width state:

```text
mutation profile + admitted drive snapshot + immutable volume snapshot
+ selected boot/default-drive state + per-drive session CWD context
+ checked DTA registration + optional explicit clock capability
```

It holds no raw guest pointer, host `HANDLE`, Bochs object or OpenNT/SoftPC
state.  `dem_package_session` remains the BOP ingress/gather coordinator;
`bx-core` and `bx-mantle` remain selector-blind.

## Profile contract

| Concern | Direct | Readonly | Retained later interfaces |
| --- | --- | --- | --- |
| Drive/volume/media/identity observation | selected real host drives captured into the session snapshot | same admitted observation; no fallback to excluded host drive | Overlay/Virtual retain their existing snapshot/view interfaces unchanged |
| CWD/default drive | validated host path and explicit CLI/VDM-session CWD state; no ambient process-CWD mutation | validated declared/selected read view and private session CWD state | later profile backends supply their own view resolver to the same provider contract |
| DTA/DPB/guest output | checked fixed guest ranges and source layouts | identical mechanical transport | profile-neutral |
| Date/time query | host observation | host observation | profile-neutral |
| Date/time set | only through an explicitly admitted direct clock capability; preserve OpenNT privilege-not-held `AL=0` rule | explicit non-mutating, source-derived result; never call `SetLocalTime` | no Overlay/Virtual behavior is added |
| Media ID set | original unsupported `CF=1` | same | unchanged |

`readonly` is a storage/content policy, not a reason to hide the admitted host
drive inventory, time, machine identity, DTA or DPB.  It does prevent content
and host-global clock mutation unless a future profile explicitly admits one.

## Required migrations

1. **State and admission:** initialize the provider from the existing copied
   mutation profile, drive snapshot and volume snapshot at session setup.
   Reject an absent, excluded or unavailable drive through the source-specific
   invalid-drive/media result; never fall through to a static boot root.
2. **CWD group (`13,18,1A`):** retain `dem_cwd_service_v2`'s checked CDS/path
   buffers and source register forms, but route resolution through the
   provider.  Direct must use an explicit session-scoped host CWD capability;
   it must not change the parent shell's ambient CWD by accident.  Readonly
   uses the same view contract without a host content mutation.
3. **Observation and guest-layout group (`0D,0E,0F,10,25,41,46`):** retain
   existing snapshot/geometry algorithms, then unify their selected-drive
   admission and checked output transactions.  `25` and `46` share one full
   DPB constructor; `10` retains original set-unsupported behavior.
4. **Clock/DTA group (`14,15,19,1B,1C`):** retain query layouts and DTA's
   four copied locators.  Make `19/1C` one explicit capability decision rather
   than the present unconditional failure leaf.
5. **Removal/migration:** after the source-built family fixture covers all
   15 identities, delete or migrate the detached deferred-selection leaves;
   no identity may fall through to `dem_cli_unavailable_provider`.

## Implementation sequencing and regression

S2 will implement one provider/state boundary and family regressions in three
units, all through normal package session/ingress:

1. state/admission + drive/volume/media/identity (`0D,0E,0F,10,41`);
2. CWD/default-drive + DTA (`13,18,1A,1B`);
3. clock + DPB list (`14,15,19,1C,25,46`).

Each unit receives Direct and Readonly positive/negative cases.  The final
fixture verifies every identity, profile isolation, excluded-drive behavior,
exact checked guest output sizes, privilege-sensitive clock terminal and
absence of generic fallback.  Native trace is prohibited until the entire
family fixture is green.

## Rejected alternatives

- Leaving current `gset_plane`, boot, volume and CWD leaves independently
  routed would retain the mixed fixture/deferred composition.
- Invoking `SetCurrentDirectory` on the Codex/parent process would not be an
  explicit VDM session boundary.
- Treating `SetLocalTime` as missing because it may require privilege would
  lose OpenNT's special result rule.
- Extending Overlay/Virtual while this Direct/Readonly package is active would
  violate the frozen-profile scope amendment.
