# T200 S30: real-host capability source and ABI map

## Corrected composition

NTDOS64 is a normal user-mode CLI. It may call existing Win32 and registry
APIs using the caller's normal permissions, but it never rebuilds, patches or
installs Windows components. The default DOS drive view is therefore the
CLI-policy-filtered view of real host drives. A virtual boot C: is permitted
only as a separately selected, default-off fallback profile.

```text
CLI include/exclude policy
  -> one captured real host drive snapshot
  -> host namespace handles for admitted X:\ roots
  -> bx-vdm OpenNT-derived DEM/COMMAND providers
  -> checked guest-memory transaction
```

The BYOB images are startup payloads in guest RAM. They are never copied to a
host drive as DOS boot files.

## Existing reusable seam

`bx_ntvdm_host_namespace_v1` opens the actual `X:\` roots selected by
`bx_ntvdm_host_drive_snapshot_v1`, retains handles adapter-locally, enumerates
with ordinary user-mode APIs, rejects reparse points, projects deterministic
8.3 entries, and never exposes host handles to guest memory. S30 binds it to
`bx_ntvdm_composition_runtime_v1` at admission and releases it before the
composition image/provider state is reset.

This establishes the capability lifetime; it does not by itself claim that
every DEM/COMMAND host-filesystem provider is already connected. That work is
an owner-package result of the succeeding complete BOP audit.

## OpenNT source obligations

| Family | Original owner | Real-host action | Read-only profile result |
| --- | --- | --- | --- |
| `50:13` query current directory | `demdir.c:demQueryCurrentDir` | Verify projected drive/path and repair DOS CDS to a real root when needed | Read-only query succeeds only for an admitted real drive. |
| `50:18` set current directory | `demdir.c:demSetCurrentDir` | `SetCurrentDirectoryOem` plus per-drive `=X:` session state | Admit real selected path; preserve source failure through typed result. |
| `50:1A` set default drive | `demgset.c:demSetDefaultDrive` | Validate `DL`/path, set CLI/VDM session CWD with the corresponding API | Admit real selected path; preserve mismatch/API failure. |
| open/read/search/enumeration | `demfile.c`, `demfcb.c`, `demdir.c` | Selected real host directory, no ambient rescan | Source-derived read/search result only. |
| create/delete/rename/attributes/time/dir mutation | `demfile.c`, `demfcb.c`, `demdir.c`, `demhndl.c` | Historical path calls Win32 mutation APIs | One common explicit read-only policy produces source-derived refusal; no service is silently omitted. |

## Admission rules

1. The default profile requires its selected startup drive to be present in
   the captured, policy-filtered real-host snapshot. If it is excluded (for
   example C: is excluded), direct composition rejects before CPU execution.
2. `--virtual-boot-volume=c` is reserved as an explicit, default-off fallback
   capability. It is valid only when host C: is excluded and has independent
   namespace, current-directory and mutation-failure tests. It must not alter
   a profile where real C: is admitted.
3. Host directory and registry operations are ordinary process-scoped API
   calls. They are not installation, system-file modification, or a
   `ntvdm64`-style host reconstruction dependency.

## S30 evidence and explicit follow-on

- Direct composition now rejects a profile whose declared command/startup
  drive is absent from the policy-filtered real-host snapshot, before CPU
  execution.
- `bx_ntvdm_host_namespace_v1` now participates in direct-composition
  initialization/reset and is compiled by the current-composition manifest.
- Strict MSVC x64 `/MT` fixture `t200-s30-host-namespace-unit-r5` proved
  real-root enumeration, deterministic 8.3 projection, nested lookup and
  `..` refusal. It must run outside the filesystem sandbox because it opens a
  real host root; it made no host mutation.
- Current-composition source rebuild `t200-s30-current-composition-r7`
  compiled and linked the complete minimal fixture including the new host
  namespace source and `ntdll.lib`, then reached its existing bounded
  observation terminal. The fixture process returned its declared terminal
  code `2`; the probe script accepted that assertion and completed with exit
  code zero. No unbounded guest execution occurred.

The succeeding whole-BOP audit must classify, by original owner package, the
remaining current-directory/path/search providers and every mutation-family
service before any further implementation. The optional virtual fallback is
retained as a separate feature contract and is not implemented by S30.
