# T219 S1 — DEM child/PDB lifecycle map

## Decision

The old finite-profile 50:3C provider is retained evidence, not the current
direct-host lifecycle solution. It releases only boot-namespace search state.
The current direct-host DEM file session has opaque live file handles without a
PDB owner field, so it can close them only at whole-VDM teardown. That does not
meet the source-owned process lifetime required when a child exits and its
parent VDM continues.

T219 therefore owns one DEM child/PDB lifecycle package. It must reconcile
adapter-owned resources by copied PDB identity before guest resume. It must not
replace guest DOS teardown, implement VDD/device cleanup, or create a
BOP-specific process policy.

## Original sequence and ABI

| Order | Original owner | Contract |
| --- | --- | --- |
| Entry | msproc.asm Xfer_To_User | After guest EXEC has constructed child entry, stack and PDB, it issues DEM 50:36 just before guest register setup and far-return into the child. |
| Entry provider | demmisc.c demEntryDosApp | Reads PDB from DX. Other than an optional post-first-call VDDCreateUserHook, returns VOID. No loader result, guest write or CPU policy exists. |
| Guest exit | msproc.asm Exit/Abort and msctrlc.asm reset_environment | Guest restores vectors/process state. A non-TSR child calls DEM PDBTERMINATE with current PDB in BX, frees guest arena/process resources and invokes DOS_ABORT. |
| PDB provider | demsrch.c demTerminatePDB | Optional VDD termination hook, HostTerminatePDB, then release the PDB FindFirst/FindNext list if it exists; absence is success; returns VOID. |
| Parent return | tcode.asm and cmdexec.c cmdReturnExitCode | COMMAND 54:0B follows guest return. DX is exit code, AL current drive, BX:CX redirector info. Carry means another DOS command; clear means shelled-out session end. |

DEM 50:36 and 50:3C are lifecycle notifications. Neither loads a program,
transfers control, or chooses the CLI exit code.

## Current code and ownership gap

| Current state | Evidence | Disposition |
| --- | --- | --- |
| Finite search cleanup | bx_ntvdm_search_sessions_v1_release_pdb already releases only slots matching copied owner_pdb and preserves foreign slots. | Reuse this primitive. |
| Old lifecycle provider | bx_ntvdm_dem_session_lifecycle_provider_v1 accepts exact real-mode 50:3C, calls the finite provider, resumes RIP plus four with no register or flag delta. | Valid only for its historical boot-namespace profile. |
| Direct host handles | bx_ntvdm_dem_file_session_v1 token slots contain HANDLE, generation and in_use only; teardown closes all slots only when the VDM provider is destroyed. | Gap: direct host child handles have no PDB owner or PDB-release primitive. |
| Pending direct transaction | bx_ntvdm_dem_whole_provider_v1 records copied boundary, CPU and gather state but no PDB owner. | Gap: child termination cannot safely distinguish child pending work from parent work. |
| Entry notification | bx_ntvdm_vdd_create_user_notify_service resumes 50:36 without a result. | Correctly not a loader, but must become explicit no-VDD profile selection rather than an unconditional final composition rule. |
| Historical VDD/device work | demmisc.c/demsrch.c and nt_msscs.c use VDD callbacks plus floppy/fdisk cleanup. | Deferred. It is neither ordinary host file cleanup nor a Bochs responsibility. |

## Required adapter seam

Original DEM translation units cannot compose in the modern x64 CLI because of
CCPU/SAS guest mappings, VDD and NT4 product-host dependencies. The first
usable rung is therefore one source-derived DEM session provider retaining the
original ordering and VOID result rule.

For a no-VDD profile, 50:36 resumes by four bytes with no delta. For 50:3C,
the provider must release only adapter state owned by BX low word: search
continuations, opaque direct-host handles and pending transactions. It must
leave foreign PDB state unchanged, perform no guest-memory read/write, and
resume by four bytes with no register or flags delta. Profiles selecting VDD
or device cleanup decline to their separately admitted owner.

## Recovery ladder

| Rung | Decision |
| --- | --- |
| Original source | Guest EXEC and guest teardown remain source-built. DEM source supplies mandatory order and result semantics but is not x64-composable due to CCPU/SAS, VDD and product-host dependencies. |
| Smallest seam | Implement one PDB-owned DEM resource-session ABI; reuse the existing search release primitive but migrate direct file and pending state to it. |
| External intrusion | Rejected. Missing VDD/floppy/fdisk composition is not a Bochs gap. |
| New behavior | Rejected. No synthetic DOS cleanup, VDD emulation, device service, host process launch or selector exit policy is authorized. |

## T219 package sequence

1. S1 (this record): prove lifecycle ownership and direct-host PDB gap.
2. S2: implement one PDB-owned DEM resource-session ABI; migrate direct
   handle/search/pending state; test direct, readonly, overlay and virtual
   lifecycle behavior.
3. S3: map whole COMMAND 54:0B parent-return contract and its unavailable
   historical broker/event-thread dependencies.
4. S4: compose DEM lifecycle and one-shot COMMAND-return providers and run
   package regression.
5. S5: run one bounded native COM-child entry and normal-return observation.

No code, build, host mutation, native trace, BOP implementation or Bochs
change occurred in S1. T176 finite-profile evidence remains limited to its
named profile; it is not direct-host lifecycle closure.

