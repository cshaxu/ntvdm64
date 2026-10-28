# Part 2: DOSX fault/interrupt source variants and host dispatch

The four owned guest paths below are original-identity-bound by the part-2 DPMI observer. The historical 486 WOW recipe in `dpmi/makefile:152-165` explicitly compiles `486/dxint31.asm` and `486/dxintr.asm`, while `dosxi.lnk` names the resulting objects. `dpmi/dxintr.asm` and `dpmi/ntintr32.asm` are distinct root source files with their own `PMFaultAnalyzer` names; a same-named procedure is not proof that the historical 486 object or packaged DOSX selected either. Exact immutable guest source-to-binary provenance remains unknown.

Original host `dpmi32/dpmi32.c` dispatch table entries 0A, 0E, 0F, 11 and 14–18 own corresponding DPMI fault/vector/debug/return services; guest exception frames, host CPU execution and debugger task state have separate owners. Original `486/dxintr.asm` contains its own vector table, and the corresponding `dxboot` source establishes and sizes guest protected-mode resources. Preserve the actual selected transition and guest interrupt frames rather than importing an entire unrelated NT private CPU context model.

| Owned path | Evidence-supported role and current limit |
| --- | --- |
| `dpmi/486/dxboot.asm` | Original 486 DOSX boot source selected by historical recipe; runtime frame publication depends on exact guest image. |
| `dpmi/dxintr.asm` | Original root interrupt reflector with PMFaultAnalyzer; the historical 486 rule names a different file. Alternate-profile behavior and source-to-image selection remain pending. |
| `dpmi/ntintr32.asm` | Original 32-bit interrupt/fault analyzer source with ring-transition stack; actual compile/include and runtime profile not established. Keep pending, no exclusion or duplicate-provider claim. |
| `dpmi/dxbug.asm` | Original DOSX debugging source named by historical 486 recipe; active conditional paths and target-image lineage remain separately unproved. |

The final ledger in `m0-t420-s38-dpmi32-capability-review.md:2841-2855` reports candidate-specific real 16/32-bit fault/interrupt returns and nested divide handling. It explicitly retains unchecked reduced-limit INT31/0500 writes and original debug-register implicit task-exit release as limitations; WOW notification and injected host-resource failure were not covered. Those results cannot establish that the current release reproduces the same results, nor that `ntintr32.asm` is active.

S11 should verify the selected guest frame and actual host provider on current exact package, positive/nested fault and interrupt return, invalid pointers, failure recovery and task teardown. S14 owns genuine WOW notification/task interaction; S19 binds immutable guest and release hashes. Keep the unresolved original debug/invalid-span limitations visible and do not silently claim success, rebuild guest or replace its fault engine.

Replay `rg -n "PMFaultAnalyzer|rgw032Stack|PMFaultVector" src/mvdm/dpmi`, inspect makefile and dispatch table, the exact S38 final ledger, and rerun the part-2 DPMI observer. No new test or guest/product/shared ledger change.
