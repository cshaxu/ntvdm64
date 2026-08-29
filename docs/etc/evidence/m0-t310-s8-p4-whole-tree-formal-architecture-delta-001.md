# M0 T310 S8 P4 — whole-tree formal x86/x64 diagnostic delta

## Scope

The current formal Ninja graph was executed for both host architectures.  This
is an execution cross-check for the static-review scope only:
`src/mvdm-host` and `src/mvdm-support`.

## Commands and logs

- x64: `build/M0-T310/S8/p1-machine-source/x64/whole-tree-x64-refresh.log`
- x86: `build/M0-T310/S8/p1-machine-source/x86/whole-tree-x86-refresh.log`

Both formal Ninja runs completed. No warning suppression was added.

## Result

The logs contain 301 x64 and 175 x86 diagnostics from the two scoped roots.
The 126 x64-only excess diagnostics are concentrated in these original owner
packages, which determine review order:

1. COMMAND: `cmdexec.c`, `cmdmisc.c`, `cmdenv.c`, `cmdredir.c`, `cmdconf.c`;
2. DEM file/handle/search: `demhndl.c`, `demfcb.c`, `demmisc.c`, `demsrch.c`;
3. XMS: `xmsblock.c`, `xmsumb.c`.

Most x64-only forms are C4311/C4312 pointer-width conversions, C4267 native
size narrowing, C4213 legacy thunk declarations, and C4366 handle/value
conversion. These are source-contract candidates, not permission to silence
or mass-cast them. The next owner-package reviews must distinguish guest
numeric values, stable external identities (mapping manager), private native
storage, and unavailable historical kernel transport.
