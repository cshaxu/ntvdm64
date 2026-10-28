# Historical SoftPC host icons and DEF — shard 3

Scope: three currently pending `softpc.new/host/link/{softerr.ico,softpc.ico,softpc.def}`. `softpc-resources-observer.py` and captured JSON compare all three, their historical resource.rc and sources file with both original editions, hash the observed current source/generated RC, DEF and Ninja graph, and normalize the retained worker C dependency snapshot relative to the worker build root. All five historical files match both references. No build, UI, guest or product modification occurred.

Historical `host/link/resource.rc:25-26,29-40` includes SOFTPC.ICO as SOFTICON and SOFTERR.ICO as SOFTERROR, and uses the latter in its error-panel dialog. `host/link/sources:45` lists this resource.rc for the historical resource target. `softpc.def` names a SoftPC Windows application and exports HeartBeat and nt_process_event. This documents the original role, not whether any image was built from these files.

Current observed `tools/build/New-T310OriginalSoftpcNinja.ps1:122,161-162` selects `softpc.new/obj.vdm/resource.rc`, not host/link/resource.rc; Ninja builds generated `softpc-embedded-roms.rc` into `softpc-resource.res`. That generated RC has ERRORPANEL using the standard IDI_HAND rather than the two historical icon names; the current build selects `obj.vdm/obj/i386/ntvdm.def`. `nt_error.c:691` still references ERRORPANEL, so error-dialog behavior is a live separate capability; do not exclude it. Existing `src/ntvdm-exe/softpc/README.md` ADAPTER-SOFTPC-049 and shard-1 host-error-dialog audit explicitly retain its verification boundary. The observed NTVDM map has HeartBeatThread/TerminateHeartBeat from nt_timer.obj, which are not proof of the old HeartBeat export or a complete timer lifecycle.

| file | proposed status | limitation |
|---|---|---|
| `softerr.ico` | excluded, historical alternate resource file only | Referenced by historical RC, not by inspected selected source/generated RC; no pixel or interactive error-dialog acceptance. |
| `softpc.ico` | excluded, historical alternate resource file only | Same resource selection distinction; no assertion that the final product has identical icon or appearance. |
| `softpc.def` | excluded, historical alternate link description file only | Current build script selects distinct NTVdm DEF. Exclusion does not remove timer, event, exports or equivalent behavior from scope. |

The retained C-compiler deps snapshot contained no exact normalized path match for these files; C dependency data does not describe resource compiler includes and is not by itself proof of icon omission. The inspected current and generated RC content provide separate graph evidence, but this review does not bind the generated resource bytes to the newest deployed EXE or verify runtime rendering. No restoration percentage is inferred.
