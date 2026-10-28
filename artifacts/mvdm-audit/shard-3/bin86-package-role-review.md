# Shard 3: six original guest package candidates

The [observer](bin86-package-observer.py) and
[observations](bin86-package-observations.json) reproduce source identities,
both source catalogs, every archive member with identical bytes, and each
direct `O:/winnt/system32` file as observed. The ZIP hash matches its guest
manifest. This is read-only, point-in-time evidence; it does not establish a
worker's search path or identify dynamically loaded modules.

| Original path | Proposed | Package observation and remaining question |
| --- | --- | --- |
| `bin86/config.nt` | pending | Original bytes match ZIP `config.nt`; the user's direct installed config exists but differs. Determine effective per-launch profile and preserve its user-owned configuration. |
| `bin86/i386/win87em.dll` | partial | Original 4.5-only bytes match ZIP `WIN87EM.DLL`; the direct installed system32 file is absent. Its selected loader path and x87 emulation behavior remain open. |
| `bin86/lanman.drv` | partial | Both originals match ZIP `LANMAN.DRV`; direct installed system32 path is absent. Determine if and when network consumers actually load it; no network capability is ruled out. |
| `bin86/olecli.dll` | partial | Original 4.5-only bytes match the ZIP and direct installed `OLECLI.DLL`. WRITE statically imports OLECLI, but identity is not proof of OLE operations or callback lifetime. |
| `bin86/ver.dll` | partial | Original 4.5-only bytes match ZIP `VER.DLL`; direct installed system32 path is absent. Actual guest loader resolution, version API behavior and alternate search paths remain open. |
| `bin86/write.hlp` | pending | Both original bytes match the mirror, but no exact same-content ZIP member or direct installed system32 file was observed. Determine intended WRITE Help delivery and consumer search before classifying. |

Partial describes a retained package candidate, not a passed guest feature.
Exact original text/binary identity is 100% for the available original
edition(s); no complete restoration or execution percentage is inferable.
The absent direct paths do not prove an application fails: dynamic search,
application directories, already loaded modules and alternate installations
remain unobserved. `config.nt` is user state and must not be silently replaced.
No guest, installed files or product image changed.
