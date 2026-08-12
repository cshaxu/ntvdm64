# M72 NT4 Boot Memory BOP ABI Ledger

Status: source-derived startup-memory contract, 2026-08-08.

## Scope

This record narrows the two early memory queries in the fixed NT4 startup
path. It does not implement a BOP handler, a BIOS service, or a hardware
memory probe. The historical selector values remain profile facts; a future
machine backend must expose only neutral configured-decode machinery.

## Sources

- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm`
- `src/opennt/base/mvdm/softpc.new/base/bios/bios.c`
- `src/opennt/base/mvdm/softpc.new/base/bios/mem_size.c`
- `src/opennt/base/mvdm/softpc.new/base/bios/tape_io.c`
- `src/opennt/base/mvdm/softpc.new/base/inc/tape_io.h`
- `src/opennt/base/mvdm/softpc.new/base/bios/reset.c`
- `src/opennt/base/mvdm/softpc.new/base/inc/bios.h`
- `src/opennt/base/mvdm/softpc.new/host/inc/host_gen.h`

## Historical Dispatch Ownership

The original BIOS BOP table binds selector `12h` to `memory_size` and selector
`15h` to `cassette_io`. They correspond to the INT 12 and INT 15 service
families in the original machine profile; they are not generic core API names.

`reset.c` seeds the BIOS data area word at `MEMORY_VAR`, defined as
`BIOS_VAR_START + 13h`, from `host_get_memory_size()`. The selected historical
host header supplies `640` KiB. `memory_size()` reads that mutable BDA word
through SAS and returns it in AX. The source explicitly notes that applications
can alter this location, so the response is not an immutable host-RAM capacity
query.

## Guest Call Contracts

| Call site | Historical request | Historical result used by NTIO/NTDOS | Error behavior used by this path |
| --- | --- | --- | --- |
| `sysinit1.asm:goinit` | `BOP 12h` | AX is conventional memory in KiB. The guest shifts AX left six bits to form 16-byte paragraph units, reserves its arena, then records `memory_size`. | no flags or error result inspected |
| `sysinit1.asm` extended-memory initialization | `AH = 88h`; `BOP 15h` | AX is the extended-memory amount in KiB and is saved to `sysi_ext_mem`. | source comments out the carry check; AX is consumed unconditionally |

For the second row, `tape_io.h` defines `INT15_EMS_DETERMINE` as `88h`.
`cassette_io()` handles that case and, in the non-PM branch selected by this
real-mode startup path, sets AX to zero. Thus the exact historical code permits
the fixed guest to proceed without extended memory at this point; it does not
prove that CMOS/RTC implementation, a full INT 15 service family, or protected
mode transition is a first-prefix requirement.

## Boundary Result

1. The future profile adapter must supply the two fixed-profile responses only
   after the governed configured-decode facility exists. It may not expose
   selector `12h`, selector `15h`, `AH=88h`, or BDA offsets in a generic core
   interface.
2. Conventional-memory quantity, BDA initialization bytes, and the choice to
   advertise zero/nonzero extended memory are profile/topology policy. Core
   provides memory mapping, real-mode execution, A20 semantics, and checked
   access; it does not manufacture a PC/AT memory-size answer.
3. A20 is not justified by either query alone. It remains a generic machine
   semantic justified independently by the loader/HMA path recorded in M71.
4. CMOS/RTC, FDC/HDC, and the remaining INT 15 cases remain trace-gated. The
   original `cassette_io()` contains many such cases, but static inclusion in
   its switch is not evidence that the selected noninteractive profile reaches
   them.

## Consequence For Admission

M72 changes no current backend gate. A first controlled startup fixture needs
the M70 configured-decode observation to deliver copied register results and
the M70 ordinary-RAM transaction to publish NTDOS safely. Once those exist, the
fixture can record whether the selected profile accepts the historical zero
extended-memory response or reaches XMS/HMA work that needs further support.
