# M0 T345 S3 — Ordinary DEM local-contract and paired formal closure

## Local-contract evidence

The original DEM source owns ordinary filesystem results. The selected package
was rechecked as a whole instead of adding leaf fixtures:

- File/open/create/delete/rename, handle I/O/seek/time/commit, directory and
  drive/volume operations, normal search and FCB search remain defined in the
  original `dos/dem` provider files.
- `demdir.c`, `demdisp.c`, `demerror.c` and `demmsg.c` are byte-identical to
  their OpenNT baseline. The other selected bodies carry only the registered
  `MVDM-HOST-DIV-002/003/005/006/007/103/104/105/154` bindings; the file-local
  comments and `src/mvdm-host/README.md` jointly identify every such change.
- The selected mapping manager is the only route that represents a native
  handle or a retained guest location. No raw x64 handle/pointer is placed in
  a DOS register, DTA, PDB or SFT field. The adapter does not decide DOS error
  translations.
- Named-pipe/Redirector and raw-media calls stay outside the cohort and retain
  their distinct owner transfers. Hence ordinary-file capability is not used
  to claim a pipe, device or physical-media result.

## Paired formal build

On 2026-09-01, after initializing the matching existing MSVC environment:

```text
VsDevCmd -arch=x86 -host_arch=x64
ninja -C build/M0-T343/S3/record-x86 \
  original-softpc-process.exe original-softpc-forced-closure.dll

VsDevCmd -arch=x64 -host_arch=x64
ninja -C build/M0-T343/S3/record-x64 \
  original-softpc-process.exe original-softpc-forced-closure.dll
```

Both graphs built the original SoftPC resource input and linked
`original-softpc-process.exe`. The selected `original-mvdm-dem.lib` is an
explicit input to the formal candidate, process and forced-closure targets in
both graphs.

## Boundary of the claim

The result proves current source-shaped local contracts and paired formal
linkage. It does not claim a native guest execution, raw media, named pipe,
Redirector, child/PSP return or presentation-window result.
