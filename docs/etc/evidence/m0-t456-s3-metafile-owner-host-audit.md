# M0 T456 S3 — classic metafile owner and public-host audit

## Decision

No production repair is admitted. The formal x86 graph directly compiles
`src/mvdm/wow32/wgmeta.c` into `wow32.dll`; no project metafile adapter,
registry, renderer or copied provider exists. Its selected original algorithms
remain compatible with the public host APIs they call.

`GETMETAFILEBITS`, `SETMETAFILEBITS` and `SETMETAFILEBITSBETTER` are marked
`LOCALAPI` in `wgtbl2.h` and their WOW32 bodies are deliberately disabled in
the original source because guest `GDI.EXE` owns those operations. Replacing
that split with a host bridge would create a second metafile authority, so S3
does not do it.

## Ownership and paths

| Route | Original authority | Public-host operation | Normal / failure / release rule |
| --- | --- | --- | --- |
| Create / close | `WG32CreateMetaFile` creates the native recording DC; `WG32CloseMetaFile` closes it and calls `WinMetaFileFromHMF`. | `CreateMetaFile`, `CloseMetaFile`, `GetMetaFileBitsEx`. | A close failure returns zero. On success the original converts bits to a movable guest global handle and deletes the temporary native `HMETAFILE`. |
| Guest bits to native handle | `HMFFromWinMetaFile`. | `SetMetaFileBitsEx`. | The original locks guest memory, builds a temporary native handle, then unlocks or frees according to the caller's `fFreeOriginal` transfer. |
| Copy / load / playback | `WG32CopyMetaFile`, `WG32GetMetaFile`, `WG32PlayMetaFile`. | `CopyMetaFile`, `GetMetaFile`, `PlayMetaFile`, `DeleteMetaFile`. | Copy/load returns a guest global handle only after the original bit copy succeeds; all temporary native handles are deleted after use. |
| Enumeration and record playback | `WG32EnumMetaFile`, `WG32EnumMetaFileCallBack`, `WG32PlayMetaFileRecord`. | `EnumMetaFile`, `PlayMetaFileRecord`. | Stack-local metadata bounds `rdSize`, copies guest record/object-table state before and after the synchronous callback, and frees both guest temporary allocations on every `EMF_Exit` path. |
| Delete | `WG32DeleteMetaFile`. | Guest global-handle validation/free; DDE receives its original `W32DdeFreeHandle16` notification. | Valid headers are released once. This is not a native handle registry. |

The only modern-host objects are the temporary `HMETAFILE`/recording DC and
public GDI calls. Guest global handles, callback buffers, compatibility flags
(`WOWCF_GETDUMMYDC`) and DDE release stay under original owners. The callback
is synchronous: no pointer, object table or stack `METADATA` survives
`EnumMetaFile` returning.

## Public-host capability witness

`tests/adapter-mvdm-host-out/wow/wow_metafile_host_fixture.c` is intentionally
not an extraction of `wgmeta.c`. It proves the actual modern public APIs used
by the retained original body are available and coherent: an x86 `/MT` run
creates a classic metafile, gets and sets its bits, copies and reloads it,
plays it into a bitmap, enumerates records, and confirms zero-bit restore and
null playback fail. The checked-in entry
`tests/observation/verify-wow-metafile-host.ps1` recorded at
`build/t456-s3/metafile-host-r3`:

```
WOW_METAFILE_PUBLIC_HOST_OK
```

The fixture does not claim full guest application acceptance or replace the
existing immutable-guest frontiers. It closes the finite question whether this
selected original provider needs a modern private-GDI or project-owned
metafile substitute: it does not.

## S4 handoff

S4 owns repeated package-level drawing/release and retained application
frontiers. Its scope may add no metafile engine, guest mutation or duplicate
handle state; it must use `wgmeta.c` and the original GDI local APIs above.
The known private-desktop early exits recorded in S2 remain non-passes, not
evidence about this source route.
