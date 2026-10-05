# T431 S1 native launch hook audit checkpoint

## Question, inputs and procedure

Can controlled native text targets launch original DOS/Win16 images through
run16 while keeping ordinary Windows creation semantics and adding no helper?
This is an audit checkpoint, not S1 closure or runtime Hook acceptance.

Baseline: main43fa43865, owner-accepted T430 production526f73c1c and final
e60e29844 evidence. Source and actual CMD identities are pinned in
build/M0-T431/S1/r001-source/inputs.json. No production source, runtime package,
running process, Registry, guest or MVDM/OpenNT-host mirror changed.

Procedure: inspect the current launcher/classifier and NTVWM creation path;
dump actual SysWOW64 and System32 CMD imports through the existing x86 compiler
wrapper; retrieve the official installation reference and resolve its tag to
a commit; inspect the finite width/update/helper boundaries. Outputs remain
under build/M0-T431/S1/r001-source. Existing retired src.old is absent: no
recovery candidate was silently selected from it. Current native/launcher
sources have no nthook/Detours implementation.

## Current source decisions

| Mechanism | Current owner/location | Decision and constraint |
| --- | --- | --- |
| DOS/WOW classification | opennt-host/base/win32/client/vdm.c GetBinaryTypeW/BaseIsDosApplication, selected through base_classifier.h | Reuse the existing original classifier cohort, not extension-only/MZ recognition or a new PE classifier. Its native same-machine check and x86 ABI must not be generalized in the mirror for Hook64. |
| CUI/GUI metadata | run16-exe/image_classification.c | Reuse OS image-section metadata. Hook installation needs actual process bitness independently; subsystem alone is insufficient. |
| User image search | run16-exe/application_search.c | Reuse directory-first CWD/PATH policy for the admitted launcher contract. A CreateProcess null-application command requires native token/ambiguous-name interpretation audit before using this resolver; do not substitute CLI search for Windows semantics. |
| Original CLI | run16-exe/launch_options.c and main.c | Keep optional leading --wait/-- and original target argument syntax. No newly required --. Redirect only confirmed DOS/PIF/WOW targets; missing/unknown/native images remain native failure/passthrough, not launcher COMSPEC fallback. |
| Target creation | ntvwm-exe/execution.c launch_request | Existing suspended create, authenticated bind, then resume is the initial installer insertion boundary. Worker still owns actual target process wait/result/rollback. GUI gets no character frontend authority. |
| Resource inheritance | run16-exe/native_launch.c | Preserve exact handles/list, aliases, environment and CWD. Native target numbers in environment are not capabilities for a non-inheriting child. Hook must not manufacture authority from them. |
| Product paths | common/system_root.c | Current helper derives root from actual EXE; inside injected CMD that is Windows CMD, not our product EXE. Initialization must pin the installed product DLL/run16 path from the authorized installer, not reuse native EXE-root derivation or trust PATH. |
| Observation | Subsequent trace package | No observer records/RPC or process enumeration added here. Direct completion remains unchanged; future report failure cannot kill a handed-off child. |

The original OpenNT classifier is already composed; reuse that original owner
at the x86 boundary. The full original Kernel32/CSR launch/injection shell is
not composable here and is an explicit stopping boundary. A project-local
interception/installation seam is new product behavior, not recovered DOS
execution. Register the smallest specialist exception before production code.
There is no applicable approved third-party installer already in this graph.

## Actual CMD entrypoint evidence

Pinned SysWOW64 CMD SHA256:
4D4AA884B258AC42243F41292B3954340769EC36EFDEEA6B518A963C8C72891C.
Pinned System32 CMD SHA256:
15CD209C995B7D3009C91CA40892D5B8F1FB162F3E5F10716642EA278E8E0D7B.

Both dumps contain CreateProcessW through an API-set import, CreateProcessAsUserW
and ShellExecuteExW. This proves imported entrypoints, not which route a given
ordinary command dynamically takes or that all CMD launch forms are covered.
The historical OpenNT-4.5 nt/private/windows/cmd/cext.c:407 and start.c:693 use
CreateProcess with ShellExecute fallback at440/740; this is comparison evidence,
not modern CMD execution proof. Later controlled fixtures must observe actual
ordinary external creation and API-set/export resolution.

## Installation provenance and finite blocker

