# T176 S18–S19 — Finite DEM Lifecycle Link And Observation 001

## Question

Can the source-tested finite `50:3C` provider enter the current Bochs/adapter
composition without a Bochs rebuild, and does the existing SHARE normal-return
path reach and accept it?

## S18 narrow link closure

`tools/build/New-T98S1CurrentAdapterEngineDerivative.ps1` is the retained current
adapter generator. Its explicit source list was extended by one file:
`bx_ntvdm_dem_session_lifecycle_provider_v1.c`. The fresh deferred derivative
is `artifacts/build/current/t176-s18-dem-lifecycle-r1`.

Its manifest declares 67 adapter and four CLI sources, and exactly two
pre-existing Bochs replacement objects (`main.o` and `cpu/exception.o`). The
dry run lists no archive-build prerequisite. The completed link map contains:

| Symbol | Object |
| --- | --- |
| `bx_ntvdm_dem_session_lifecycle_provider_v1_dispatch` | `bx_ntvdm_dem_session_lifecycle_provider_v1.obj` |
| `bx_ntvdm_boot_namespace_provider_v1_terminate_pdb` | `bx_ntvdm_boot_namespace_provider_v1.obj` |
| `bx_ntvdm_search_sessions_v1_release_pdb` | `bx_ntvdm_search_session.obj` |

The linked executable SHA-256 is
`CC7F430D2BD841618E479F04B322B6800E782E802AFA4881D7470940E5462A51`;
its map SHA-256 is
`46F1E0C596F6328A6F2E7EEFD918E70328B5BA64FF3502EA4E2A17F73ABE46DA`.
This is a narrow adapter link closure, not a Bochs archive/device rebuild.

## S19 one bounded observation

The sole direct-PID run retained
`artifacts/analysis/t176-s19-share-normal-return-lifecycle-observation-001`.
It used the unchanged S13 BYOB profile SHA-256
`543DCA5555DF7D0D0AA7FC828F66C5B114E3C9AC7C3D0E39120E280C9798F67B`,
the same configuration SHA-256
`41F284D4880839EB1852A0C5361CE704C38D07D60587E9B848476023058F83DF`,
and empty declared DOS tail. PID 12584 ran for 33.511 seconds and was stopped
by the declared 30-second watchdog; log SHA-256 is
`0906939B0F4CF6E371ACB607C60E4AE1952C9C14B8B1744FDE24E5049C2ABF7D`.

At log lines 533–534 the trace records:

```text
selector=50 ... service=3c ... bx=0d00 ... rip=532f
adapter accepted resume ... rip=532f next=5333 delta=00
```

Thus the reached OpenNT PDB-termination BOP selected the finite provider and
performed its generic four-byte resume. `delta=00` confirms no GPR/flags
mutation. The focused S17 test, rather than this ABI-visible trace, proves
owned, foreign and absent-slot release behavior.

## Limits and disposition

This trace does not reach `54:11`; it therefore does not demonstrate COMMAND
normal completion or a CLI exit result. After accepted `50:3C` continuation it
eventually reaches the already-known native real-mode stack-prefetch fault and
third-exception stop. The watchdog result is not a controlled guest exit. No
retry or new BOP/provider/device work is authorized by this observation.

## Follow-up

T176 S20 should source-correlate the post-`50:3C` continuation from
`0032:5333` to the first independently reachable owner boundary. It must
determine whether `54:11` is structurally expected before the existing native
fault, and may not infer that absence from one bounded trace is provider
failure.
