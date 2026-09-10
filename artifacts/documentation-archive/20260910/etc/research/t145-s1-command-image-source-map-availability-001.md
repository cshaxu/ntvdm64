# T145 S1 COMMAND image/source-map availability 001

## Inputs

The controlled artifact is source-built `COMMAND.COM`, 50,384 bytes,
SHA-256 `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`.
Its build manifest is
`artifacts/toolchain-runs/command-tools16-opennt-v1/command-build-manifest.json`.

## Availability result

The retained command build tree contains the artifact, source tree and
historical tool/manifests, but no emitted `.map`, `.lst`, or `.sym` artifact
that binds offsets in this exact COM binary to COMMAND source labels. The
manifest/source-build identity proves provenance, not a symbolic instruction
map.

Consequently the observed physical tail address cannot be attributed to a
COMMAND source routine. Reconstructing a source map from a different compiler
or from heuristic disassembly would not be evidence for the controlled binary
and is outside this task.

## Cross-check

The existing source ledgers independently prove the required ownership chain:
normal DOS open/seek/read/close feeds original NTDOS `$Exec`, then starts
COMMAND. That supports the contained namespace/provider architecture but does
not prove either the allocation size or an instruction path at the tail.

## Disposition

The source-map gap is explicit. Further progress needs a runtime-equivalent
guest control-flow observation that preserves the T130 CPU object identity, or
an independently reproducible historical command listing build. Neither is
admitted here. No BOP, adapter, Bochs, guest-image or machine change follows.
