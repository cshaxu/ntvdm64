# T478 S6 — SAS tail applicability disposition

## Scope and conclusion

Baseline: 688e2152b. The owner authorized S6 through closure, explicitly limiting
work to original OpenNT capability and excluding repeated work on resolved
behavior. Product changes would require build, tests, worktree assets/release
publication and a local commit; O:/winnt writes and push are prohibited.

Finding 04 closes as a bounded **no demonstrated applicable repair** disposition,
not as implementation of the three tails or proof of universal unreachability.
No production or guest code changes are justified by the available evidence.
Keep the coherent vector layout and existing unavailable entries. No success
stub, invented memory behavior or speculative layout cleanup is introduced.

| Slot | Current selected behavior and original evidence | Disposition |
| --- | --- | --- |
| Sas_touch / c_sas_touch | Original CCPU initializer references it; selected fallback raises a breakpoint. Both pinned OpenNT families have declarations/initializer entries but no provider body found in their SoftPC C/assembly sources. Current selected symbols show only the initializer reference and provider definition. | Retain unavailable boundary; no demonstrated missing supported operation to implement. |
| SasVirtualiseInstruction / c_VirtualiseInstruction | Same original declaration/initializer situation and selected breakpoint provider. No additional direct reference found in current libraries or directly linked objects. | Retain unavailable boundary; do not invent instruction virtualization semantics from its name/signature. |
| IsPageInstanceData | Null final slot. Original YODA page-table presentation calls it only under `#ifndef CCPU`; selected generated header exposes no CCPU macro/function for it. | Non-selected diagnostic consumer, not an admitted CCPU40 capability gap. Do not add a provider or revive another profile. |

The actual SAS scalar/string/move/fill, A20 and physical mapping providers remain
unchanged. In particular, the separately reached Sas_overwrite_memory was already
connected to c_sas_overwrite_memory by DIV-189; it is not one of the unresolved
tails and is not reimplemented here. Cpu.Sas publication and SasSetPointers' full
structure copy remain intact. The shorter C-VID initializer would misalign the
expanded x86 layout, so it is not a replacement. SasVectorNames' historical shorter
metadata is not consumed by the binder generator: it reads the CPU/public-private
groups and takes the SAS declaration from the selected generated header.

## Evidence and reproducible procedure

The [retained recheck](../../../artifacts/mvdm-audit/shard-3/sas-vector-s6-recheck.py)
is run from this worktree root with Python and the existing MSVC 14.43.34808
x86 dumpbin. It reuses the [original PE probe](../../../artifacts/mvdm-audit/shard-3/sas-vector-pe-probe.py)
with the current executable hash and matching map; it never compiles, executes,
patches or publishes a product. Output is confined to build/t478-s6. The
[compact results](../../../artifacts/mvdm-audit/shard-3/sas-vector-s6-results.json)
retain inputs, hashes, commands, process timing and observations.

Source searches used `rg -n -i 'sas_touch|virtualiseinstruction|ispageinstancedata'`
on current host/project sources and on both read-only original SoftPC trees,
restricting original provider searches to C and assembly. Original roots are
O:/repos.external/OpenNT/base/mvdm/softpc.new and
O:/repos.external/OpenNT-4.5/nt/private/mvdm/softpc.new. The upstream anchor and
historical source identities remain in the [original audit](../../../artifacts/mvdm-audit/shard-3/sas4gen-vector-audit.md).
No guest tree is audited or changed. Current CCPU macros call the two c_* symbols
directly; non-CCPU PROD macros use the vector, so those routes are distinguished.

Current inspection results:

- All 74 worker/WOW32 cached libraries scanned: the only two named undefined
  references occur in original-ccpu386.lib; the only definitions occur in
  softpc-ccpu-vector-defaults.lib. The original sasCdef.obj relocation table puts
  those references in the data initializer at offsets D8 and E0.
- All 238 directly linked object entries from the NTVDM/WOW32 Ninja link edges
  have no named references to those three functions. This complements the archive
  scan; it is not an indirect-call theorem or a new compilation result.
- The current PE initializer has 58 fields: indices 54 and 56 point to the same
  folded breakpoint function, VA 0057E7C0; index 57 is zero. EDL_fast_bop is distinct.
- Current NTVDM disassembly has no direct reference to that provider address,
  absolute references to the three live tail addresses, or the checked obvious
  `call [register+offset]` forms. This text filter does not resolve computed
  indices, two-stage pointer loads or arbitrary aliases.
- NTVDM, WOW32 and VDMREDIR export scans expose none of the five checked names
  Cpu, Sas, c_sas_touch, c_VirtualiseInstruction and IsPageInstanceData. This
  does not prove an address cannot escape through another API.

## Existing verification reused, with limits

The prior isolated SAS fixture already verified breakpoint negative controls and
zero observed vector calls during bounded scalar/string/move/fill, A20 and page
mapping operations. It did not cover direct macros or pre-install initialization;
its separate timer/PIC continuation was a non-pass. Those results are retained
as historical evidence, not rerun or relabelled as new production acceptance.

[S4](m0-t478-s4-remote-rap.md) passed the 16 Full product gates; [S5](m0-t478-s5-directory-restart.md)
then verified directory cases without production changes. No product source is
changed by S6, so a fresh build, Full run or publication would not establish a
new repair. Existing gates remain evidence for those workloads, not proof of
all SAS-tail paths. This S performs static/identity verification only.

All ten worktree release images still equal the tested S4 package manifest,
package 8f9f583a-a682-4175-b82e-1f926c9dbba4. NTVDM release SHA-256 is
BA61A9FEBFB19A523FB2151D3C6A1288691B261D7E345C5076E8A3C544975C82.
The current raw cache image has SHA-256
A4472C6DACB360B296F757CC61480EB186253A1D793478AECE428775D61629CD;
its .text, .data and .reloc section addresses, sizes and bytes equal the published
image, validating the relevant code/table comparison despite metadata differences.
The comparison authority here is S4's frozen product-system32-manifest.json,
not an assertion that every older metadata file under assets/release was refreshed.
No assets are overwritten and no new package is claimed.

## Closure and reopening condition

This research-only P records the source/binary disposition, keeps explicit
unsupported behavior and leaves functioning original owners alone. The missing
original bodies and lack of exhaustive indirect/full-lifetime tracing remain
limitations, not silently passed tests. There is no basis for a feature repair
without an actual applicable caller and original contract. Reopen finding 04
only on that new evidence, rather than scheduling another identical audit.
S7/finding 10 remains proposed; it is not admitted. T478's broader inventory
qualification remains open. Governance and diff checks are required before commit.
