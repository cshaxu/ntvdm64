# Part 5: selected video trace object and full-screen declaration boundary

Both owned paths are individually original-hash-checked in the part-5 host-input observer. This is a bounded selected-profile review, not global exclusion.

`softpc.new/base/video/ega_trcr.c` is compiled into the captured original video archive, but its actual captured x86 object has no executable section and no mapped function (independent `video-native-role-review.md` image observer). The entire tracing body is conditional on `EGA_DUMP` and non-X86GFX; `gvi.c:293-295` starts it only under EGA_DUMP. If selected in an alternate diagnostic profile, `dump_init` opens a file, installs video-memory handlers and calls `host_applClose`/exit on open failure, and `dump_end` closes the file. The source search of this small reviewed pair does not establish who invokes dump_end or restores handlers. The ordinary actual video provider remains the mapped original `gvi`/`video_io`/C-VID and presentation owners, as established for the image in the video report. This does not mean EGA_DUMP or its failure/lifecycle capability is unneeded globally.

`softpc.new/host/inc/nt_fulsc.h` is a real header dependency for nine captured selected objects, including `nt_timer.obj`. It declares full-screen hardware-state layouts, geometry constants and helper functions. `nt_timer.c:395-412` only invokes CheckForFullscreenSwitch in X86GFX, then calls `host_graphics_tick` and idle accounting; the header's mere presence does not prove that conditional path is enabled. Its 80x50 constants in old declarations do not alone prove current dynamic text-frame capacity or an actual bound. `nt_det.c` and `nt_cga.c` are compiled by the captured graph, but exact conditional expansion and active hardware-state consumers require source/object mapping, not a reference count. Real frames and presentation owners must be inspected separately.

| Owned path | Evidence-backed recommendation |
| --- | --- |
| `softpc.new/base/video/ega_trcr.c` | Pending diagnostic behavior. Captured release object has no code; EGA_DUMP-enabled file/hook lifetime and applicability remain unproved. No overall exclusion. |
| `softpc.new/host/inc/nt_fulsc.h` | Exact selected-object input; classify as original declarations only, do not claim all macros or old hardware fullscreen code are active. No overall exclusion. |

`video-native-role-review.md` records candidate-specific image bindings for ordinary video/BOP10/BOP42 and leaves EGA_DUMP open. S13 should establish actual selected state/geometry consumers and video/presentation ownership for current release, and if diagnostic profile is admitted, test EGA_DUMP file-open failure, handler registration/restoration, reset and close with disposable output. S19 pins the build flags, actual linked image and runtime package. This review found no reproduced current failure and ran no test. No guest, product, shared ledger or build state changed.

Replay: inspect original files and captured Ninja/part-5 host-input observer; `rg -n "dump_init|dump_end|EGA_DUMP" src/mvdm/softpc.new/base/video`, and read the video-native report. Do not enable diagnostics or create trace files in this audit.
