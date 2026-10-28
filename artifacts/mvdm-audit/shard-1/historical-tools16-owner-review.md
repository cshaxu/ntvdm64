# Shard 1: pending original 16-bit build-tool files

These 12 `tools16/` paths were `待核验` in the current shard ledger and are not mirrored under `src/mvdm`. The observer freshly checks the original file hashes against the frozen reconciliation: eight executable names are present in the OpenNT-4.5 source edition only, while four error/message data files match both editions. It never executes the original binaries. Per-file identities, catalog IDs and four checked host graph hashes are in `historical-tools16-owner-observations.json`.

| File | Evidence-backed historical role / boundary |
| --- | --- |
| `buildmsg.exe` | A historical COMMAND makefile explicitly invokes `buildmsg.exe` with a locale message file and skeleton to generate message inputs. A separate original `buildmsg.c` build chain was previously recorded; neither makes this particular binary's source lineage certain. |
| `cl16.exe` | Named compiler in original Win16 COMMDLG recipe (`CC=cl16`). Actual resolved executable search path and the archived executable's version are not established. |
| `masm.exe` | MASM is invoked by original DOSX and Win16 recipes. The command name does not prove which executable was resolved in an old build. |
| `h2inc.exe` | `inc/makefile` invokes `h2inc` to generate several assembly includes; the separate original h2inc source makefile identifies a historical producer, not the provenance of this exact image. |
| `rc16.exe` | Original COMMDLG recipe invokes RC16 for resources. Actual historical binary selection and result lineage remain unproved. |
| `stripdd.exe` | Original HIMEM makefile invokes `stripdd himem.bin $(dest)`; a separate source tree exists. Identity of its historical compiled binary remains unproved. |
| `q23.exe`, `rcqq.exe` | Historical executable artifacts. No exact selected recipe or functionality is established in this bounded review; retain pending rather than assigning a role from the name. |
| `c1.err`, `cl.err`, `cl.msg`, `rcqq.err` | Historical error/message-data files. Association with a particular compiler process, language edition or selected runtime is unproved; filename alone is not enough. |

No exact current or either original `tools16` path appears in the four checked x86 worker/WOW32 and x64 frontend/service Ninja graphs, nor in their normalized VALID dependency records. None of these basenames was found as a member of the current `assets/winnt.zip`, or at the exact `O:/winnt/system32` basename. These checks are bounded: commands may resolve a different tool via PATH, other build trees and guest media are not searched, and a command’s textual name is not proof it chose a particular frozen EXE.

The first six EXEs are candidates for historical guest build-tool roles only, and the remaining files require explicit ownership evidence. Do not infer that guest binaries have been rebuilt from them or that guest/compiler functionality can be excluded; current task policy prohibits rebuilding guest.

Replay `python artifacts/mvdm-audit/shard-1/historical-tools16-owner-observer.py`. No tool, compiler, product, guest or provider was executed; no product, guest, build tree or shared ledger was modified.
