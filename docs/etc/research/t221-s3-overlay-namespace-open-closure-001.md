# T221 S3: Overlay Namespace Open/Create Subfamily Closure

The `demOpen`/`demCreate` source family (`50:03`, `50:12`, `50:22`) now
selects one Overlay namespace backend after the existing checked pathname and
copied-PDB transaction.  It carries access, DOS sharing mode, creation
disposition and create attributes into a private COW file/token.

The full MSVC x64 `/MT` whole-provider fixture proves:

- `50:12` returns an `OVERLAY_FILE` token under a valid copied PDB owner;
- `50:03` truncates only the private object and retains `CX` attributes;
- `50:22` returns `ERROR_FILE_EXISTS` for an existing base;
- a deny-both open rejects a second open with `ERROR_SHARING_VIOLATION`;
- `50:02` closes the resulting typed token; and
- an admitted host file containing `host` still contains `host` after the
  Overlay `50:03` create/truncate transaction.

The final run was:

```text
tools/probe/Invoke-T202S4DemWholeProviderProbe.ps1
  -RepositoryRoot O:\repos.hobby\ntdos64
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\039
```

This closes only Overlay namespace **open/create**.  It does not close the
namespace owner: `01/04/05/06/17/44`, FCB, search, metadata/time, tombstone
semantics, and the explicit Virtual view remain separate package work.
