# T154 S1 fast-read provider source/test/build closure 001

## Scope

T154 implements exactly the T153 source-derived x86 `50:42` FASTREAD
provider.  It is adapter-local, reads only the declared finite boot namespace,
and uses the existing checked bulk-result/session ABI.  `50:43` FASTWRITE is
not selected and remains the existing original-unavailable/no-op disposition.

## Changed composition

- `bx_ntvdm_dem_fastio_provider_v1.[ch]` is the explicitly named fast-I/O
  family provider; it delegates only its read half to the bounded boot
  namespace provider.
- `bx_ntvdm_dem_readonly_file_v1_fast_read` recognizes only the `C4 C4 50
  42` instruction window and retains the original copied handle, position,
  count, destination and ZF convention used by normal read.
- runtime selection is strictly after the normal plane gate and strictly
  before the original-unavailable fallback for `50:42` alone.  A prepared
  bulk transaction is queued through the pre-existing host session.
- no Bochs source, CPU ABI, RAM primitive, device, firmware, guest source,
  ambient host namespace, DEMREAD alias, write backend or MIPS-state change
  is present.

## Reproducible verification

The current `CMakeLists.txt` target source lists were extracted verbatim and
compiled directly with `C:\\msys64\\ucrt64\\bin\\gcc.exe` (GCC 16.1.0),
using C11, `UNICODE`, `_UNICODE`, `WIN32_LEAN_AND_MEAN`, the existing
`src/cli` and `src/bx-ntvdm-adapter` include roots, and `bcrypt`.

| Target and invocation | Inputs | Result |
| --- | --- | --- |
| `bx-ntvdm-dem-readonly-file-service-test` | 13 listed C sources | exit 0; reports bounded open/seek/read/close lifecycle. |
| `bx-ntvdm-adapter-runtime-test` | 69 listed C sources | default invocation exit 0; reports identity-to-pending-transaction path. |
| `bx-ntvdm-adapter-runtime-test --t97-path-search` | same 69-source executable | exit 0; exercises common DEM-plane `50:42` routing from the finite v4 boot namespace. |

The final artifacts are:

| Artifact | SHA-256 |
| --- | --- |
| `artifacts/build/t154-adapter-tests-gcc-direct/fastread-test-r5.exe` | `9C542E92F00879C430600C3ED779F8C62385515E23613D240F7CA68FB8C677D8` |
| `artifacts/build/t154-adapter-tests-gcc-direct/bx-ntvdm-adapter-runtime-test-r6.exe` | `932DC2F34FA44497F1B631A7F52BF8CCAB1862B3A3FC772E5E5A0959A513911F` |

The focused test proves full read, short read, EOF, ZF-controlled seek,
invalid destination rejection (CF set, AX 87 and no transaction), and that
the fast-read helper declines `50:43`.  The runtime plane test proves
`50:42` returns AX 13 with CF clear and queues the thirteen-byte
`REM NTVDM64\r\n` CONFIG payload at real-mode address `0100:0090`, i.e.
physical `01090h`.

## Build-front-end limitation

The project-owned CMake configuration requires MinGW-w64 GCC.  The available
MSYS `make` rewrites native `O:\\` paths, and the available native Ninja
binary is blocked by local execution policy.  Direct compilation above keeps
the declared modern UCRT64 toolchain and the authoritative target source
lists, but is evidence of adapter test/build closure rather than a replacement
for the owned CMake/Ninja frontend.

## Disposition

This closes static adapter source/test/build and runtime-plane coverage only.
It does not rebuild the Bochs derivative or claim a Bochs guest observation.
That recomposition and its bounded real-path observation are separate work.
