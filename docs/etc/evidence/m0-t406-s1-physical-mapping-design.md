# M0 T406 S1 physical mapping research and restoration design

## Request, scope and method

Owner admitted queue head on 2026-09-12: thoroughly research/design original
physical mapping, quantify unnecessary diff, identify still-needed diff,
overlay and autonomous mechanisms, ideally zero autonomous implementation.
One agent executed then reviewed. Repair awaits owner approval. Baseline
f2646d80b; T405's runtime/WIP limitations remain. This is U02, not a new CPU,
broker or U03 cache-selection implementation.

Read current/original functions, callers, headers, original build descriptions,
current graph and retained object symbols. Compare using git diff --no-index
--numstat and --unified=0 against O:/repos.external/OpenNT/base/mvdm.
Search named services across OpenNT and OpenNT-4.5 C/header/assembly/includes,
then other text/build inputs. Enumerate SoftPC .obj/.lib with rg --files -uuu.
External inputs stayed read-only; no source import/build/runtime repair.
An exact-name web search supplied no additional result and is not evidence
of global source absence. All conclusions below rely on local primary inputs.

## Reproducible input identity

Paths below are relative to src unless prefixed otherwise; SHA-256:

| Input | SHA-256 |
| --- | --- |
| adapter-mvdm-host-out/softpc/mvdm_softpc_physical_mapping.c | 48A898098DAE4BD8E14D28DDC0044B064D4EFDD608C153035D9A4635987B2AD0 |
| adapter-mvdm-host-out/softpc/include/mvdm_softpc_physical_mapping.h | 6BCFC1DE638A3E0C771AEDCC7524B263FC076AA4390EFAF8C6C4F4DA5F75F785 |
| mvdm-host/softpc.new/host/src/nt_mem.c | A21CE730F39BC55AC09E6E5FAD540EA0FF9C98D9977F1ADF88FFAD68851DF78D |
| mvdm-host/softpc.new/host/src/nt_emm.c | 58BACF6663B513AC96F6A2A260E8499212005BE0D95DBC1CFA41D8910DFF89F6 |
| mvdm-host/softpc.new/base/ccpu386/ccpusas4.c | C9CAFF416D80CF8088D5239C9698BA858FBBD0FA8B7D9EBBE02A87467C60C541 |
| mvdm-host/wow32/wdib.c | D0BD4F9CB7BBA0DC5E977A8C096337DF0830E3804EF6AEB8759CF979176DD910 |
| OpenNT original softpc.new/host/src/nt_mem.c | 55832EE249E18683E6D80C582AC8DFF47318A5D467D1B1017777FC961BDCCF5E |
| OpenNT original softpc.new/host/src/nt_emm.c | D51310AF362E60023FEB59C89AFB21FF6FAE8E1BB74AE8614231F4EC3A001729 |
| OpenNT original softpc.new/base/ccpu386/ccpusas4.c | 2802840EF84106AE0F99C6F17AF370DA66C9381A1355570F5998039FBB33534E |

## Original owner and four-rung findings

1. Original nt_mem.c:VdmAddVirtualMemory already owns DWORD alignment, 4 KiB
   rounding, Intel-address reservation, overwrite notification, remap flag
   and returned offset. VdmRemoveVirtualMemory restores normal backing through
   VdmSetPhysRecStructs(HostAddress, IntelAddress, Size), clears the flag and
   frees the reservation. This is available original logic, not missing code.
2. Original emm_mngr.c owns EMS handles, page selection and lifecycle; nt_emm.c
   owns the CPU40 EMS storage and host_map_page/host_unmap_page wrappers. These
   call VdmMapDosMemory/VdmUnmapDosMemory in four 4-KiB pages per EMS window.
   Its comment about copying is not proof of its actual remap implementation.
3. Definitions of those two services and VdmSetPhysRecStructs were not found
   in the searched original source/build inputs. Original host/src/sources
   selects nt_mem.c/nt_emm.c; obj.vdm/sources chooses a4Pd/Pg libraries or
   ccpu486.lib. Both inspected SoftPC trees contain zero .obj/.lib files even
   with ignored files included. This is a bounded missing-body/build-carrier
   finding, not proof that no historical distribution has the bodies or that
   the functions belong to the NT kernel.
4. Original CCPU c_GetPhyAdd only computes Start_of_M_area + address (or
   BACK_M reverse addressing); it has no external-page translation lookup.
   Original MIPS prot_c.h PhysicalPageREC has a translation member, while
   base/cvidc/prot_c.h's same-named structure does not. Copying the name or a
   generated GDP offset cannot compose the original remap algorithm in CCPU.
