# M0 T418 S1 legacy broker archive removal

## Decision

The six-file `broker.lib` local plane is deleted. It was a 387 physical-line
in-process registry (`broker`, `wire`, and `base_vdm_record`, with headers),
not the live authenticated transport selected by the three executable product.
The paired `wire_test` and `base_vdm_record_test` fixtures are deleted with it.

## Source-first proof

Before removal, the repository-wide source sweep found no production include,
call, export, generated-graph dependency or selected test target for this
plane. The only direct users were its two isolated tests. The live broker uses
`vdm_message`, `vdm_values`, `vdm_startup`, `vdm_receipt`, `vdm_delivery`,
`console_membership`, `rpc_security` and `service.idl`; all remain selected in
`broker-transport.lib`. In particular, the original BaseSrv strategy remains
in mirrored `opennt-host/base/win32/server/srvvdm.c`, rather than in the
deleted local record.

## Fresh x86 proof

`New-T310OriginalSoftpcNinja.ps1` generated the fresh
`build/M0-T418/S1/broker-free-x86-002` graph. It contains none of
`broker.lib`, `broker-base-vdm-record-test.exe` or
`obj/broker/{broker,wire,base_vdm_record}.obj`. A sequential native x86 build
completed all 506 selected commands:

```text
SEQUENTIAL-BUILD-PASS commands=506
```

The graph produced `run16.exe`, `basesrv.exe`, `ntvdm.exe`, the CCPU/C-VID
fixtures and `VDMREDIR.dll`. The VDMREDIR link initially exposed a pre-existing
missing closure: its original NetLib call to `RtlNtStatusToDosError` needs the
already-selected `original-opennt-rtl-x86.lib`. The generator now links that
original RTL archive; no replacement implementation was added.

The final forced-closure DLL reports deliberately unresolved original process
and native-child seams under its existing `/FORCE` audit rule. That target is
non-runnable by design and does not weaken the successful product links.

## Result and handoff

S1 removes 387 archive-source lines and two unreachable tests, while retaining
the live service transport unchanged. T418 S2--S5 may now move the retained
process-owned code into `run16`, `basesrv` and `ntvdm` without treating the
deleted local registry as a migration candidate.
