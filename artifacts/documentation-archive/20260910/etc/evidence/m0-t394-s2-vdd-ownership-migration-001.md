# M0 T394 S2 — VDD ownership migration

## Scope and result

Moved exactly five admitted VDD paths from `adapter-mvdm-host-out/softpc` to
the existing `adapter-mvdm-host-out/vdd` family:

| Final path | Role |
| --- | --- |
| `vdd/include/mvdm_softpc_vdd_configuration.h` | installable-VDD configuration declaration |
| `vdd/mvdm_softpc_vdd_configuration.c` | registry configuration boundary |
| `vdd/mvdm_softpc_vdd_unavailable.c` | `ClearInstanceDataMarking` unavailable boundary |
| `vdd/include/mvdm_vdd_sft_shadow.h` | SFT/JFT shadow declaration |
| `vdd/mvdm_vdd_sft_shadow.c` | session-owned SFT/JFT shadow implementation |

No function body, signature, status/failure rule, VDD callback behavior,
guest byte, BOP, session contract or termination behavior changed. No
forwarding header was needed: all selected source and fixture consumers resolve
the retained public header spelling through the `vdd/include` root.

## Build-graph result

`New-T310OriginalSoftpcNinja.ps1` now emits a separate `adapter-vdd` object
group and `vdd-bindings.lib` on both x86 and x64. Each generated graph names
only these three VDD bodies under `obj/adapter-vdd`; no old SoftPC VDD body
path appears in the generated graph. The formal product, forced closure and
Redirector DLL link inputs consume `vdd-bindings.lib` independently from
`softpc-bindings.lib`.

The SFT/JFT fixture moved with its owner to
`tests/adapter-mvdm-host-out/vdd`. Its generator now makes both the VDD public
include root and original `mvdm-host/inc` (`doswow.h`) explicit. The latter
was a previously implicit include dependency exposed by the ownership move.

## Verification

All commands used the current migrated paths and MSVC `/MT` with explicit x86
or x64 environments.

| Check | x86 | x64 |
| --- | ---: | ---: |
| `t280_s20_mvdm_vdd_sft_shadow_fixture` fresh compile and run | pass / 0 | pass / 0 |
| `t394_s2_vdd_boundary_fixture` fresh compile and run | pass / 0 | pass / 0 |
| Formal-graph VDD bodies (`shadow`, `unavailable`, `configuration`) object compile | pass / 0 | pass / 0 |
| Source/test/tool stale-path sweep | zero matches | zero matches |

The new T394 fixture proves: a null configuration output pointer returns the
documented invalid-parameter error; a configured/missing/error registry state
keeps the documented result/handle shape; and a no-session
`ClearInstanceDataMarking` call retains `ERROR_CALL_NOT_IMPLEMENTED` without
pretending a session terminated. The existing shadow fixture continues to
cover allocate/associate/commit/retrieve/discard and cross-session identity
rejection.

## Disposition

S2 is closed. The next proposed packet is S3 only: the two COMMAND
standard-stream redirection paths. The mixed termination/observation file and
`error_abi.h` remain explicitly outside S2 and retain their S5 audit gate.