5. Original x86_emm.c provides a different full EMS host implementation,
   under MONITOR: section views, MEM_DOS_LIM, fixed-address mappings and monitor
   backfill ownership. It is useful source evidence, not a drop-in CPU40
   provider. Importing this whole branch revives a prohibited monitor profile;
   replacing it with public API names alone does not prove matching ownership.
6. NTVDMx64-derived fmstubs.c contains an empty VdmSetPhysRecStructs placeholder.
   Current formal selection takes only activity/vector subsets, excluding that
   body. Selecting the placeholder is neither original recovery nor a fix.

Therefore rung 1 is usable for upper allocation/EMS/WOW policy, but not yet
established for the three lower mapping services. Rung 2 is the recommended
finite binding below original callers. A mirror-private hook may be needed
at c_GetPhyAdd (rung 3) because unchanged CCPU has no remap boundary. Its body
must not become a new EMS policy. Rung 4 cannot be described as avoided until
an original lower body is actually recovered; renaming local code or moving
it into opennt-host is not original-source recovery.

## Current full-path findings

### DIB contract break is real at the interface, runtime cause remains unproved

wdib.c is text-equal to the OpenNT file. Its two non-i386 paths directly pass
pvBits to VdmAddVirtualMemory, followed by original selector/callback handling.
The retained r015-wow32-provider graph compiles wdib.c without /Di386; dumpbin
/symbols on its obj/wdib.obj shows undefined references to VdmAddVirtualMemory,
VdmRemoveVirtualMemory and VdmAddDescriptorMapping. Thus this is not merely a
branch assumed dormant because the host executable is x86. Retained object
selection is not evidence that a live WRITE run reached a DIB call.

Current prepare requires a prior publish record, but the only explicit caller
found in src/tests/tools is ccpu_bounded_execution_fixture.c. The fixture
publishes external_page before calling the original entry, hiding that gap.
The current record identifier already equals the x86 native pointer; surrogate
language in DIV-035 is stale. Restore the direct original caller contract;
do not teach WOW a new publication protocol.

Current set has a void result and can silently decline registration, while
the upper original entry returns success. Removal uses zero rather than the
original normal-backing address. Resolve matches only actual supplied bytes;
padding/unpublished/retired lookup misses fall back to ordinary RAM without
distinguishing a missing remap from a genuinely ordinary page. Exact-span
unmap returns NOT_FOUND for repeats; alias remapping is first-match, single-hop
with no source RAM/backing extent validation. These are observable policies
requiring source/consumer proof, not automatically bugs in every workload.

### CPU, bulk and direct-pointer consumers

c_mem.h physical operations call phy_r/phy_w; phyR and writes reach c_GetPhyAdd
after original A20/type/range checks. c_sas_PRS/PWS loop over bytes; linear
loads/stores and forward moves similarly retain original scalar SAS calls,
including page splits for words/dwords. Keep these original loops unchanged.

However nt_emm.c EM_loads/stores/moves use memcpy/CopyMemory/MoveMemory on a
single get_byte_addr result. host_copy and host_exchg conventional-memory
ranges can therefore cross guest mapping boundaries while assuming contiguous
host storage. A lookup at only the first byte is insufficient for such a span.
The upper algorithms are original; the missing binding must either provide
truly contiguous alias views or preserve copying/move ordering through bounded
SAS access. Do not replace memmove with an unchecked forward-copy loop.

EMM backing is allocated by original VdmAllocateVirtualMemory and freed by
VdmFreeVirtualMemory; GDI/view owners retain DIB allocation/free responsibility.
The mapping mechanism borrows backing, never frees the DIB. The existing
session-tagged lists have no internal synchronization; source-thread/teardown
requirements must be established before assuming concurrent access is safe.
U07-M shared lease redesign remains DPMI/XMS-owned; reached U02 backing
publication/retirement must still be proved locally.

## Measured footprint and reduction candidates

Counts are physical added/deleted text lines, including comments/blank lines,
not executable statements or a net executable-size estimate. Reproducible
comparison uses OpenNT/base/mvdm as root and git diff --no-index --numstat.

| Mirror path | Added | Deleted |
| --- | ---: | ---: |
| softpc.new/host/src/nt_mem.c | 70 | 21 |
| softpc.new/host/src/nt_emm.c | 12 | 6 |
| softpc.new/base/ccpu386/ccpusas4.c | 42 | 1 |
| softpc.new/base/dos/emm_mngr.c | 32 | 24 |
| softpc.new/base/inc/emm.h | 6 | 3 |
| softpc.new/host/inc/host_emm.h | 6 | 1 |
| wow32/wdib.c | 0 | 0 |
| Total selected mirror comparison | 168 | 56 |

