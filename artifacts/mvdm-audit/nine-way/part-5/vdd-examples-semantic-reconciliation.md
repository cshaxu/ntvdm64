# Part 5: VDD examples versus project VDD contract

This supplements the individual hashes and package scan in `vdd-samples-family-review.md`; MSCDEX is handled in its own deeper report and must not be discarded as an example. Original `vdd/samples/readme` describes SAMPLE1 as guest application/device interception and SAMPLE2 as NTVDM I/O/memory interception. Actual project original `softpc.new/host/src/nt_bop.c:899` owns guest RegisterModule; `nt_vdd.c:269,403` implements VDD port-hook API. A sample DLL is neither the host VDD service nor proof all VDDs work.

Original AdLib `adlibvdd/vdd.c:181,428-447` opens its particular native synth device, returns FALSE if IO hook install fails, and on DLL detach removes the hook and closes its device. SAMPLE2 `sample2/vdd/vdd.c:104-123` installs independent I/O and memory hooks and conditionally removes them at detach before freeing memory and heap. Those are source contracts for separate examples, not proof their devices or underlying project hook teardown were tested. `hpscan16.asm` is a DOS device caller that requests HPSCAN32.DLL; its `.sys` recipe uses `exe2bin` on its own `.exe` output. FAX16 explicitly says unimplemented command codes always succeed, demonstrating why this sample must not be treated as a complete fax service.

| Owned path | Concrete role and boundary |
| --- | --- |
| `vdd/samples/adlibvdd/adlibvdd.def` | Original sample DLL export definition; no selected product provider inferred. |
| `vdd/samples/adlibvdd/vdd.c` | Original AdLib sample port hook and specific synth device lifetime; source-only. |
| `vdd/samples/adlibvdd/vdd.h` | Its original device protocol declarations. |
| `vdd/samples/hpscan/hpscan16.asm` | Original sample guest device request/registration stub; actual HPSCAN32 service unproved. |
| `vdd/samples/hpscan/hpscan16.inc` | Original private guest device structures/commands. |
| `vdd/samples/hpscan/hpscan16.exe` | Current-only intermediate-shaped binary; historical recipe uses .exe to create .sys but its exact provenance is unproved. |
| `vdd/samples/hpscan/HPSCAN16.SYS` | Current-only guest sample binary; package scan has no exact ZIP hit; actual selection remains pending. |
| `vdd/samples/readme` | Original documentation for sample1/sample2; not device implementation. |
| `vdd/samples/sample1/fax16/fax16.asm` | Original guest fax demonstration, deliberately incomplete. |
| `vdd/samples/sample1/fax32/fax32.c` | Original sample fax host counterpart; does not establish a working fax product. |
| `vdd/samples/sample1/fax32/fax32.def` | Original sample module exports. |
| `vdd/samples/sample1/fax32/fax32.h` | Original sample declarations. |
| `vdd/samples/sample2/16bits/16bits.asm` | Original test guest, including test vector setup; not a general DOS app implementation. |
| `vdd/samples/sample2/16bits/16bits.inc` | Original test device ranges/contract. |
| `vdd/samples/sample2/vdd/vdd.c` | Original host I/O and memory-hook demonstration, separate state and cleanup. |
| `vdd/samples/sample2/vdd/vdd.def` | Original sample DLL export definition. |
| `vdd/samples/sample2/vdd/vdd.h` | Original demonstration declarations. |
| `vdd/samples/vsbd/vdd.c` | Original Sound Blaster sample source, using VDD IO hooks; host device and completeness not proven. |
| `vdd/samples/vsbd/vsbd.def` | Original sound sample module definition. |
| `vdd/vddserv.doc` | Historical VDD API description; does not replace current production entrypoints. |

These are evidence/classification recommendations; standard product does not link these C sources according to `m0-t473-s1-unselected-host-source-disposition.md`, but alternate loads remain unproved. S17 should validate actual project RegisterModule/dispatch, I/O and memory hook install conflict, partial-failure rollback, unhook, process/task teardown and callback after provider unload through controlled representative fixtures; S13 owns real hardware semantics when relevant; S19 binds to release. Do not copy sample success stubs, claim physical devices, or import unrelated drivers. No runtime test or guest/product/shared-ledger modification in this review.

Replay `rg -n "RegisterModule|VDDInstallIOHook|VDDDeInstallIOHook|VDDInstallMemoryHook|VDDDeInstallMemoryHook|CloseHandle" src/mvdm/vdd/samples src/mvdm/softpc.new/host/src/nt_bop.c src/mvdm/softpc.new/host/src/nt_vdd.c`, inspect corresponding makefiles and part-5 observer JSON.
