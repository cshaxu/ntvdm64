# Part 8: TOOLHELP actual guest build and WOW class thunk boundary

All 13 owned files match both frozen originals; the original TOOLHELP makefile's object list and direct rules select dllentry, module, notify2, task1, ththunks, timer and usergdi2, but comment out class1 and class2 rules. Its ththunks emits ClassFirst/ClassNext guest thunks. Current original WOW32 `wthtbl2.h` maps both names to `UNIMPLEMENTEDAPI`; this is a **source-table limitation**, not proof of the behavior of all TOOLHELP functions or of a particular runtime reachability. The observed `task1.c` TaskFirst/TaskNext validates entry state and calls TaskInfo; it is not the same as the class thunks. Captured ZIP TOOLHELP.DLL matches source-tree image, and NE name tables export TaskFirst @63, TaskNext @64, ClassFirst @69 and ClassNext @70. No guest binary was rebuilt or executed.

| Owned key | Evidence and bounded disposition |
| --- | --- |
| `wow16/toolhelp/class1.c` | Original class implementation, but its makefile rule is commented. **Pending** alternate profile and class function equivalence; don't claim generic TOOLHELP absent. |
| `wow16/toolhelp/class2.asm` | Same commented original rule. **Pending** alternate profile/guest class implementation. |
| `wow16/toolhelp/dllentry.asm` | Original recipe links dllentry.obj. **Historical guest build source** only; initialization/retirement untested. |
| `wow16/toolhelp/module.c` | Original recipe links module.obj. **Historical guest build source**; enumeration/handle reuse untested. |
| `wow16/toolhelp/notify2.asm` | Original recipe links notify2.obj. **Historical guest build source**; nested notifications and cleanup untested. |
| `wow16/toolhelp/string.h` | Original module.c/task1.c build dependencies name it. **Historical guest declaration**; actual semantics untested. |
| `wow16/toolhelp/task1.c` | Historical recipe links task1.obj; source TaskFirst/Next calls TaskInfo. **Historical guest build source**; actual TDB, termination and task enumeration remain unverified. |
| `wow16/toolhelp/ththunks.asm` | Historical recipe links ththunks.obj; emits class thunks into named WOW table entries above. **Historical guest build source**; table/image runtime binding remains unverified. |
| `wow16/toolhelp/timer.asm` | Historical recipe links timer.obj. **Historical guest build source**, timer callbacks and lifecycle untested. |
| `wow16/toolhelp/TOOLHELP.DLL` | Original/source-tree/ZIP bytes match; NE exports above confirmed. Candidate **partial interface/image evidence**, not live load or behavior. |
| `wow16/toolhelp/toolhelp.h` | Original module/task rules name it. **Historical guest declaration**, caller layout validation pending. |
| `wow16/toolhelp/TOOLHELP.RES` | Historical compiled resource artifact; exact recipe-to-binary provenance pending. |
| `wow16/toolhelp/usergdi2.asm` | Historical recipe links usergdi2.obj. **Historical guest build source**, cross-GDI behavior untested. |

The class table gap is not automatically fixed by importing the older class implementation or by finding the exports. Task termination capabilities and NTMON behavior must be assessed independently. Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-8/toolhelp-family-observer.py` and compare JSON. No guest, product, build tree or shared ledger changed.
