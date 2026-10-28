# DOS/v86 MEM and SETVER: original target role candidates

This read-only review addresses the DOS/v86 portion of the earlier 473 role-pending C/H census, **not** a complete or current ledger reclassification of all 473. [Observer](dos-v86-target-current-observer.py), [per-file CSV](dos-v86-target-current-inventory.csv), and [receipt](dos-v86-target-current-evidence.json) identify 13 bounded guest-target physical-file candidates. The input is the earlier 225-path round-three/round-four unresolved snapshot, not the current live shard status; update decisions must rejoin the current authority. The receipt's remaining 212 count covers that whole 225-path cross-tree snapshot, not just DOS/v86.

Original and current `dos/v86/cmd/mem/makefile` are byte-identical and link `mem.exe` with `link16`, explicitly listing each candidate C object and its local header prerequisites. The `setver` makefile likewise links `setver.exe` using `link16`; its rule copies `$(COUNTRY)\message.h` into the root before `setver.obj` uses it. In the recorded USA-MS case the `usa-ms/message.h`, generated-position root `message.h`, and original source bytes match. This proves a source/recipe relationship, not that an installed immutable guest binary came from this tree.

| Exact relative paths | Historical target and path-level role |
| --- | --- |
| `dos/v86/cmd/mem/{mem.c,membase.c,memctrlc.c,memex.c,submsg.c}` | C sources of named MEM.EXE guest objects. |
| `dos/v86/cmd/mem/{mem.h,msgdef.h,xmm.h}` | Explicit local prerequisites of named MEM.EXE guest objects. |
| `dos/v86/cmd/setver/{parse.c,setver.c,setver.h,message.h}` | Named SETVER.EXE guest object sources and prerequisites. `message.h` is the generated-position copy. |
| `dos/v86/cmd/setver/usa-ms/message.h` | USA-MS input copied by that recipe to the SETVER guest build's root `message.h`; other country selections require their own evidence. |

All 13 match their original source bytes. For the captured x86/x64 worker and x86 WOW32 graphs, the observer checked graph text, normalized recorded exact Ninja dependencies, and non-guest C/H lexical same-basename includes under `src` and `tools`; no conflicting host use was found for these candidate paths. All four retained dependency logs report zero stale blocks. This supports a **guest-target candidate outside the observed host C/H path set**, not an unconditional guarantee about generated/external, unbuilt or future host consumers. The C/H scan deliberately excludes `dos/v86` and `wow16` guest trees; dual-use dependencies still require a separate check. Do not delete source, alter guest images or exclude MEM/SETVER functionality from the T; S12 and future guest-owner work retain those obligations independently.

Source building tools such as `dos/v86/tools/src/buildidx/buildidx.c` and COMMAND's `buildmsg.c` are not included: an original guest-build helper can itself be a host tool and needs separate target/consumer proof. Generic standard library headers and unproven alternate paths remain pending. No guest was built or executed.

Replay at repository root: `python artifacts/mvdm-audit/host-ch-scope/dos-v86-target-current-observer.py`. It needs the pinned OpenNT reference and four populated Ninja build trees and invokes only `ninja -t deps`. Two consecutive observations at the recorded input hashes yielded receipt SHA-256 `795D673BDD01E639E385E8C9630A05A80C73A240452121672037EE3D27EE5370` and CSV SHA-256 `8584A9E66E5927C6945AC4AB95A6A9E3AF5B62D49A9E1E857DFFE11CB7DA4145`. Future shard, source or graph changes require a fresh snapshot and reconciliation.
