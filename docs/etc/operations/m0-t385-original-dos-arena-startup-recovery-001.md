# M0 T385 — Original DOS arena startup recovery

## Purpose

Restore the original DOS arena/MCB and SoftPC guest-memory preconditions that
allow COMMAND to allocate a non-resident initial environment, pass it to the
unchanged `54:0F` provider, and continue to the original first `54:01`
command acquisition.

## Ordered subtasks

1. **S1 — Original allocation and machine-memory owner ledger.** Map the
   immutable DOS `INT 21h/AH=48h` caller, `$ALLOC`, `sysinit` MCB construction,
   `endmem`, BIOS `BOP 12h` memory-size result, and reached CPU40/SAS
   initialization path. Add a default-off scalar-only record at the existing
   `54:0F` table boundary for `ES`, `BX`, `AX` and `CF`; it must not read
   command bytes or alter guest state.
2. **S2 — Source-shaped arena/memory prerequisite recovery.** Recover the one
   proved original DOS or SoftPC initialization prerequisite. Preserve the
   guest allocator, COMMAND image and `cmdGetInitEnvironment` contract. A
   same-shaped SoftPC binding is permitted only after the source-recovery
   ledger rules out direct composition.
3. **S3 — Formal x86 link and allocator contract proof.** Build only the
   affected formal CPU40/x86 closure and prove the selected MCB/allocator and
   `54:0F` scalar directions locally, including an invalid-span or
   invalid-arena direction where the original failure remains observable.
4. **S4 — Fixed pure-DOS observation.** Reuse the existing non-debug fixed
   stage observer once. Success is a post-`54:0F` first `54:01` boundary or an
   earlier, source-owned non-corruption failure. Native child execution starts
   only after this pure-DOS predecessor has closure; WOW16/Win16 is deferred.

## Boundaries

This is not a COMMAND provider rewrite, guest-media edit, error-dialog
workaround, CPU opcode patch, BaseSrv repair, `cmdExec32`, DOS PSP/child return,
WOW16, Redirector, CPU30, Bochs or x64 task. The retained owners are original
DOS allocator/sysinit plus original SoftPC/SAS guest-memory initialization.

## Completion handoff

After S4 reaches the first `54:01` or identifies its next source owner without
resident corruption, return to Queue item 1: original COMMAND `cmdExec32`
stage-to-return recovery. Only then may the project prove DOS-initiated native
32-bit child execution.
