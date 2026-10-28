# Shard 1: historical audio VDD sample source roles

The two source files are `待核验` in the current shard ledger, and both they and their corresponding original `sources` recipes match the two frozen OpenNT editions. The observer hashes each, checks exact source path mentions in four current host Ninja graphs, resolves each VALID dependency against its graph directory, and performs a bounded release/ZIP name census. It does not run either sample or touch audio devices.

| Pending source | Original component and source-level lifecycle |
| --- | --- |
| `vdd/samples/adlibvdd/vdd.c` | Separate `ADLIBVDD` dynamic-library sample using NTVDM VDD services and an Ad Lib kernel-driver interface. Its recipe names `vdd.c`, `TARGETTYPE=DYNLINK` and `DllEntryPoint`. Source attach attempts to install port hooks; visible detach removes hooks and closes its device handle. Actual driver availability, failed attach and concurrent shutdown are unverified. |
| `vdd/samples/vsbd/vdd.c` | Separate `VSBD` dynamic-library Sound Blaster sample, with WinMM among its declared libraries. Its source searches for wave devices, tries multiple I/O port ranges, and visible process detach closes wave state, unhooks ports and deletes the critical section. Failure, callback, DMA and concurrency behavior are not proved by this source reading. |

Neither exact source path appears in the four checked x86 worker/WOW32 and x64 frontend/service graph text or normalized VALID dependencies. The exact DLL names were absent from `assets/release`, `O:/winnt/system32`, and ZIP basename census at observation. The original VDD-loading path accepts externally configured components, so this bounded absence does not prove that no external sample could be loaded elsewhere. These two paths are candidate historical independent sample-source roles for the inspected build only; do not exclude general VDD, Ad Lib or sound support or infer that another audio implementation is equivalent.

Replay `python artifacts/mvdm-audit/shard-1/vdd-audio-samples-observer.py`. JSON includes source, recipe, ledger and graph hashes. No guest, product, sample, compiler or audio provider ran; no guest, product, build tree or shared ledger changed. Original sample deployment, runtime behavior and source-to-binary provenance remain unverified.
