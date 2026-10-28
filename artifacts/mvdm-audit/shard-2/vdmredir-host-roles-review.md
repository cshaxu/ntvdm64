# Shard 2: VDMREDIR host input and image identity

Five pending files match both original trees. The companion observer resolves Ninja's relative dependency paths against the worker build directory, records exact compile/resource/link edges and captures three DLL hashes. No guest or product was run or modified.

| Entry | Captured evidence and bounded disposition |
| --- | --- |
| `dlstruct.h` | Exact normalized dependency of `vrnetapi.obj`; original source describes down-level structures. Candidate **partial**; layout, marshalling and network behavior remain open. |
| `vrputil.h` | Exact normalized dependency of `vrnetapi.obj`; declares private redirector utilities. Candidate **partial**; individual consumers and errors are not fully tested. |
| `vrica.h` | Exact normalized dependency of `vrnetb.obj`; declares interrupt-related interfaces. Candidate **partial**; IRQ ordering, async completion and teardown remain open. |
| `vrdisp.c` | Selected as `vrdisp.obj` and present in current `VDMREDIR.dll` link input; source table names NetBIOS and DLC entries. Candidate **partial**; the existing shard-3 private-map review has bounded earlier image/table evidence, not live service or current image equivalence. |
| `vdmredir.rc` | Selected resource compiler input, with its resulting resource object passed to the DLL link. Candidate **partial** for current build inclusion; version resource linkage does not prove functional behavior. |

**Current cache and publication differ:** captured `build/.ninja/msvc-x86-release/worker/VDMREDIR.dll` SHA-256 is `FE70BD966A225A08EA64C385B82FC8883EB5B5A59C954CAC3633A0A880BBA61A`, while `assets/release/VDMREDIR.DLL` and `O:/winnt/system32/VDMREDIR.DLL` match at `F1AA244001393318EB4BECDAB60985D468142471B31C0AA82CCB424BEF1B4DD7`. This is an observed identity difference only; its cause and semantic impact were not investigated. The current graph cannot alone prove the published DLL was built from its captured objects. Coordinator must resolve release identity against the actual latest baseline before making that claim; do not replace files in this audit.

The earlier `../shard-3/redir-role-review.md` records an independently linked historical private-map comparison with an earlier release identity. Recheck its exact inputs before carrying that evidence forward. Real network peers, NetBIOS/DLC operation, concurrent service calls, errors and process/interrupt lifecycle remain unverified by this review.

Replay `python artifacts/mvdm-audit/shard-2/vdmredir-host-roles-observer.py` and compare its JSON. Graph and image hashes are snapshots. No guest, product, installation, shared ledger or build tree was modified.
