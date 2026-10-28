# Shard 2: legacy disk provider and CD-ROM declarations

Two pending files match both original trees. The observer/JSON capture current graph, object sections and a normalized single-object dependency snapshot; no disk or guest was run.

| Entry | Evidence and limited disposition |
| --- | --- |
| `softpc.new/base/disks/gfi_sflp.c` | Original source describes a remote SLAVEPC floppy-controller/DMA provider guarded by `SLAVEPC`. Current worker graph compiles and archives the object, but captured flags do not directly define `SLAVEPC`, the object has no code section and no owner appears in the worker map. **Remain pending**: the remote-provider profile's applicability, transport and runtime contract must be decided; this is not proof the file is absent from the build or that local floppy behavior is missing. |
| `softpc.new/base/inc/cdrom.h` | Original header defines CD-ROM packet/device declarations. Its exact current consumer and profile were not established; it is not in the one captured `gfi_sflp.obj` dependency list. **Remain pending**; a one-object negative cannot establish whole-graph non-use or CD-ROM feature absence. |

The existing `../../disk-native-role-review.md` distinguishes current local floppy/fixed-disk owners from the two SLAVEPC remote-provider variants and lists targeted disk tests. This additional object observation does not validate remote transport, actual media reads/writes, DMA, interrupts, reset or ejection. A new functional decision requires identifying the intended remote profile and its provider/transport, rather than enabling the macro blindly; CD-ROM needs an independently traced actual host and guest consumer.

Replay `python artifacts/mvdm-audit/shard-2/dos-disk-legacy-provider-observer.py` and compare the JSON. The Ninja dependency paths were resolved from the worker build directory. No product, guest, installation, shared ledger or build tree was modified.
