# M0 T207 S1 — passive normal-terminal sequence map

## Question

Which existing observation seams can prove, without changing execution, that
one integrated run saw final COMMAND `54:0B` resume before a later terminal
outcome?

## Existing seams

| Seam | Owner | What it records | Insufficient fact |
| --- | --- | --- | --- |
| `bx_ntvdm_normal_return_observation_v1` | bx-vdm | One accepted exact `54:0B` resume and DX-to-AX evidence. | No later terminal outcome or ordering. |
| `bx_ntvdm_terminal_observation_v1` | bx-vdm | First accepted typed STOP and aggregate counts. | No `54:0B` predecessor or STOP identity classification. |
| `bx_ntvdm_mantle_generic_ud_stop_observation_*` | bx-mantle | Selector-blind boolean used only to distinguish typed STOP from watchdog. | Must remain selector- and provider-blind. |
| engine direct-composition fixture | test | Current copied engine installation, execution and reset lifecycle. | It enables only the STOP observer and currently proves a `54:01` declared-plan STOP. |
| T203 finite native fixture | test | Default-off `54:0B` observer under CPU5 finite run. | It observed no `54:0B`; its finite lifecycle is not engine-qualified. |

The first two bx-vdm observers run after an accepted product bridge outcome,
but they maintain independent records. Their current ABI cannot prove that
the same run saw normal return before the later STOP.

## S2 admission

S2 may add one **test-only, default-off bx-vdm sequence observer**. It is a
post-dispatch listener called only after the existing bridge accepted an
outcome. It may copy at most:

- one exact `54:0B` RESUME witness (fixed existing predicate);
- the first subsequent typed STOP witness; and
- a fixed-width order/status field.

It must not read guest memory, select a provider, alter CPU/result/timer state,
create a stop, retain a pointer, export through bx-mantle, or become an engine
or CLI result. The generic mantle stop boolean remains unchanged. The existing
engine direct-composition fixture is the target integration harness because
it uses copied request installation and ordinary engine reset.

| Required S2 outcome | Meaning |
| --- | --- |
| no normal witness + STOP | Existing declared-plan terminal or another stop; not ordinary completion. |
| normal witness + later STOP | Candidate ordered sequence only; requires source attribution and separate publication admission. |
| normal witness + budget/failure | Explicit non-completion. |
| malformed/non-`54:0B`/wrong-resume | No capture. |

## Inputs

| File | SHA-256 |
| --- | --- |
| `src/bx-vdm/bx_ntvdm_normal_return_observation_v1.c` | `CE21F948CFB5E3E65EADA418B74C95A92026957FE40C6C67D8D3EAE7880D0E08` |
| `src/bx-vdm/bx_ntvdm_terminal_observation_v1.c` | `0B8455521E00961C4906D38A688B46637E98D0E6F7D429FC87419381F168E1E2` |
| `src/bx-vdm/bx_ntvdm_vdm_generic_ud_bridge_v1.c` | `CD1CD975F9E8CA494AD96C47BCFE4362149CD2055A22808448D543F5A31F4F69` |
| `src/bx-mantle/bx_ntvdm_machine_stage_v1.cc` | `37A6312F2F36AD627DFD5A6EDD0692AE818524205FD5088F305701521BA54FD2` |
| `tests/bx-mantle/bx_ntvdm_engine_direct_composition_v1_test.c` | `894EB8C80D04E127C35E277C7E0F297470E8C4036AB1034E8F68FFE7C43EA5A2` |

