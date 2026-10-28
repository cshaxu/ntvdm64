# DOS utility and COMMAND intermediate identities — shard 3

These three files were selected from current shard 3 pending rows. `dos-utility-image-observer.py` and captured JSON compare exact SHA-256, size and first bytes across the source tree, both reference editions, archive members and explicitly checked installed paths. No guest image was modified, decompressed, built or executed; these guest binaries are not Ninja host objects, so dependency-path normalization is not applicable to their identity checks.

| path | proposed status | evidence and boundary |
|---|---|---|
| `dos/v86/cmd/backup/backup.exe` | pending | 22,966 bytes with `KWAJ` signature, matches OpenNT 4.5; corresponding OpenNT path absent. Historical backup/makefile:10-11 binplaces it without showing how it was produced. No exact BACKUP.EXE member was found in the archive or checked installed root/system32. Do not assume these bytes are a directly executable MZ image, or that other media lack backup capability. |
| `dos/v86/cmd/edlin/EDLIN.EXE` | pending | 12,642-byte MZ image, matches both originals. Historical edlin makefile builds/link16s EDLIN.EXE from ASM objects and generated message controls. No exact EDLIN.EXE member in the archive or checked installed root/system32; source-to-image provenance, alternate locations and editor execution remain unproven. |
| `dos/v86/cmd/command/command.exc` | excluded, historical intermediate file role only | 51,152-byte MZ image matches both originals. COMMAND makefile:514-536 links command.exc, copies to temporary .exe and uses reloc to create COMMAND.COM. Archive and installed system32 COMMAND.COM match each other (50,384 bytes, SHA-256 `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43`); no command.exc member found. This recipe does not prove the retained EXC generated the published COM. Exclusion does not apply to COMMAND runtime behavior. |

Archive name searches cover all member paths for the listed basenames; installed checks cover only `O:/winnt` and `O:/winnt/system32`. File identities establish neither execution nor compatibility; no overall restoration percentage is inferred.
