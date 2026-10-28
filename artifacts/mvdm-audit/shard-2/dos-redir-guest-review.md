# Shard 2: DOS redirector guest identity and source boundary

Two pending rows were inspected against both original trees, historical build declarations and the release ZIP. Their identities and context hashes are captured in the companion read-only observer and JSON.

| Entry | Evidence and decision |
| --- | --- |
| `fudgedef.inc` | Matches both originals; defines several constants. No active consumer or selected configuration was established in this review. Retain pending; a source comment or same-directory placement alone is not proof of use. |
| `obj/redir.exe` | Current-only: absent from both original trees and frozen catalogs. The original makefile names `$(OBJPATH)\redir.exe` as a linked, binplaced output, but that does not establish this particular binary's provenance. Retain pending as a supplemental guest image. |

The current-tree `obj/redir.exe` is `F4BE4EA0476971E3C702C5D4B4857664C0092205C2C64295313812297A33240A`. ZIP `WINNT/SYSTEM32/REDIR.EXE` and installed `O:/winnt/system32/REDIR.EXE` instead match each other at `B6E9FAD30A5423EAD9ECB45C8E28197EA62A39187D36241F9018DB82FACAC3A7`. They must not be conflated. No assertion about which binary a guest loads in a given session follows from package presence.

Original `redir.asm` includes the redirector service declaration; `rdrsvc.h` describes BOP 57h and individual services. This identifies a guest/host interface, not successful live dispatch, initialization, named-pipe or redirector lifecycle support. Notably, the inspected initialization SVC line is commented in that source, so it must not be presented as an active call. Prior `../shard-1/dos-core-guest-owner-review.md` records other historical object inputs. Guest-to-host behavior, installed-image provenance and BOP implementation remain separate questions.

Replay `python artifacts/mvdm-audit/shard-2/dos-redir-guest-observer.py` and compare its JSON. No guest, product, installed file, shared ledger or build tree was executed, modified or rebuilt.
