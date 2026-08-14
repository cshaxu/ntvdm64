# M0 T198 S111: Restored DEMFASTREAD Native Observation

## Result

The sole admitted native run of the restored composition reaches the original
stream initialization BOP `5F` after its `50:42` event, and does not transfer
to `C000:014A`.

This directly reverses S107's no-op regression. It proves path recovery, not
DOS completion, console output, a VGA capability, or continuous execution.

## Reproducible Evidence

- Build root: `artifacts/build/t198-s111-fastread-restored-native-r2`
- Provenance: `t198-s97-post-bootstrap-native-probe.json`, `runs: 1`, exact
  S93 source-built COMMAND/SHARE inputs, current manifest and CPU5/Pentium-MMX
  inherited closure.
- Runtime: MSVC x64 `/MT`; fixture exit `2` / native controlled observation.
- `run.log` SHA-256:
  `f59318f786316646ac64efd8463467fc5e38e3feaaa481686c4262e1ea166b00`.

The log records `fast-read observed=1` at `0032:6351`, then
`spckbd observed=1` at `8dc8:45a6` with `eax=0000beef`.

`spckbd observed=1` is recorded before composition delegation and confirms the
canonical `C4 C4 5F` ingress. The current composition owns its exact CS:SI/DS
stream-state transaction; S89/S90 proves the resulting state selects the guest
stream path. The absence of a C000 generic-UD record is the meaningful
comparison to S107.

## Limits and Follow-up

The passive bridge's final `next=50:42` telemetry is not itself a provider
outcome marker. S112 must source-map its recorder ordering and the composition
return path before interpreting it as an accepted or declined transaction.
S111 permits no second guest run.