Official Detours v4.0.1 tag resolves to
e4bfd6b03e50de46b47abfbd1e46b384f0c5f833.
Research-only creatwth.cpp SHA256:
062533627BFA775246948FAD92F9F3C0EF7841CC1A83C15DB9178BE12F2DBF4F.
MIT license SHA256:
B301808B732CFAA60DF2B4B422D78CD97D2A15058B207E7E33F0535BA5170DD6.
No source/library is imported or linked. Any later adoption retains notices.

At that pinned source, lines654–684 reject opposite-width updates; the
CreateProcessWithDllExW wrapper at1432–1435 falls back to ProcessViaHelperW.
Its default helper branch is forbidden. Same-width suspended import installation
is a candidate, not tested here. This rejects the unmodified cross-width
mechanism, not every possible no-helper design. The one reusable installer
required by S1 remains unresolved; no helper or silent coverage waiver is added.
See [official source](https://github.com/microsoft/Detours/blob/e4bfd6b03e50de46b47abfbd1e46b384f0c5f833/src/creatwth.cpp)
and [installation contract](https://github.com/microsoft/Detours/wiki/DetourCreateProcessWithDllEx).

## Finite contract and next gates

Initial supported candidate is ordinary CreateProcessA/W native-text creation.
Preserve application/command interpretation, original mutable argument tail,
security attributes, explicit environment, CWD, streams and handle lists.
Confirmed 16-bit redirection returns the real run16 process/thread objects:
DOS wait/result and Win16 startup-only semantics, not an invented guest process.
Native results remain their actual objects/error codes. Caller-requested
CREATE_SUSPENDED must remain suspended; installer suspension is not ownership
of the caller's suspension count. Do not broaden into alternate token/logon,
elevation, protected process, debugging, managed-image rewriting or global hooks.
Unsupported combinations need exact passthrough/refusal tests, not guesses.

Before freezing initialization, prove authorized product-path discovery, width
selection, installer rollback and root capability transfer for both inheriting
and non-inheriting creation. Discovery text never authorizes a session.
Keep DllMain bounded; no blocking broker call/complex launch under loader lock.
Do not add a reporting ABI until the current authenticated resource contract
can carry the exact creation-time identities; observation remains subsequent work.

S1 is still open: actual ordinary CMD route, cross-width feasibility, initialization
ABI/resource transfer and final supported-flag ledger are not yet proved.
S2 production code is not admitted by this checkpoint. The current installed
eight-file package is untouched and usable.

## Follow-up: finite no-helper width decision

On2026-10-05 the owner requested continued execution. This follow-up inspects
the official Detours cross-width change and installed Windows API carriers;
it does not inject, create target processes, compile hooks or admit private
WOW64 transitions. The preceding contract design remains a candidate.

Research artifacts remain in build/M0-T431/S1/r001-source:

| Input | Identity / observation |
| --- | --- |
| Official repository PR161 metadata | Open, unmerged; head daaae6988c94647d7d194e8b15340ec3c2360bb3, retained detours-pr161.json. Status is a retrieval-time observation, not a support guarantee. |
| PR161 diff | detours-pr161.diff SHA25641E64913D8E618DF724A5715013EC35FDF477B2B896AF66196AEDAA5E1B8B3D7. |
| Exact head creatwth.cpp | detours-pr161-creatwth.cpp SHA25608BA0DA5FFAAC0C050CE297037F78F0FC9E1B7E4106AC10BE01A39F17DEDBF87. |
| Exact head MIT notice | detours-pr161-LICENSE.md SHA256B301808B732CFAA60DF2B4B422D78CD97D2A15058B207E7E33F0535BA5170DD6. No production import. |
| Installed SysWOW64 ntdll.dll | SHA25656770F0C0B47F313662444F3ECF77257BE36F1B82DDB36BE14555790FCA2F430; exports dump SHA2565294E25ABECF8FCC67639DB4C9172D4BCF3613F44417D14F32A639ECE824371B. |
| Installed System32 CMD headers | cmd64-headers.txt SHA2568ACE2D8E9CB491AEDCD4F2FEF392D1D95A7D426F45F68155D424AB814314844B; native AMD64 CUI, preferred ImageBase0x140000000, high-entropy ASLR. Preferred base is not an observed runtime mapping. |

Procedure: retrieve the official PR metadata/diff and its exact-head source
and notice; inspect the actual width branches; run dumpbin /exports and
/headers through the existing compiler wrapper. The initial sandbox network
fetch failed; the approved read-only network retry succeeded. No downloaded
implementation was built or executed. No project process was stopped.

### What the patch actually changes

[PR161](https://github.com/microsoft/Detours/pull/161) adds an UpdateImports32
instantiation to the64-bit build and selects it for a32-bit target. The exact
[source](https://github.com/number201724/Detours/blob/daaae6988c94647d7d194e8b15340ec3c2360bb3/src/creatwth.cpp)
at703–717 still rejects a64-bit target from the32-bit build with
ERROR_INVALID_HANDLE. The64->32 branch at720–733 is a research candidate,
not an accepted four-width installer. General CreateProcess/runtime restore,
rollback and payload-layout compatibility remain untested here. PR discussion
of private WOW64 APIs is not implementation or acceptance evidence.

| Direction | Finite disposition |
| --- | --- |
| 32->32 | Original same-width Detours import-table mechanism remains a source candidate; runtime proof pending. |
| 64->64 | Same mechanism, separate hook-only ABI/toolchain admission and runtime proof pending. |
| 64->32 | PR161 is a source candidate for the same installer contract; unmerged patch requires exact provenance, restoration/layout review and focused tests before adoption. |
| 32->64 | Neither pinned v4.0.1 nor PR161 provides it. Public pointer-width APIs cannot represent arbitrary64-bit target addresses from this32-bit installer. A different finite memory/installation facade is required; it has not been admitted or proved. |

### Why two DLLs do not finish the32->64 route

The public [ReadProcessMemory](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-readprocessmemory)
and [VirtualProtectEx](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualprotectex)
contracts take caller-width address pointers. An x86 caller cannot represent
an address above4GiB with those arguments. The native CMD preferred image base
demonstrates why assuming every target header lies below that boundary is not
a valid contract; no actual runtime address is claimed from its file header.
The retained source uses pointer-valued header/import addresses, remote reads,
writes, allocation and protection changes. A narrower read/write-only fix
does not establish that full transaction or its rollback.

The installed32-bit ntdll exports NtWow64QueryInformationProcess64,
NtWow64ReadVirtualMemory64, NtWow64WriteVirtualMemory64 and
NtWow64AllocateVirtualMemory64. The dump has no NtWow64ProtectVirtualMemory64
or NtWow64FreeVirtualMemory64 export. It also exposes NtWow64CallFunction64,
RtlWow64CallFunction64 and Wow64Transition, but their presence does not supply
an admitted general function-call ABI, allocation undo or protection facade.
No private entrypoint was called. This is a capability gap in the selected
mechanism, not proof that all possible no-helper mechanisms are impossible.

[QueueUserAPC](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-queueuserapc)
explicitly warns about cross-process/cross-width addresses. A guessed remote
LoadLibrary address/APC is not a safe fallback. Executing an extra remote
thread to load a DLL while the caller requested an entirely pre-entry install
also needs a different ordering/loader/suspension contract; it is not adopted
as a drop-in workaround. The
[official helper design](https://github.com/microsoft/Detours/wiki/OverviewHelpers)
uses a matching-width helper, which remains prohibited here.

### Recovery disposition and admission boundary

- Existing original OpenNT classification remains reusable; its obsolete
  Kernel32/CSR product shell is not a composable cross-width installer.
- The smallest unchanged external source slice supports same-width installs;
  the selected unchanged cross-width wrapper requires the forbidden helper.
- PR161 is the smallest identified external adaptation for64->32, not32->64.
  No patch is adopted merely because the repository is official.
- A32->64 facade must prove full-width discovery/read/write/allocation,
  protection, context/import installation and undo without running the child's
  main thread or adding a helper. Private WOW64 ABI/transition work requires
  a separate bounded research admission, not an implicit product exception.
  Do not import the prohibited NTVDMx64 injector or add a second injector.

The next required owner decision is whether to admit a build-only, no-helper
private WOW64 feasibility probe for that exact32->64 transaction, with no
production wiring, no host policy/Registry changes and no deployment. Such an
admission must name the source/ABI/toolchain boundaries and explicit failure
stop. A successful probe would still not admit the production hook automatically.
Without it, the current source/API audit has reached its specific stop boundary;
S1 remains open, S2 is not admitted, and actual CMD/bootstrap tests are not
claimed passed. The accepted runtime package and unrelated edits are preserved.
