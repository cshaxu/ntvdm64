# Kernel31 historical inputs: shard 3 file-role review

Scope: seven shard 3 paths under `src/mvdm/wow16/kernel31`. This is read-only source and file identity evidence; no guest build or execution. Full SHA-256 comparisons, sizes and missing-source distinctions are reproducible with `python artifacts/mvdm-audit/shard-3/kernel-input-observer.py`; captured output is `kernel-input-observations.json`.

| Path | Proposed file status | Evidence and boundary |
|---|---|---|
| `debug3/kernstub.exe` | pending | 721-byte MZ image matches OpenNT 4.5; corresponding OpenNT path absent. `kernel.def:6` names KERNSTUB.EXE and `makefile:317,346,377` builds and uses a DEST copy from `kernstub.asm`. The debug3 binary is not proven to be that generated DEST file or a deployed executable. No assumption of guest execution or exclusion. |
| `kernel.api` | pending | Identical in both reference trees. `layer.asm:20` includes it; `makefile:645` lists it as dependency. The layer's actual guest image and behavior remain unproven. |
| `newexe.h` | pending | Identical in both references. `rip.c:10` includes it, with historical dependency in `makefile:833`; `wow16/toolhelp/{stack1,module,global}.c` also include a header by that name, but include path resolution is not proven. No claim about actual runtime route. |
| `m.msg` | excluded, empty historical file role only | Zero bytes in all three trees; no substantive table to connect. Does not exclude guest error messages or any associated runtime behavior. |
| `patch.txt` | excluded, documentation role only | Identical in both references; prose begins as an overview of Windows 95 module patches, not an executable patch or proof of deployed guest behavior. |
| `qgrep.grp` | excluded, historical symbol-generation input role only | Identical in both references; `makefile:388,396,407` uses it for historical kernel symbol outputs. This classification does not exclude the guest kernel or its debugging behaviors. |
| `state.rst` | excluded, historical editor state only | Identical in both references, 72 bytes; contains file/position state for `e:\\src\\core\\kernel\\lddebug.asm`. Not evidence that `lddebug.asm` itself is unused. |

No numerical runtime coverage can be inferred from these source and file roles. In particular, excluding a historical input here is not a finding that its corresponding guest capability is absent or complete. Kernel runtime behavior, actual header resolution, and the deployed identity of the stub remain outside this observation.
