# Shard 1: original sample2 VDD and 16-bit companion

The three files below are `待核验` in the current shard ledger and match both frozen OpenNT editions. The observer verifies their hashes, original sample build inputs and four normalized host Ninja VALID dependency sets. It does not compile, launch or install the sample.

| Pending file | Original role and build relation |
| --- | --- |
| `vdd/samples/sample2/16bits/16bits.asm` | Original DOS sample application that drives sample I/O, memory and DMA and hooks a DMA completion interrupt. The original 16-bit makefile builds `16bits.obj` and links `16bits.exe`; actual execution is unproved. |
| `vdd/samples/sample2/16bits/16bits.inc` | The sample's guest port, memory and DMA constants; `16bits.asm` includes it and the sample makefile lists it as an object dependency. |
| `vdd/samples/sample2/vdd/vdd.def` | Definition for the separate sample VDD DLL exporting `VDDInitialize`; its `sources` specifies `TARGETTYPE=DYNLINK`, the same entry name and `vdd.c`. The exact BUILD tool's definition-file resolution was not replayed. |

The separately sourced VDD sample's attach path allocates resources and installs I/O/memory hooks; detach conditionally removes those hooks, frees guest mapped memory when present, and destroys the heap. These are sample source observations, not proof of project VDD provider correctness or exceptional rollback. The observer compares six numeric constants shared by the guest `.inc` and sample VDD `vdd.h` (I/O first/last port, memory segment and first/last offset, and DMA channel): all match. It does not prove computed expressions, physical mappings, interrupt numbering, DMA semantics or cross-process lifetime.

The three exact paths have no VALID dependency hit in the four checked x86 worker/WOW32 and x64 frontend/service trees. `assets/winnt.zip` has no member ending `/16bits.exe`. These observations support candidate **historical sample source/definition roles for the checked production builds only**; they do not exclude the project VDD APIs, other samples, external deployment or a guest feature. The VDD source's own lifecycle must not be confused with current product ownership merely because the names match.

Replay `python artifacts/mvdm-audit/shard-1/vdd-sample2-owner-observer.py`. No compiler, sample, product, guest or provider ran; no guest, product, build tree or shared ledger was modified. Source-to-image provenance and all functional behavior remain unverified.
