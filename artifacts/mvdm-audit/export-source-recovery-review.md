# Original export-source recovery findings

Two source files require partial classification. Their
[per-file records](export-source-partial-dispositions.csv),
[actual observations](supplemental-export-definition-review.json) and
[independent template review](shard-1/export-template-profile-review.md)
separate source retention, export-name availability and runtime semantics.

## Selected modified declaration

`softpc.new/obj.vdm/obj/i386/ntvdm.def` is absent from the two primary
comparison sets but has a registered supplemental original. Its original
SHA-256 is `98C00C3C4737108674061779A6C1E06F113C597D76536A49E0723AA617D58A7A`;
the current file is `09B1B928016158B2B5492C6726B0E1CE2EFBF07C4D14D0ED1C5DFDD4D0EFE362`.
Commit `9e8821856` added exactly one declaration,
`Sim32FlushVDMPointer`; all 136 original non-comment declaration lines
remain. Text retention is therefore 100%, while the mirror is still partial
under the owner's difference rule. This is not executable-code or behavior
coverage.

The file is active configuration input. T310's generator at lines 168 and
443–454 reads it into `generated/ntvdm-wow32-provider.def`. Actual package-c
dumpbin output contains the added export at ordinal 44, RVA `0004DA90`, and
all 134 original export names. Source/file selection inferred only from
`ninja -t inputs` missed this configuration read. Historical T405 evidence
had already identified this exception; this pass reconnects it to the current
whole-file ledger. Its path beneath `obj/i386` does not justify exclusion.

## Original template/profile gap

Canonical `softpc.new/obj.vdm/ntvdm.src` is byte-equal to both pinned primary
originals at `D47400179C1388B58FB8EAEC8331C6EEA16EE548BBA10E7AFBE588E9549BD0BC`.
It is currently bypassed by the static declaration described above.
Real x86 CL preprocessing with `i386` and `CPU_40_STYLE` yields 140 distinct
declaration names. This preprocesses metadata only; it defines no CPU30
profile and compiles/executes no monitor or guest code.

The sealed package exposes 138 of those 140 names (98.57%). The two absent
names are `VdmQueryFreeVirtualMemory` and `VdmReallocateVirtualMemory`, both
in the template's CPU40 branch. Four of that branch's six names are supplied
by the current project extension list. This is a concrete incomplete original
profile surface, not a reason to exclude the original template.

Name presence does not prove aliases, DATA, calling convention, provider
identity, ordinals or behavior. Name absence does not prove an internal call
fails: original DPMI code also has direct/in-process routes and approved
allocator adaptations. Source search finds original `nt_mem.c` definitions;
the cached map includes the query provider, while reallocation linkage and
its selected consumers require their own acceptance. No executable-code
restoration percentage is asserted for this declarative template.

## Finite recovery and acceptance

S11 owns recovery of the canonical original export-generation boundary:
use the primary-original template and explicit admitted x86/CPU40 metadata
profile; retain project additions in the existing generated extension layer.
Do not silently widen supplemental source admission or remove the published
`Sim32FlushVDMPointer` API. Restore the modified historical declaration's
original bytes when it no longer carries a project addition.

The group must resolve both missing original names through their real memory
owners, verify signatures/aliases/DATA, preserve the existing published
surface, and exercise valid and invalid allocation/query/reallocation/free
lifecycles before enabling or declaring the new surface complete. Coordinate
the original memory service checks with S12. Verify original-template profile
selection, cached generation/link propagation, no-change incremental builds,
actual PE exports, WOW restart/retirement and existing DOS/XMS/DPMI/VDD gates.
Source presence or forcing a link is not that acceptance.

This source-only audit changes no runtime or guest. The two partial rows keep
the source-policy/registration and runtime obligations visible instead of
labelling the generated carrier unused or the name ratio full functionality.
