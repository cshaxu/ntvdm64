# Build-ID guest BOP and kernel-build FIXEXE roles

## `softpc.new/base/inc/build_id.h`

This owned original header defines the bit-packed `BUILD_ID_CODE`; `softpc.new/base/bios/build_id.c:59-74` services module number in AL, writes `Base$` to DS:CX, sets BX to that code and clears AX for BASE_MODULE. `bios.c:183` registers it at BOP 21. The current captured x86 worker `ntvdm.exe.map` maps `_Get_build_id` to `original-softpc-bios:build_id.obj`; part-4 `mvdm-evidence.json` records this header as a VALID dependency of that object. Historical `docs/etc/evidence/m0-t420-s26-bios-capability-baseline.md:34,49` and `tests/observation/bios_capability.asm:60-74` report and implement an actual guest BOP witness for Base$, nonzero ID and AL=0. This is evidence of that earlier version and bounded case, not a fresh S10 run or proof of every module selector and guest pointer failure.

Suggested state: selected declaration/input for original mapped BIOS BOP, with historical real guest behavior evidence. Retain pending current-release object-to-image binding and non-BASE/error coverage rather than implying total BOP contract completion.

## `wow16/kernel31/fixexe.c`

Original source implements a host build-time utility that opens a supplied executable read/write and updates its DOS MZ length and maximum-allocation fields; it is not an NTVDM runtime BOP. `kernel31/makefile:310-360` runs `fixexe kernel.exe` after 286/386 links (and resource stamping on 386) as part of the historical guest kernel build. The rule at 829 covers fixexe.obj; `shard-2/tools16-unmirrored-review.md` records retained OpenNT-4.5 `tools16/fixexe.exe` as a separate candidate tool and explicitly does not prove it was compiled from this C source. Manifest KRNL386 is a preserved approved ZIP baseline; do not infer it was built by this recipe. Source says it sets size to file size +512, while its expression is `(lFilesize + 511)/512` and the comment should not be substituted for verified byte-level output. Do not run it on guest.

Suggested state: historical guest-kernel build-tool source; current guest binary provenance and this tool's actual execution not demonstrated. No product or guest changes, no build or test run.
