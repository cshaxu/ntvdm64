# T221 S2 P2: 29-service DEM whole-provider disposition

## Package boundary

The implementation unit is one `dem_whole_provider` state owner with five
source-owner subfamilies—not five independent BOP projects:

| Subfamily | Identities | Original owners | Provider/backend decision |
| --- | --- | --- | --- |
| Namespace/path | `01,03,04,05,06,12,17,22,44` | `demfile.c`, `demdir.c` | One checked path/view resolver and opaque open/create result. Direct roots use host namespace; readonly uses declared/readonly view; overlay and virtual need their own view backend. |
| Handle I/O | `00,02,08,16,1E,27` | `demhndl.c` | One backend-kind-aware token table. Close/seek/read/write/time/commit must validate token generation and kind before any backend action. |
| Pathname/FCB search | `09,0A,0B,0C` | `demsrch.c` | One continuation owner; pathname and FCB forms retain distinct DTA/SRCHBUF byte layouts and first/next failure rules. |
| FCB file state | `07,20,2C,2D,2E,2F,30,31` | `demfcb.c` | One FCB adapter behind the same view/token owner. Wildcard path operations, FCB token state and FCB I/O remain distinct operations but share policy and lifetime. |
| Pipe EOF | `47,48` | `demhndl.c` plus COMMAND/VDMREDIR | Explicitly unavailable in this package. A later Redirector provider alone may introduce a pipe-token kind. |

This is exactly 29 identities. `dem_whole_provider` remains the sole holder of
file session, view resolver and search state; `dem_package_session` remains
the BOP routing/gather coordinator only. No new selector logic enters
bx-core/bx-mantle.

## Backend contract

Every operation first resolves a **shared mutation-profile result** and then
chooses the matching provider view. The operation does not decide its own
mode.

| Shared mode | View semantics | Token/search semantics | Required negative rule |
| --- | --- | --- |
| Direct | Selected real host drive roots, ordinary user-mode Win32 APIs, normal ACL/share errors projected through the source error contract. | Private token owns a direct file handle; continuation enumerates the selected root. | Host path, share, ACL and stale-token failures must not fall through to readonly. |
| Readonly | Immutable declared images plus admitted read view. | Read tokens/search are view-specific and mutations refuse according to the original source-shaped result. | No host write, metadata mutation, or direct fallback. |
| Overlay | Package-owned copy-on-write/tombstone namespace over an admitted base view. | Token identifies overlay/base object; search merges overlay and base without host pointers. | No direct host mutation; no separate per-service overlay hacks. |
| Virtual | Explicit synthetic boot-volume view only when the selected profile asks for it (including the approved excluded-host-C case). | Tokens/search belong to the virtual view; startup images are inputs, not host C: writes. | No implicit virtual C: and no ambient host-drive fallback. |

The current implementation supplies only the Direct and part of Readonly
columns. Overlay and Virtual are intentionally unavailable today; S3 must add
a common view/backend seam before claiming either profile.

## Existing-code migration

| Existing area | Decision in the whole provider |
| --- | --- |
| `dem_file_session_v1`, `dem_local_file_backend_v1` | Retain as the direct token backend, but extend token kind/lifetime only through one package-local interface. |
| `dem_namespace_*`, `dem_handle_*`, `dem_fcb_*`, `dem_search_*` partitions | Migrate behind the single provider view contract; their service membership remains a guard, not independent policy. |
| `readonly_namespace`, declared snapshot, boot namespace fixtures | Retain only as Readonly/Virtual view inputs and test fixtures; never as an unlabelled direct-host fallback. |
| `dem_cli_unavailable_provider` after a partition declines | Replace for the 27 non-pipe services with provider-owned source-shaped failures. Retain pipe unavailable only for `47/48`. |
| `release_owner(PDB)` | Retain as a diagnostic/compatibility utility only; do not bind it to `50:3C` or rebuild guest JFT/SFT. |

## Family regression matrix

The later implementation S must have one positive and one negative check per
subfamily and profile—not one happy-path test per BOP:

| Subfamily | Positive family check | Negative family check |
| --- | --- | --- |
| Namespace/path | Direct open/create/rename/delete and readonly image open through the one resolver. | Cross-root rename, denied share/ACL, readonly mutation, overlay/virtual no-direct-fallback. |
| Handle I/O | Token open → seek/read/write/time/commit → close with source register result. | Stale/wrong-kind token, token after close, PDB ownership mismatch, zero-length/truncate and failure mapping. |
| Search | Path and FCB first/next write exact DTA/SRCHBUF forms. | Altered/stale continuation, no-more, FCB vs pathname form mismatch, overlay collision ordering. |
| FCB | Wildcard delete/rename and FCB open/I/O/date/info preserve selected-view records. | Malformed path/record, no-match, invalid FCB token and partial-write source form. |
| Pipe | Both identities retain the source-labelled unavailable result. | A local file token must never be accepted as a pipe token. |

The existing `bx_ntvdm_dem_whole_provider_v1_test.c`, token, local backend,
search partition, and profile tests are starting fixtures only. The final
family regression must instantiate Direct, Readonly, Overlay and Virtual
through the same provider API and prove their mutual exclusion.

## Implementation ordering

1. Introduce the provider view/backend abstraction and backend-kind token
   validation without changing BOP routing.
2. Move existing Direct and Readonly subfamilies through that abstraction and
   make their family tests pass unchanged in meaning.
3. Add Overlay and explicit Virtual backends with their negative isolation
   tests; do not expose them through endpoint-local conditions.
4. Replace the 27 non-pipe generic fallback outcomes with provider-owned
   source-shaped failure behavior, retain pipes deferred, then run the complete
   29-service regression.
5. Only after package regression closure, admit one native trace as integration
   evidence.
