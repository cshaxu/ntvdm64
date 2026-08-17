# T225 S7 Full Ninja Module Graph Result

## Result

The formal full module graph is now manifest-bound and buildable with Ninja
1.13.2 and MSVC x64 `/MT`. It uses only current repository source and the
pinned CPU5/Pentium-MMX, non-x86-64 Bochs configuration projection.

## Membership and identity

`tools/build/t225-s7-full-module-manifest.json` is the explicit membership
authority; the generator accepts no source glob. The checked graph contains:

| Archive | Sources |
| --- | ---: |
| `bx-core.lib` | 122 |
| `bx-mantle.lib` | 15 |
| `bx-vdm.lib` | 149 |
| `cli.lib` | 9 |

The resulting manifest identity is:

- source manifest SHA-256: `cca15a9622821a14b0d5e0ca38dfa8d18a45e991a25a61e8b3e2aad186360816`;
- projected configuration SHA-256: `cb4ede495b5dc4e481d51dcf411e43b17d2d664a476a3039a838ba9f5a4e8f6d`;
- MSVC identity SHA-256: `4a25db7d8fabe8eeae00b365f8f05cbcd2a026db52fcbb4f44c08670d190e95c`;
- generated graph configuration SHA-256: `4aa33cc54ea2d48efc9241602969871114e801331f1ed6138503a9f3e8c07ce5`.

The generator uses `deps = msvc`, `/showIncludes`, and response files for
archive/link edges; the latter is required because the `bx-vdm` archive exceeds
the Windows command-line limit when its object list is expanded inline.

## Verification

A clean disposable root `build/t225-s7/full-r6` completed all 303 edges:
all four static archives, `ntdos64-native.exe`, and the admitted
`t225-s6-four-image-profile-fixture.exe`. A subsequent `ninja -n` reported
`ninja: no work to do`.

A reversible timestamp-only update to
`src/bx-vdm/bx_ntvdm_cpu_state_abi.h` selected exactly 102 dependent compile,
archive, and link edges through MSVC include discovery. It completed, restored
the header timestamp, and a further `ninja -n` again reported no work.

The Ninja-linked S6 fixture was run only with the approved local OpenNT
reference-input staging contract. Its process exit was zero: Direct and
Readonly installed; deferred Overlay and retired Virtual were rejected at stage
`-1`, as required. No guest execution, BOP semantic claim, or source-built
artifact claim follows from this regression.

## Boundaries

This establishes build composition and incremental rebuild mechanics only.
The graph deliberately excludes `artifacts/build`, `bochs.exe`, historic
product `main.cc`, `config.cc`, GUI/plugin/device archives, and prebuilt adapter
composition. The native CLI executable is a fresh link proof, not a runtime
admission.

The full graph now supersedes the P1 custom object-cache implementation. Its
historical evidence remains indexed, but the two executable cache scripts may
now be removed in the dedicated retirement change.