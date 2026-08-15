# T202 S2: DEM host-namespace directory validation

## Question

Can the DEM namespace/CWD package validate an admitted real-host DOS-relative
directory without reintroducing the historical process-wide current-directory
side effect or exposing a host path/handle to guest ABI?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demdir.c` (`demQueryCurrentDir` and
  `demSetCurrentDir`);
- `src/opennt/base/mvdm/dos/dem/demgset.c` (`demSetDefaultDrive`);
- the S30 root-handle namespace and T201 common mutation profile boundary.

## Procedure

Added `bx_ntvdm_host_namespace_v1_directory_exists`. It accepts only an
admitted drive index and an empty or DOS 8.3 backslash-separated relative
directory. It resolves every component below the retained root handle,
rejects reparse points, malformed components and `..`, and returns only a
boolean. It neither constructs an absolute host path nor mutates the process
current directory.

The MSVC x64 `/MT` fixture creates a temporary directory under a real selected
host drive, verifies the root, its relative path and a nested `SUBDIR`, then
verifies `..` rejection. The fixture cleans up its own temporary files.

## Observations

`Invoke-T202S2HostNamespaceProbe.ps1` succeeded at build
`artifacts/build/t202-s2-host-namespace-r3`:

```text
bx-ntvdm-host-namespace-test: contained DOS projection and deterministic enumeration verified
```

The same fixture compiled in the restricted sandbox but could not open the
real drive root; this is an execution-sandbox limitation, not a product
fallback. The passing run used the ordinary user-mode host permission
boundary, performed no installation or system configuration change, and had
no guest execution.

## Interpretation

This is one package primitive, not a `50:13`, `50:18`, or `50:1A` closure.
The next S2 step may build a DEM-owned per-drive CWD record on it and route all
three source-shaped operations through the shared direct/readonly/overlay/
virtual profile result. It must not call `SetCurrentDirectory` on the CLI
process merely to preserve an historical implementation detail.

## Follow-up

Implement the DEM session-private CWD/volume state and its four profile
outcomes, then connect the complete current-directory/default-drive family as
one package regression.
