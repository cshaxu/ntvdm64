# M0 T331 S2 — Original XMS CPU40/session binding

## Result

S2 closes the one missing live composition seam identified by S1.  The
original XMS allocator, dispatcher, A20 and UMB bodies remain their existing
source owners.  The selected CPU40 SAS address space is now the active
session guest-memory provider for each outer original SoftPC invocation.

`xms.486/i386/xmsmem86.c` remains unselected mirror evidence: its historical
implementation turns an Intel `ULONG` address into a process pointer.  The
same-named active callbacks in `adapter-mvdm-host-out/softpc/mvdm_xms_memory.c`
continue to acquire bounded session leases.  Those leases now copy through
the original CPU40 CCPU exports:

```text
c_sas_memory_size()                 -> range gate
c_sas_loads(address, bytes, count)  -> guest read
c_sas_stores(address, bytes, count) -> guest write
```

The adapter declares only these three verified fixed-width contracts instead
of including `sas.h`: that historical header requires a source-TU macro layer
which is not an adapter dependency.  No host pointer, Bochs input, second
guest memory backing, XMS policy, or synthetic service outcome was added.

`mvdm_softpc_execution_run_original_entry` and
`mvdm_softpc_execution_run_until_return` install the provider after binding
the selected session, then remove it on ordinary return, arm failure, and the
original controlled non-local return.  This makes lease lifetime match the
outer SoftPC invocation rather than an arbitrary BOP or host object lifetime.

## Formal verification

After regenerating the current formal graphs from
`tools/build/New-T310OriginalSoftpcNinja.ps1`, both commands completed:

```text
VsDevCmd -arch=x86 -host_arch=x64
ninja -C build/M0-T319/S3/termination-x86 original-softpc-candidate

VsDevCmd -arch=x64 -host_arch=x64
ninja -C build/M0-T319/S3/termination-x64 original-softpc-candidate
```

Both rebuilt `mvdm_softpc_guest_memory.c` and archived it in
`softpc-bindings.lib`; the candidate target completed.  This is a paired
source/link closure only.  It does not claim that a guest has yet exercised
every XMS dispatch slot; S3 owns that original package contract matrix.

## Disposition

The original A20, UMB and INT15 calls retain the machine-owner transfers
recorded in S1.  This S only restores the shared SAS-backed bounded-memory
precondition required before the original XMS/SubAlloc callbacks can operate.
