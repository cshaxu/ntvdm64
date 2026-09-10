# T221 S3 P44 — Package-session matrix transport map

## Purpose

The remaining T221 regression must exercise all 29 selected identities through
`dem_package_session`, rather than direct calls into provider partitions.  This
map records the common session transport prerequisites before that one matrix
fixture is written.  It does not change provider semantics or claim a family
closure.

## One owner session, five routes

| Owner subfamily | Identities | Session route | Fixture transport required |
| --- | --- | --- | --- |
| Namespace/path | `01,03,04,05,06,12,17,22,44` | `namespace_dispatch` | Fixed RAM-backed action executor. `03/12/22` additionally require the copied two-byte CurrentPDB read from registered DTA; `17` gathers two path inputs. |
| Handle I/O | `00,02,08,16,1E,27` | `handle_dispatch` | Opaque token result carried only in registers. `1E` needs RAM gather; `16` needs checked RAM writeback. |
| Search | `09,0A,0B,0C` | `search_dispatch` | Registered DTA plus RAM gathers/writes. Pathname and FCB forms remain different byte layouts; `0B/0C` consume typed continuation. |
| FCB state | `07,20,2C,2D,2E,2F,30,31` | FCB path/direct/I-O dispatch | Path and wildcard forms gather FCB/path buffers; `2E/30` are direct typed token operations; `2F` requires both the registered DTA and a checked I/O action. |
| Pipe EOF | `47,48` | `redirector_deferred` | No namespace, token, DTA or host fallback is permitted; both retain the source-labelled deferred invalid-handle result. |

## Matrix fixture shape

The next fixture must create one boot namespace plane and one package session
per mutation profile.  Each session registers the three shared DEM classes,
installs one admitted host namespace (or the explicitly selected Virtual
view), registers DTA and a copied CurrentPDB word, and uses a single bounded
RAM action executor.  It then runs positive and negative owner-family cases
through normal BOP ingress and registry selection.

This gives every identity the same mechanical boundary.  It forbids direct
partition calls, host handles in guest state, selector-local profile choice,
and generic CLI-unavailable fallback for the 27 non-pipe identities.

## Source basis

- `src/bx-vdm/bx_ntvdm_dem_package_session_v1.c`: dispatch ordering and the
  three common mechanical-action helpers.
- `src/bx-vdm/bx_ntvdm_dem_*_route_partition_v1.c`: fixed path/FCB/handle
  gather contracts.
- `tests/bx-vdm/bx_ntvdm_dem_whole_provider_v1_test.c`: existing source-built
  Direct/Readonly/Overlay/Virtual provider semantics and ABI inputs.

## Gate

P44 is an implementation-map checkpoint only.  T221 S3 remains open until
the resulting matrix executes all 29 identities with positive/negative
Direct, Readonly, Overlay and Virtual owner-family evidence.  Native trace
remains out of scope.
