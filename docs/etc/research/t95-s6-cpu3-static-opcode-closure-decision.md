# T95 S6 CPU3 static opcode closure decision

## Question

Does the CPU3 projected configuration permit a source-object subset of Bochs
CPU instruction handlers, or must a minimal machine retain the original CPU
handler family even when its first profile does not expose modern CPU features?

## Inputs and procedure

The accepted CPU3 projection sets `BX_CPU_LEVEL=3`,
`BX_SUPPORT_X86_64=0`, `BX_SUPPORT_VMX=0` and `BX_SUPPORT_SVM=0`.
The earlier CPU3 link ledger nevertheless reported unresolved VMX, SVM, MSR,
SMX/GETSEC and ADX identities from `fetchdecode.obj`.

This audit inspected the original static opcode declarations in
`refs/bochs/cpu/ia_opcodes.h` and their decoder-table references in
`refs/bochs/cpu/fetchdecode.cc`. Representative direct references are:

- `fetchdecode.cc:509,514,1054,1059` for RDMSR and GETSEC dispatch;
- `ia_opcodes.h:723` for RDMSR;
- `ia_opcodes.h:1478-1496` for VMX;
- `ia_opcodes.h:1500-1513` for GETSEC and SVM; and
- `ia_opcodes.h:2215-2216` for ADCX/ADOX.

## Observation

`ia_opcodes.h` stores original member-function addresses in a generated static
opcode metadata family. The CPU profile/ISA discriminator belongs to opcode
availability/dispatch semantics; it does not eliminate every member-function
address from the compiled metadata. `fetchdecode.cc` in turn retains table
entries for operations such as RDMSR and GETSEC outside the observed CPU-level
conditional regions.

Consequently, removing those handler source objects leaves unresolved symbols
even though CPU3 will not expose the associated ISA semantics to the first
guest profile. The previously unresolved GETSEC identity is now source-located
as `BX_CPU_C::GETSEC` in the static opcode declaration, but this audit does
not assert a provider object beyond the original CPU source family.

## Decision

The Bochs CPU is an indivisible native execution subsystem for this project.
Any future minimal-machine build recipe must derive and retain the original
Bochs CPU handler/object family required by its own decoder metadata. It must
not solve a linker failure by editing the opcode table, replacing handlers,
inventing stubs, or enabling the corresponding guest feature. CPU3 remains a
profile policy for execution/CPUID behavior, not a license to reshape decoder
linkage.

This retains the user-required adoption boundary: Bochs owns CPU semantics;
the adapter and OpenNT own none of them. It shifts the remaining minimization
work to machine composition and product-shell retention, where a boundary can
be evidenced without changing the CPU decoder.

## Follow-up

Before a new link observation, derive the CPU source/object closure from
original Bochs build metadata rather than a hand-selected subset. Separately
retain the negative conclusions for the SIM virtual surface, `pc_system`
lifecycle, and logging `fatal` path until an original Bochs-local object
boundary is proven. No source, config, build, link or runtime action is
authorized by this decision.
