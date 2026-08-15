# T212 S3: Native CLI Lifecycle Integration

Packet: M0 T212 S3, Ordinary Mode  
Date: 2026-08-15  
Status: source-built integration closure

## Delivered integration

`src/cli/ntdos64_native_cli.c` now creates the S2 copied lifecycle policy
before it forms the unchanged engine request, derives the existing default
`1,000,000` instruction-tick budget from that policy, then classifies the
returned engine result only after `bx_ntvdm_engine_run_v1` has returned.

The CLI output now retains the source engine facts and appends the distinct
CLI-owned facts:

```text
terminal=<engine> detail=<engine> lifecycle=<cli> presentation=<cli> cancellation=<cli>
```

The host process exit preserves the prior broad mapping for the present engine
outcomes. It remains a host presentation code, not a DOS register result. No
normal completion or host cancellation is reported unless that value is
actually returned by the engine.

## Current composition closure repair

The initial fresh rebuild exposed four stale omissions from
`tools/build/t198-s50-bx-vdm-composition-manifest.json`, all already-existing
current-composition dependencies:

| Omitted source/requirement | Consumer that proved the dependency | Repair |
| --- | --- | --- |
| obsolete `bx_ntvdm_cmd_cli_profile_service.c` | manifest existence check | removed; replaced with the four T211 COMMAND owner providers |
| `bx_ntvdm_session_host_context_v1.c` | DEM CWD and COMMAND package-session link | added |
| `bx_ntvdm_startup_configuration_provider_v1.c` and policy | boot namespace provider link | added |
| `bx_ntvdm_normal_terminal_sequence_observation_v1.c` | generic bridge native CLI link | added |
| `ntdll.lib` | current host namespace link | explicit native CLI recipe import |

The repaired manifest has 135 source records, zero missing paths, and its
fresh `r3` build has 134 rebuilt linked-member objects; the generic bridge is
the declared compile-only member. The old fixture run exits `2` under its
inherited assertion and is retained only as negative fixture evidence. It is
not used as CLI success evidence.

## Native CLI verification

Command:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T212S3NativeCliLifecycleProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t212-s3-native-cli-lifecycle-r6 `
  -CompositionBuildRoot O:\repos.hobby\ntdos64\artifacts\build\t212-s3-current-composition-r3
```

The fresh MSVC x64 `/MT` CLI link and run passed its declared negative
integration expectation. The current profile returned engine terminal `2`,
detail `1`—a composition rejection—and the native output was:

```text
ntdos64-native: terminal=2 detail=1 lifecycle=3 presentation=3 cancellation=0
```

The CLI exited `3`, exactly matching that classified composition rejection.
This demonstrates that the lifecycle layer preserves rather than disguises
the current engine result. It does not claim guest execution, normal guest
completion, cancellation, or successful product admission.

## Follow-up

The next lifecycle work is not a trace-driven BOP patch. T212 S4 first maps
the exact cooperative cancellation seam required for the in-process engine:
where a CLI request can be observed, which mantle-owned execution boundary
can stop safely, and how cleanup/result ownership remains unchanged. It must
also disposition wall-time and resource limits. No source change is admitted
until that map shows a fixed-width, selector-blind contract.
