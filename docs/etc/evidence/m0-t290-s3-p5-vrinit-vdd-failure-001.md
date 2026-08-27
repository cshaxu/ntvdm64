# M0 T290 S3 P5 — Original Redirector initialization failure proof

## Scope

This delivery composes the exact original
`mvdm-host/vdmredir/vrinit.c` translation unit only far enough to prove its
source-defined unavailable result when the historical VDD product hook is not
present.  It neither enables a `57:xx` entry nor supplies a VDD, NetBIOS, DLC,
async completion, physical IRQ, or guest-pointer success path.

## Source identity and recovery ladder

1. **Original source reuse:** `src/mvdm-host/vdmredir/vrinit.c` is unchanged
   (SHA-256 `088ecff80459ed358ebf4f7dd291821748475a8fbdccaf27a1ee1df2b6648273`).
2. **Smallest same-shaped bindings:** the fixture supplies only the link
   carriers that the selected early-return branch cannot reach: the historical
   VDD hook rejects, NetBIOS/DLC counters observe non-entry, and the
   selector-disabled `vrdisp.c` helper and NetBIOS/PIC symbols satisfy static
   linking.  They are not providers and cannot produce success.
3. **External-code intrusion:** none.
4. **New behavior:** none.  The only imported file is the exact public SDK
   declaration carrier `mvdm-platform-abi/source/public/sdk/inc/dlcapi.h`.
   Both canonical OpenNT sources have SHA-256
   `c98cae26f503b5285e465b2fd40e0fe1a6344a44d5bb5bd00e876f20dd8c1dda`.

## Commands and result

Both commands were run outside the sandbox with the repository MSVC `/MT`
toolchain:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T290S3VrInitFailureNinja.ps1 -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T290/S3/vrinit-failure-x86
.\build\M0-T290\S3\vrinit-failure-x86\bin\t290-s3-vrinit-failure-fixture.exe

powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T290S3VrInitFailureNinja.ps1 -Architecture x64 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T290/S3/vrinit-failure-x64
.\build\M0-T290\S3\vrinit-failure-x64\bin\t290-s3-vrinit-failure-fixture.exe
```

Both fixtures exited `0`.  They prove:

- `VrInitialize()` returns `FALSE` when `VDDInstallUserHook` rejects.
- `VrNetbios5cInitialize`, `VrDlcInitialize`, and physical interrupt delivery
  are not entered on that path.
- `VrInitialized()` remains false.
- `VrUninitialize()` preserves its original carry-clear result even after the
  unavailable initialization attempt.

The original source still emits its historical compiler diagnostics
(`C4005`, `C4431`, `C4210`, plus x64 `C4311` on its unselected direct guest
pointer success branch).  The Ninja recipe records rather than suppresses
these; source is unchanged.  The VDD success branch, guest-visible
`VDM_LOAD_INFO` write, NetBIOS/DLC initialization and asynchronous IRQ
machinery remain explicitly deferred to their named T290 owner groups.
