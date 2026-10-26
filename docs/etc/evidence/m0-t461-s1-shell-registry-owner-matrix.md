# M0 T461 S1 — Shell and layered Registry owner matrix

## Question

Which selected Shell and layered-Registry paths are presently composed, who
owns their original behaviour, and what exact finite work belongs in T461 S2
without creating a second shell, Registry provider, scheduler, or host
Registry writer?

## Inputs and procedure

This source-only admission review used the checked-in selected OpenNT mirror,
the current x86 composition sources, `proposal-wow32-shell-registry-001.md`,
the WOW32 minimum-diff governance record, and the inherited Shell callback
witnesses.  It did not alter guest media, installed packages, the host
Registry, or runtime processes.

The review followed each admitted export/table entry to its original body,
then recorded its Guest/WOW/host ABI, current adapter, normal/failure/retire
owner and proof entrypoint.  Lexical matches outside the selected families
were classified rather than silently absorbed.

## Selected Shell surface

| Entry and original owner | Production path and authority | Failure/retirement rule | Existing proof and S2 boundary |
| --- | --- | --- | --- |
| `SHELL` ordinal 20 → `WS32ShellExecute`, `src/mvdm/wow32/wshtbl2.h` and `wshell.c` | Original thunk maps five 16-bit strings and the HWND, calls `WOWShellExecute`, and retains original `W32ShellExecuteCallBack`. The registered `wow32-dll` callback adapter only preserves the two-argument synchronous callback scope. The original callback updates the DOS directory, owns `malloc16`/`free16`, and enters guest `RET_WINEXEC` (55). | Argument mappings are released on every original return. A callback failure or exception remains the existing W01 callback/dispatch owner; no new launcher or shell resolver may be introduced here. | `tests/observation/verify-wow-shell-callback.ps1` proves modern-provider success/failure, nesting and callback-scope unwind; `verify-wow-mine-gameplay.ps1 -ShellExceptionSibling` is the retained real original callback/guest-return witness. Neither proves arbitrary Shell associations. S2 may bind only the existing modern shell service behind this original body. |
| `SHELL` ordinal 21 → `WS32FindExecutable`, `wshtbl2.h` and `wshell.c` | Original thunk maps filename, directory and output buffer, calls host `FindExecutable`, flushes the returned guest buffer, and converts only results above 32 to `HINSTANCE16`. | Original mapped-string cleanup and output flush own normal and error exit. The host API remains the capability provider; it is not a licence to create an executable-association database. | No current focused real-guest `FindExecutable` witness was found. S2 must add normal, <=32 error and output-buffer proof through the original thunk before claiming this row implemented. |

## Layered Registry surface

| Owner / caller cohort | Present authority and composition | Normal, failure and retirement ownership | Disposition |
| --- | --- | --- | --- |
| `src/ntvdm-exe/softpc/mvdm_shadow_registry.c` and header | This is the only project-owned `NTVDM.REG` façade. It loads the product-local file once per worker, returns opaque registered key objects, and supports selected `OpenKeyEx`/query/info/enum/close plus the selected native `NtOpenKey`/enumerate/close readers. Its host snapshot is root-allowlisted and read-only; it never returns a native Registry handle to original callers. | `mvdm_standalone_worker` initializes it after package-root validation and shuts it down before worker teardown. Key registration/unregistration owns opaque handle lifetime; foreign/stale handles are rejected. Missing/malformed/oversize file values return existing errors. | Retain as the single Registry mechanism. It lacks create/set/delete, tombstones, atomic persistence and cross-worker serialization; those finite omissions are T461 S2/S3 work, not a reason to add another provider. Host Registry writes are prohibited. |
| Original selected reader cohort: `cmdexec.c`, `cmdkeyb.c`, `demgset.c`, `config.c`, `nt_msscs.c`, `vxd.c`, `ntvdm.c`, `wkman.c`, `wow32.c` | These translation units include the façade and preserve their original read/caller error branches. The façade's allowlist keeps unreviewed callers from becoming host-Registry readers. | Original callers own their buffers and fallback; façade owns its key/snapshot storage. Worker cleanup owns final façade shutdown. | S2 must preserve those read paths while extending only the one façade's admitted operations. |
| Original Win16 Registry thunks: `WK32RegOpenKey32`, `WK32RegEnumKey32`, `WK32RegEnumValue32`, `WK32RegCloseKey32` in `wkernel.c` | They are selected caller surface but do not include the current façade header, so static composition does not prove that they reach `NTVDM.REG`. | Original guest pointer and return conversion remains the thunk owner. | S2 must explicitly choose a source-shaped binding or record an unavailable result. It may not silently send these calls to a second provider. |
| `mapembed.c` class-registration create/set paths and the guest `wow16/regedit` application | These use Registry write APIs but are OLE/application consumers, not an admitted T461 S1 implementation path. | Their caller-specific OLE/application lifetime is not owned by the façade inventory. | Hand forward the consumers to W18 after T461 defines the shared write/delete contract. Do not claim OLE or REGEDIT acceptance from this audit. |

## Findings and minimum-diff decision

1. Direct reuse is available for both selected Shell thunk bodies and their
   existing callback/guest-return ordering; the smallest present adapter is
   the registered callback-scope binding.  A substitute launcher, association
   cache or callback registry would violate the selected ownership boundary.
2. The existing `mvdm_shadow_registry` is the one valid project-owned Registry
   owner.  It already prevents raw host handles crossing into the original
   callers and writes no system Registry key.  Replacing it with per-caller
   `Reg*` shims or a host-backed parallel store would create competing facts.
3. The missing Registry operations are substantive, not merely exports:
   create/set/delete require a single in-memory overlay, tombstones, atomic
   `NTVDM.REG` persistence, worker-loss cleanup and cross-worker serialization.
   S2 may establish the operation contract and one persistent owner; S3 owns
   concurrency/restart acceptance.  No fixed static artifact proves either.
4. `FindExecutable` has no focused real-guest witness and Win16 Registry
   thunks have no proven façade binding.  They are explicit S2 blockers, not
   hidden fallbacks to the system Registry.

## S2 handoff and limits

S2 owns selected Shell host-service binding and original return/error tests,
plus the one façade's read/create/set/enum/delete/close conversion contract.
It must use the original caller bodies and preserve their buffer, status and
cleanup order.  S3 owns atomic persistence, tombstones, cross-worker
serialization and restart/loss proof.  W18 remains owner of OLE consumers;
W11 remains owner of DDE.

This matrix is static source/composition evidence. It proves neither generic
Shell association correctness nor Registry write/persistence behaviour, and
does not authorize a product publication.
