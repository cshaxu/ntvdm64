# T432 S4 bounded Detours helper adaptation register

## Admission and original source

The owner-approved T432 S1 design admits the matching rundll32 transient
installer, preserving true suspended Windows children. S4 is active in CURRENT.
This register precedes source modification; runtime gates below are not passes.

- Source: Microsoft Research Detours4.0.1, pinned commit
  e4bfd6b03e50de46b47abfbd1e46b384f0c5f833, src/creatwth.cpp.
- Selected repository file: src/nthook32-dll/detours/creatwth.cpp.
- Before-adaptation SHA256:
  062533627BFA775246948FAD92F9F3C0EF7841CC1A83C15DB9178BE12F2DBF4F.
- Microsoft copyright and MIT LICENSE.md remain unchanged and packaged.
- Exact admitted seam: DetourProcessViaHelperDllsW only; register identifier
  T432-DETOURS-DIV-001. A helper entry/export in project-owned entry.cpp/build
  graph is not an imported-algorithm change.

## Recovery ladder and minimum change

Reuse the selected official AllocExeHelper, copied helper payload and matching
DLL's DetourFinishHelperProcess/DetourUpdateProcessWithDll import algorithm.
No rewrite of import update, payload discovery, restoration or decoding.
The unchanged W helper cannot meet this project's finite installation contract:
its internal double ResumeThread and infinite wait cannot be corrected by its
CreateProcess callback. A wrapper would still wait forever inside the call.
No usable OpenNT implementation of this modern dual-width import installer
exists in the selected closure. The admitted smallest source seam therefore:

1. Gets the real host Windows directory using GetWindowsDirectoryW, not the
   inherited WINDIR (which can describe the guest package). Keeps the existing
   opposite-width Sysnative/SysWOW64 loader selection. The W boundary preserves
   its caller's already-selected matching DLL path after AllocExeHelper: that
   original allocator assumes caller-width names and rewrites the last32./64.
   occurrence even in a directory. Our caller supplies actual target-width
   paths, so that heuristic would corrupt unrelated directories. Keep the
   allocator/layout unchanged; recopy the explicit bounded strings in W only.
2. Holds a SYNCHRONIZE target process handle while the real creator also pins
   the actual child. Creates only the matching Windows rundll32 suspended,
   with no inherited handles and hidden startup UI.
3. Copies the unchanged helper payload, resumes helper once, waits on target
   death first and helper completion second, with a10-second deadline for
   installation only. Target execution has no such deadline.
4. Rejects a dead target, failed helper exit, failed resume/copy/create/wait.
   On unsuccessful installation, terminates/joins only this call's owned helper,
   closes helper thread/process and local target-wait handles, frees payload,
   and preserves a concrete error for creator rollback.

The public function shape stays unchanged; only the W helper is used by our
installer. The A helper and original DetourCreateProcessWithDll wrappers remain
unselected for production cross-width installation. No new helper EXE,
component, broker role, frontend relationship or process-tree kill.

## Required evidence before closure

Both actual directions32-to64 and64-to32, plus retained same-width and pinned
context-only paths; matching DLL loaded before immediate descendants; A/W,
GUI/CUI, environment/handle-list/CWD, true exit/handles and caller suspension.
Use the existing creation callback seam for controlled helper create failure,
held helper timeout and target death. Prove finite return and owned helper
termination, no helper residue/handle growth, and caller rollback of only the
unpublished target. Missing/wrong DLL and partial installation remain failures,
not silently unhooked success. Record after-source hash/diff and native maps.
