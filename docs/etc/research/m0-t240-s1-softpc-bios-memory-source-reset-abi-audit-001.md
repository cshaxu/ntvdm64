# M0 T240 S1 SoftPC BIOS Memory Source, Reset And ABI Audit

## Question

What is the smallest source-first route for the reached `C4 C4 12` operation
after `50:11`, and does it require a new mantle-owned conventional-memory
query or a broader PC machine feature?

## Inputs and procedure

- OpenNT guest caller: `base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm`.
- Original SoftPC owner: `softpc.new/base/bios/{bios.c,mem_size.c,reset.c}`
  and `base/inc/{bios.h,sas.h}`.
- Current sources: `src/bx-vdm/bx_ntvdm_bios_memory_service.c`,
  `src/bx-mantle/bx_ntvdm_minimal_machine.cc`, and the formal module manifest.
- Prior machine snapshot: `bop-12-memory-size-contract-r20.md`.

The audit traced the guest caller through the original table/handler, resolved
the data address and reset writer, then searched current production/test
routes and formal-manifest membership.

## Observations

1. `sysinit1.asm:goinit` deliberately substitutes `BOP 12h` for `INT 12h`,
   then shifts returned `AX` and stores the result as DOS `memory_size`.
2. `bios.c` maps entry `0x12` to `memory_size`. `mem_size.c` reads
   `MEMORY_VAR` before `setAX`; its comment explicitly says applications may
   write this area. Its observable contract is therefore a live mutable word,
   not a fixed profile constant.
3. `bios.h` resolves `MEMORY_VAR` to `BIOS_VAR_START + 0x13`, where segment
   `0040h` makes physical address `0x413`. `reset.c` writes that word through
   SAS using `host_get_memory_size()`, whose historical host header defines
   640 KiB.
4. The selected stock-Bochs first-profile snapshot instead records BDA bytes
   `7f 02`, namely 639 KiB. This is valid evidence of the current firmware's
   seed, but not authority to freeze future reads at `0x027f` or to replace
   the original mutable-word contract.
5. `bx_ntvdm_bios_memory_service_v1_dispatch` is a formal-manifest member,
   but it has no production caller: source search finds only its own fixture.
   It hard-codes `0x027f` and combines selector `12h` with a separate
   `15h/AH=88h` experiment. It is inactive evidence/fixture material, not an
   active source-shaped route.
6. The current mantle initializes native RAM and exposes generic checked RAM
   actions. It has no need to own or name the BDA word. A fixed-width checked
   read of physical address/length is sufficient for the source provider;
   no BOP, BIOS, DOS or OpenNT term crosses into mantle.

## Four-rung recovery decision

| Rung | Disposition | Reason |
| --- | --- | --- |
| 1. Direct original translation unit | Rejected for direct modern linkage | `mem_size.c` depends on historical `host_def`, `CpuH`, SAS and CCPU register APIs; importing the full historical product composition would violate the bounded modern runtime boundary. |
| 2. Original source through smallest shim | Selected | Mirror the tiny original read-then-`setAX` body. Replace only SAS read and CCPU AX write with a named bx-vdm shim over the existing checked RAM action and typed CPU result. Preserve BDA address, ordering, width and failure/decline behavior. |
| 3. External-code intrusion | Rejected | No Bochs change is needed: core/mantle already provide RAM lifecycle and generic checked access. |
| 4. Newly authored provider | Rejected | A hard-coded value or a new mantle query would lose the original mutable-BDA behavior without necessity. |

## Boundary and S2 recommendation

`bx-vdm` recognizes exact real-mode `C4 C4 12`, requests a generic checked
two-byte RAM read at `0x413`, and applies the source-shaped AX-only typed
result plus `RIP+3`. `bx-mantle` executes only the generic read; it does not
know BOP, BIOS, DOS, OpenNT, SoftPC or `0x413`'s meaning. The existing
firmware/reset lifecycle remains responsible for the first-profile seed until
a separately source-audited reset integration is admitted.

The S2 contract must test a changed guest BDA word followed by a query, proving
that the value is not cached or hard-coded. It must also reject inactive,
out-of-range, wrong-width and teardown reads. `15h/AH=88h` is explicitly
outside T240 and must be separated from the superseded helper during S3.

## Confidence and follow-up

High confidence for owner, caller, BDA address, return contract and current
helper inactivity: all are directly visible in primary source/current source.
Medium confidence for the chosen seed policy: the 639 KiB value is a recorded
stock-firmware profile observation, while the historical SoftPC macro says
640 KiB. T240 intentionally preserves the mutable BDA contract first and
does not silently decide to overwrite firmware state. A later reset-profile
task may choose and verify a different seed only with explicit source/profile
evidence.
