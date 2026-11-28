# S15 P81 N024 taskman.c migration

Verified runtime97 after P80 HEAD981c01d948ac536f2e9886110824597075843717/runtime96.
N0247/9; S15 11/14; overall59/159; five delivered files since P75 Full.

## Source and scope

XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
Source/XPSP1/NT/windows/core/ntuser/kernel/taskman.c,
blob45a98a808bbbd24bb14810bb9a023fc7caedadc2.
OpenNT blob5974042373096e1a98231ba5a224ae3823b547b7.
Raw Git inputs and three-way audit: build/t485-s15/n024-taskman.

Restore XP priority scheduling, destruction and wait bodies. DestroyTask now
frees the linked TDB at removal and clears THREADINFO.ptdb; its fixture asserts
that postcondition. CurrentTaskLock is unused by production and excluded by
XP's own #if0; retain that condition with #error, remove obsolete declaration
and test calls. No guest source or binaries change.

Retain explicit task/domain context and native-stack readiness: a suspended
native callback frame cannot safely become the CPU execution owner. Existing
host wait and event providers remain the only mechanisms, with no new queue.
XP removes the old CheckForClientDeath callbacks; remove that obsolete provider
slot and keep worker validity at the actual native wait boundary.

XP ntcb.h:7843-7880 sends MEOW scheduling notification to the existing registered
client pointer, returning1 when absent. clglobal.c already owns that pointer;
WOW wow32.c registers NULL. Use the same receiver contract in C without a
second registry, queue or missing-provider success shim.

XP Setup task destruction posts DTM_SETUPAPPRAN (userk.h:3747,WM_USER+90) to
its desktop shell. Bind that three-line object access to GetShellWindow and
PostMessageW on the executing native desktop, never cast a kernel PWND to HWND.
No setup-name classifier is added; queue's current classification is unchanged.

XP retail BEGINATOMICCHECK is empty (userk.h:1935), paired with the already
empty ENDATOMICCHECK. UserAllocPoolNonPaged and zzzWakeInputIdle bind to the
same heap allocator and input-idle callback previously used by taskman.

## Local verification, not delivery

606 unchanged nonblank XP lines retain correct diagnostic mappings. Original
LF restored. One original #if0 branch excluded. Local nonerror diff:
OpenNT1point/1496lines -> XP17points/69lines; +16points/-1427lines, with the
old whole-file newline span now exposing real adapter locations.
Population1332/792byte-identical/803nonerror-identical/529different;
8028points/63075lines -> 8044points/61648lines. Baseline switch is file-local.
Cached full build launched PID21784. Publication/tests/commit/push pending.

First build failed C2373: the actual product precompiled headers already
provide pfnWowTask16SchedNotify and its original calling convention. Removed
the redundant declaration and two existing notification constants; reused
the original declarations rather than inventing a parallel ABI.

## Final delivery

[Receipt](n024-taskman-result.json) and [census](n024-taskman-census.json).
Cached full build, dual26hash, controls40/40, task-order, init-notification,
task-lifecycle, init-rollback and genuine Win16 controls pass.
No guest change or rebuild. Original line audit606matches, no mismatch.
One original #if0 exclusion; LF/EOF retained. Final nonerror byte-sensitive
and normalized metrics agree: 17points/66lines.
Whole census1332/792/803/529/8044/61645;
delta0/0/0/0/+16/-1430.
1321XP/11OpenNT. N0248/9; S15 11/14; overall59/159; six since P75 Full.
Next userk.h; no S/T closure.
