# M0 T198 S114: Post-Commit DEMFASTREAD Native Observation

## Result

The sole S114 native run proves that the restored source-derived `50:42`
provider is active on the real startup path:

```text
fast-read-commit observed=1 resume=6355 gpr16-mask=00000001 ax=000d
flags-mask=00000001 flags-value=00000000
spckbd observed=1 state=8dc8:45a6 eax=0000beef ...
```

Thus one fast-read writes an actual 13-byte count to AX, clears CF and resumes
after its four-byte BOP. The original BOP `5F` stream initialization follows.
The same run retains a later declined `50:42` fallback record, so the success
does not falsely claim that every fast-read is supplied.

## Reproducible Evidence

- Build root: `artifacts/build/t198-s114-fastread-postcommit-native-r1`
- Provenance: `t198-s97-post-bootstrap-native-probe.json`, `runs: 1`, current
  manifest, exact source-built S93 inputs and CPU5/Pentium-MMX closure.
- Runtime: MSVC x64 `/MT`, bounded fixture exit `2`.
- `run.log` SHA-256:
  `d4843f7d7c3eb9cf5e90796a1e69e964cf7c811b4d5a10d9f455fffd72d2f730`.

## Disposition

The provider, checked guest-RAM action and typed resume are now native-proven
for one real `50:42`. No Bochs, ROM, VGA, device, write capability or CLI
namespace expansion is implicated. S115 is source-only and owns the distinct
declined `50:42` call.
