# M0 T385 S1–S4 — COMMAND initial-environment transient recovery

## Scope and fixed inputs

This record closes the changed-owner investigation from T384 without changing
CPU40, the immutable DOS/COMMAND media, the DOS allocator, or the `54:0F`
provider's environment construction.

- Formal CPU40/x86 product: `build/M0-T371/S2/formal-x86/original-softpc-process.exe`.
- Product SHA-256 after the selected recovery:
  `3474f94b3a468d8eb27fe65f5e675e9c00707e6b8e24dc5f7a1efd704e8835a7`.
- Fixed non-debug, console-owning stage: `O:\ntvdm64`.
- Declared guest command: `EXIT`.
- The observer supplied only
  `MVDM_COMMAND_CONTINUATION_REPORT_PATH`; app captures and removes that
  diagnostic variable before original environment enumeration.

## S1 — Original owner chain and scalar result

The source chain is intact:

1. `rdata.asm::EndInit` calls `INT 21h/AH=48h`, places the returned segment in
   `ES`, and invokes `CMDSVC SVC_GETINITENVIRONMENT` (`54:0F`).
2. DOS `$ALLOC` begins at `arena_head`; `msinit.asm` constructs the MCB from
   `endmem`; `sysinit1.asm` obtains conventional memory through BIOS `BOP 12h`.
   SoftPC reset/BIOS/SAS initialization supplies that conventional-memory
   model.
3. The original provider `cmdenv.c::cmdGetInitEnvironment` owns the request
   size and the `ES:0` byte sequence. `MVDM-HOST-DIV-193` retains its bounded
   session lease only as the former direct-address binding.

The new default-off `MVDM-HOST-DIV-199` scalar report established that the
first allocation was valid: `ES=049F`, `BX=0010`, `AX=049F`, `CF=0`. The
unchanged provider then returned `BX=0270` paragraphs. After guest
free/reallocation, the second request used `ES=049F`, `BX=0270`.

This disproves the proposed bad-MCB/invalid-lease explanation. It instead
identifies the original COMMAND transient hazard: its BOP-return instruction
at `03F4:2128` has linear address `0x6068`, inside the enlarged environment
destination `[0x49F0, 0x70F0)`. The historical grow-and-retry path therefore
overwrites code that must still execute after the provider returns.

## S2 — Selected source-shaped recovery

`cmdenv.c` now applies `MVDM-HOST-DIV-200` only at that insufficient-first-
buffer gate. Rather than returning the requested enlarged paragraph count, it
returns the original guest-defined `BX=0` disposition. `rdata.asm` already
defines this as `copy_old_env`: COMMAND retains its initial DOS environment.

This is not a new environment algorithm, CPU change, MCB replacement or
guest-media edit. It is a narrow modern-host admission: a large inherited host
environment is not copied when the original transient cannot safely survive
the required retry. Environments that fit the initial block remain on the
original construction and copy path.

## S3 — Formal closure

The formal CPU40/x86 Ninja target rebuilt `cmdenv.c`,
`original-mvdm-command.lib`, and the final product successfully. Existing
historical C-runtime warnings remain visible and were not suppressed; no new
compile or link error occurred.

## S4 — Fixed runtime result

The one post-recovery fixed observation exited normally (`exit=0`). Its
child-only scalar report records:

```text
MVDM-CMD-ENV svc=0F stage=0 es=049F bx=0010 ax=049F cf=0
MVDM-CMD-ENV svc=0F stage=1 es=049F bx=0000 ax=049F cf=0
MVDM-CMD-CONT svc=01 stage=0 cs=95EB ip=03C4 ax=0002 bx=0000 cf=0 first=1 repeat=0 dos-state=00000001
```

The remaining `54:01` records show original COMMAND delivery activity and no
longer show the prior `03F4:2128` `0x63` failure. This is startup and first
command-acquisition evidence only; it does not claim a general DOS workload,
DOS child return, or `cmdExec32` completion.

## Disposition

T385 is closed. Its next owner is Queue item 1, the complete original
`cmdExec32` stage-to-return package. WOW16/Win16 remains deferred.
