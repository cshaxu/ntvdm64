# T225 S7 incremental composition-build result

> **Historical record.** Its two custom-cache implementation scripts were retired
> after the full manifest-bound Ninja graph passed; see
> [T225 S7 custom-cache retirement](t225-s7-custom-cache-retirement-001.md).
> Commands below are retained evidence, not current build instructions.

## Result

S7 establishes a local, reproducible module-object cache for focused `bx-vdm`
fixtures. It is a build shim only: no Bochs, guest, BOP or provider behavior
changed.

## Cache contract

`tools/build/Invoke-ModuleObjectCache.ps1` produces a module static library and
always produces a new fixture executable. Each cached object has a manifest
keyed by its source hash, recursively resolved repository-local include hashes,
MSVC identity hash, x64 architecture, `/MT` CRT, language mode, defines and
ordered include roots. A missing or non-identical manifest is a miss.

The cache is intentionally local to `build/t225-s7/`; it is ignored by Git,
not under `artifacts/`, never a source/build admission input, and never a
runtime or release artifact.

## Verification

Command:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T225S7IncrementalCacheProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 -RunId r3
```

The three reviewed manifests below `build/t225-s7/r3/` record the following
focused `bx-vdm-cpu-state-abi` fixture facts:

| Phase | Cache hits | Cache misses | Fresh link | Fixture exit |
| --- | ---: | ---: | --- | ---: |
| cold | 0 | 1 | yes | 0 |
| unchanged hot | 1 | 0 | yes | 0 |
| disposable changed source | 0 | 1 | yes | 0 |

The changed input was a copy under the disposable S7 build root with only a
comment appended. Its cache key differs while its resolved project header hash
is retained, demonstrating source invalidation without reusing the original
object. The target executable is linked on every phase; it is not cacheable
acceptance evidence.

## Limitation and next use

This delivery proves the cache mechanism on a self-contained `bx-vdm` ABI
module. T225 S6 must adopt the same manifest-backed module cache before another
full bx-core/bx-mantle composition closure. The next immediate use is the
focused COMMAND-profile binding probe, which can now avoid rebuilding Bochs.