Separately: physical_mapping.c is 326 lines and its header 31 (357 total).
No dedicated U02 implementation body is in mvdm-host-overlay; the mapping
body is autonomous adapter code. Shared diagnostics and patch files cannot
be counted wholesale as U02 deletion candidates.

| Candidate restoration group | Current added/deleted footprint removable if verified | Conditions |
| --- | --- | --- |
| nt_mem DIV-034 pointer-width macros and arithmetic | 36 / 14 = 50 | Restore as one x86 allocation-header cohort; verify types/build and allocation/free/reallocate. |
| EMM DIV-061 pointer carrier changes across four files | 46 / 32 = 78 | Restore matching declarations/callers together; retain stdlib declaration and separate LIM selection. |
| nt_mem DIV-035 publication/alignment/removal protocol | 25 / 6 = 31 | Lower binding must accept raw pointer, handle capacity/failure and restore normal backing first. |
| Total design candidates | 107 / 52 = 159 | About 71% of the 224-line comparison footprint, before any required new binding hook. Not a tested net reduction. |

The first two groups total 128 diff lines of x86 rollback candidates. The last
31 are conditional on remap recovery. Residual comparison would be 61 added /
4 deleted (65 total) before new edits, not zero. Preserved portions include
declarations, mapping hook, observations, printf correction and LIM profile
selection. Diagnostic removal belongs to its owner; do not silently count it
as functional restoration here.

publish (lines 99-150), prepare (152-170), cancel (310-318) contain 80 lines;
their declarations add five. Those 85 lines are gross protocol-retirement
candidates, not guaranteed net savings: resource validation and safe setup must
still exist below the original interface. The other 246 implementation lines
also need redesign/review, not blanket retention or deletion. No honest final
line count for the replacement binding exists before implementation.

## Proposed design and S2 acceptance

1. Restore original upper nt_mem and EMS policy/types after paired compilation.
   Keep wdib.c unchanged. Preserve original VdmSetPhysRecStructs signature and
   its removal-as-normal-backing semantics, not a zero-token special protocol.
2. Prefer one finite page-binding authority in the SoftPC boundary. Initialize
   required metadata before the void remap call can fail; use original known
   physical extent, not a generic token manager. Allocation failure must return
   before publication/reservation success, or have an explicit rollback. The
   exact capacity/preallocation scheme needs implementation proof, not an
   invented source attribution. Remap metadata does not own guest allocations.
3. Keep original EMS map/unmap calls and translate page windows to the selected
   backing. Prove two windows share bytes, switching preserves data, and normal
   backing is restored on removal. Only consumer-proven overlap/unmap behavior
   is implemented; arbitrary chains/partial mappings are not new features.
4. Bind CCPU physical resolution through the smallest registered hook without
   opcode/segment semantics changes. Audit all direct-span users. Prefer original
   SAS loops for bounded transfer where contract-equivalent; overlap-safe moves
   and EMS callbacks require original ordering evidence before any substitution.
5. Freeze same source/media baseline; test raw-pointer DIB entry without manual
   publication, unaligned input, rounding, failure before publication, normal
   restoration, alias retirement and no use-after-free. Test CPU/SAS scalar,
   word/dword boundary, linear/physical bulk and direct EMS copy/exchange across
   noncontiguous windows; include A20/type checks and common ordinary RAM.
6. Formal x86 link plus COMMAND/EDIT and EMS workload regression; reached DIB
   guest/host bidirectional visibility and cleanup need real consumer evidence.
   A stalled WOW workload stays a named acceptance limitation. Report actual
   removed autonomous/new binding/imported original lines separately.

Zero autonomous upper policy is a justified target. Zero non-original lower
mapping code is not currently demonstrated: exact lower bodies and a matching
CCPU remap model are absent from searched inputs. Necessary capabilities are
not proof that the current linked-list implementation is necessary. Original
sources may specify caller behavior without providing executable lower bodies.

## Reviewer conclusion and limits

S1 establishes a bounded restoration design and measured gross candidates,
not a proven drop-in repair or final net reduction. No new build, execution or
source edit was performed. Retained object evidence proves a DIB link edge,
not the WRITE failure's cause. Report this distinction to the owner and stop
before S2; approval is required for implementation. Run governance/link/diff
checks and preserve this evidence through commit/push.

Delivery review: documentation governance with relative links/anchors and
git diff --check passed. Arithmetic recheck gives width +82/-46, conditional
protocol +25/-6, total +107/-52. No src/tools/tests change is present. The
reported amounts remain prospective gross footprint, not implemented savings.
