# R3 Original Bootstrap Activation Ledger

Status: source-derived activation plan, 2026-08-07. This record does not
authorize a runnable engine or a new bootstrap adapter.

## Purpose

The next executable research cut must enter the fixed NTIO image at
`0070:0000` only after the historical initialization owners have run or an
explicitly admitted lower host dependency has failed closed. It must not reuse
the frozen CCPU lifecycle harness, self-authored IVT/BDA profile, individual
BOP/DEM handlers, or NTIO-to-NTDOS ladder as a runtime substitute.

The selected execution path remains:

```text
CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS
```

## Historical Order And Ownership

`softpc.new/host/src/nt_msscs.c:InitialiseDosEmulation` supplies the final
DOS-emulation ordering reference, but it is not the process initialization
root. `base/support/main.c` calls it only after `gfi_init`, virtual-driver
initialization, `config`, CMOS handling, `cpu_init`, and video-global/screen
setup. In particular, `host/src/config.c:config` calls `sas_init`, and original
`ccpu386/ccpusas4.c:sas_init` installs the SAS vector, allocates the CCPU
M-area, connects memory, and calls `rom_init`/`copyROM`. An R3 target must not
skip those prerequisites by calling `InitialiseDosEmulation` directly.

Once that prerequisite prefix has completed, the historical DOS-emulation
subsequence is:

```text
read fixed virtual-interrupt word
-> io_init
-> reset
-> SetupInstallableVDD
-> scs_init (CMDInit -> DemInit -> XMSInit -> DBGInit)
-> copy ntio.sys to 0070:0000
-> restore virtual-interrupt word
-> set CS:IP = 0070:0000
```

The following responsibilities must remain with their original owners:

| Operation | Historical owner | R3 treatment |
| --- | --- | --- |
| Fixed virtual-interrupt word at `0070:0014` | `InitialiseDosEmulation` plus NTIO image protocol | Preserve/restore only after a complete mapping and image-copy preflight. |
| Device I/O initialization | `io_init` | Run the original provider or stop at its first unavailable lower host dependency. |
| Low RAM clear, IVT setup, ICA, CMOS, PPI, equipment, memory-size, timer, keyboard, and video initialization | `reset` and the original provider libraries | Do not replace with scaffold bytes or profile success responses. The first unadmitted provider/callback is a trace stop. |
| CMD/DEM/XMS/DBG shared state initialization | `scs_init` and its original callees | Preserve the historical call order. Do not recreate DEM globals, SVC tables, PDB/SFT/DTA state, or XMS policy outside those owners. |
| NTDOS load request | NTIO `BOP 50h/SVC 11h`, `MS_bop_0`, `DemDispatch`, `demLoadDos` | The original chain must perform it. A runner may supply only a preflighted resource root at the admitted `DemInit` boundary. |

## Calls That Cannot Be Adopted Unchanged

`InitialiseDosEmulation` is a historical product entry, not the modern CLI
entry. The following calls remain excluded from the first non-invasive cut:

| Source call | Reason | Required replacement boundary |
| --- | --- | --- |
| `GetSystemDirectory() + "\\ntio.sys"` | Ambient installed-file discovery conflicts with BYOB containment. | Per-session, manifest-validated runtime root. The runner supplies the root only; it does not replace historical placement or SVC handling. |
| `CreateFile`/`ReadFile` of NTIO inside `InitialiseDosEmulation` | The source hardcodes the forbidden root and performs non-transactional product assembly. | Modern outer runner validates and atomically stages explicit BYOB bytes before CCPU entry, preserving the source-defined load address and virtual-interrupt restoration sequence. |
| `SetupInstallableVDD()` | Old installable VDD policy is not required for the first CCPU CLI profile. | Explicitly unavailable unless a selected original caller reaches a separately admitted VDD provider. |
| Priority/WOW adjustments | Historical process-shell policy. | Not part of the direct CLI session contract. |
| `DemInit` system-directory acquisition | Same ambient-file policy violation; it later controls `demLoadDos`. | The already recorded R2.2 per-session runtime-root seam, only after a live NTIO prefix reaches original `SVC_DEMLOADDOS`. |

The outer runner may perform no other device, BIOS, BOP, DEM, or DOS behavior.

## Activation Preconditions

Before a real NTIO prefix trace can start, all of these conditions must be
checked transactionally:

1. the pre-`InitialiseDosEmulation` original startup prefix has reached and
   completed original CCPU/SAS/ROM initialization, or stopped at its first
   unadmitted lower dependency;
2. a selected private BYOB manifest is accepted before guest memory mutation;
3. ordinary writable RAM includes the full `0070:0000` NTIO image range and
   the fixed four-byte virtual-interrupt word; no ROM/provider overlap exists;
4. the original direct-CCPU host foundation has initialized its SAS/context
   lifecycle and reports no unadmitted callback during setup;
5. `io_init` and `reset` are invoked through their original owners, not a
   frozen scaffold; and
6. all required register/BDA/media inputs are either source-backed profile
   values or an explicit `bootstrap_input_unproven` stop.

## First Execution Boundary

The first R3 executable target is deliberately not NTDOS loading. It must:

1. build the complete mapping and state plan without committing it;
2. commit once, execute original initialization through `reset`, then load
   NTIO at `0070:0000` and restore the virtual-interrupt word;
3. start original CCPU at `0070:0000` and retain the complete original
   `BIOS[]` table; and
4. emit a redacted trace then stop at the first missing original provider or
   admitted lower host callback.

No BOP/SVC result, interrupt response, BIOS vector, disk/media response, or
DOS return may be supplied merely to continue. The trace must state the
historical caller that reached the missing boundary, the current CS:IP, and
only redacted role/address/length metadata.

## Current Admission Result

NTIO and NTDOS now have managed source-to-image rebuild evidence, and NTIO's
`off_d0` post-link layout requirement is proven. They do not yet satisfy the
activation preconditions above. In particular, the frozen zero-memory and
scaffold-IVT probes in `OPENNT-NTIO-ENTRY-STATE-CARD.md` are negative evidence,
not R3 initialization inputs. The next implementation task is therefore the
direct-CCPU original-initialization link/host-foundation audit, not another
NTIO service handler.

### DPMI Mapper And Dispatch Selection

The complete original BIOS table retains `MS_bop_3`, whose original body in
`nt_bop.c` calls `DpmiDispatch`. Therefore a no-DPMI closure is a useful
negative diagnostic but cannot become the full BIOS[] runtime link image. It
must not be "fixed" with a local dispatch stub.

The initial broad direct-CCPU closure selected both a SIM32-enabled
`host/src/sim32.c` object and `dpmi32/i386/dpmi386.c`; both define
`Sim32pGetVDMPointer` and `ExpSim32GetVDMPointer`, producing a duplicate-owner
failure. The original host `sources` descriptor includes `sim32.c` with no
`SIM32` define, while the DPMI32 descriptor selects its i386 mapper. The
SIM32-enabled object is therefore trace-only, not the full historical link
role.

The corrected broad diagnostic now uses the original inactive host `sim32.c`
role plus the original DPMI32 mapper. It no longer has the duplicate-symbol
failure and, in the current captured link log, reaches 149 unresolved host/ABI
dependencies. DPMI is required as
a link owner for the complete table, but R3 still treats every DPMI BOP and
protected-mode transition as unavailable unless a later original caller reaches
it and its separate feature contract is admitted.
