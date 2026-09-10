# T209 S2 — boot-namespace/configuration construction closure

## Change

The profile-search snapshot now distinguishes payload-required COMMAND/target
entries from declared startup entries.  CONFIG/AUTOEXEC may be consistently
empty before bind (`NULL`, zero length), while malformed `NULL` with nonzero
length remains rejected.  Binding atomically reprojects byte counts into the
existing metadata snapshot and retains only pointers to the destination
provider's owned image arrays.

No BOP route/result, host policy, Bochs source, synthetic startup content or
external fixed-width ABI changed.

## MSVC x64 `/MT` evidence

| Fixture | Result |
| --- | --- |
| `t209-s2-profile-snapshot-r2` | passed: unbound CONFIG/AUTOEXEC metadata entries are zero length; malformed required COMMAND content is rejected |
| `t209-s2-boot-provider-r10` | passed: declared-unbound provider is valid, bind replaces zero lengths with provider-owned image lengths, and guest read returns the bound image bytes |
| `t209-s2-dem-lifecycle-r5` | passed: the dependent `50:3C` PDB search-state owner fixture constructs and completes again |
| `t209-s2-dem-package-r3` | compiled and linked, then exited `197` in the existing `demSetDefaultDrive` / synthetic-CWD path; this is an independent host-context package boundary, not an initialization or lifecycle failure |

The provider fixture also corrected two stale fixture facts exposed during
recovery: `C:\WINDOWS` has ten OEM bytes (not nine), and generated
configuration content begins with provider-generated country/shell text, so
the read assertion now compares against the provider-owned bound image rather
than assuming a `FILES=` prefix.

## Disposition

T209 is closed.  The next owner package is complete CLI host-context
projection, including real selected-drive/CWD semantics.  It must treat the
exit-`197` result as package-level source/ABI evidence, not a request for a
one-service patch.

