# T95 S7 Native Firmware-to-NTIO Handoff Design

## Historical Requirement

OpenNT does not boot DOS from media.  Its `InitialiseDosEmulation` calls the
SoftPC `reset()` path, then loads `NTIO.SYS` and assigns `CS:IP=0070:0000`.
The direct entry is therefore historical.  It is not, however, a license to
skip the machine state established by the preceding reset.

`reset.c` is explicit about that distinction.  Its build-selected paths
initialize interrupt vectors and then initialize PIC, CMOS, PPI/equipment,
timer, keyboard, video, DMA and storage/reset owners before OpenNT loads
NTIO.  The exact SoftPC vector contents are not transferable to Bochs: they
name SoftPC handlers and in some NTVDM/X86GFX configurations are established
by a separate video path.  Recreating them in adapter code would be a
replacement SoftPC, not a Bochs integration.

## Native Bochs Fact Base

The accepted `pc-reset-floor-v1` profile has already proven an intact Bochs
hardware reset while declaratively denying PCI/ACPI/PCI-IDE/VBE and attached
media.  It retains the original CPU, RAM, system/VGA ROM, CMOS, DMA, PIC, PIT,
floppy controller, core VGA, keyboard and one empty ATA-controller lifecycle
closure.  No new device is selected by this design.

The local Bochs sources also provide an unmodified native route for an
optional ROM:

- `main.cc` loads `optromimage1` and `optromimage2` through the existing ROM
  loader.
- `config.cc` parses `optromimageN: file=..., address=...` as ordinary Bochs
  configuration.
- `rombios.c` initializes the IVT, scans the VGA ROM, completes its device
  POST work, and then scans `C800:E000` optional ROMs before `INT 19h`.

The retained stock-firmware fixture proves that this ROM BIOS can reach a
neutral boot sector at `0000:7C00`; that fixture is evidence only, not a
product boot-disk proposal.

## Rejected Alternatives

| Alternative | Rejection reason |
| --- | --- |
| Adapter writes an INT 10h value or an IVT/BDA image | Reimplements selected SoftPC/BIOS state, leaves Bochs device state incoherent, and grows adapter into a machine owner. |
| Per-vector or per-device Bochs patches | Repeats the same problem as a pile of local compatibility patches; it is not a single native lifecycle. |
| Use a floppy/HDD boot sector as the product handoff | Makes a bootstrap medium guest-visible and adds a storage-capability contract unrelated to NTIO. |
| Execute full POST and fall into `INT 19h` | Still needs a deliberate, verified transfer to the historical direct NTIO entry. |

## Recommended Boundary: Native POST + Optional-ROM Handoff Marker

The proposed product path is a one-purpose, read-only optional ROM in the
standard `C800:E000` scan range.  It contains only a valid ROM envelope and a
single neutral stop instruction.  It neither implements an interrupt, device,
filesystem, BOP, DOS service, nor guest payload.  It is a composition asset,
not a Bochs or OpenNT source change.

```text
Bochs reset -> unmodified Bochs ROM POST / selected reset-floor devices
            -> VGA ROM initialization -> optional-ROM scan
            -> neutral marker -> one generic #UD handoff consumer
            -> adapter supplies its already-validated opaque startup plan
            -> generic Bochs RAM/state commit -> NTIO 0070:0000
```

The current pre-loop execution-plan helper cannot be reused unchanged: it
writes the payload and changes CS:RIP before firmware runs, while the existing
exception result ABI permits only low GPR/CF deltas and a RIP resume.  Neither
is sufficient for this transfer.  The next implementation therefore needs one
new, default-off **generic startup-plan handoff consumer** at the already
existing #UD seam.  It must perform the same plan preflight, capture/restore
the declared preserved bytes, copy the opaque payload, and apply only the
existing real-mode CS:RIP entry setter before it leaves the exception loop.

That consumer must remain unaware of the marker's address, ROM, firmware,
BOP, or OpenNT meaning.  The adapter initializes the selected session before
the CPU loop but holds its plan pending; it may arm one single-use pre-entry
phase only for a selected composition profile.  It validates the copied
exception record against the declared marker contract and cannot consume an
unmatched event.  Bochs continues to own CPU execution, RAM commit, devices,
firmware execution and all machine state.

## Admission Prerequisites

This is a design, not an implementation admission.  Before any code, build,
or runtime trace, the next packet must define:

1. the option-ROM byte layout, load address, checksum and fixed stop record;
2. the adapter's pre-loop session initialization plus one-shot plan-take ABI,
   and every rejection/teardown behavior;
3. proof that the selected CPU-level-5 reset-floor profile reaches the marker
   before `INT 19h`, with no boot image or host storage path;
4. the exact object/link closure and the required focused negative tests;
5. teardown/restart behavior so a failed POST or unmatched `#UD` cannot expose
   a partial NTIO plan.

Only after those conditions are admitted may the existing generic `#UD`
mechanics be reused.  No BOP implementation, host capability, filesystem,
CLI behavior, OpenNT edit, or additional Bochs device is authorized by this
design.

## Deferred-Plan Contract Shape

The next implementation packet must add exactly one profile object, named
`machine_startup_handoff`, alongside the existing `machine_startup_plan` and
`machine_startup_entry`.  Its closed v1 fields are `exception_vector`, the
two-byte `instruction_bytes`, `cs`, and `ip`; the selected optional-ROM marker
will declare `6`, `0f0b`, `ca80`, and `0003`.  The parser rejects every other
shape or value in v1.  Thus the address is a profile-owned adapter fact, never
a Bochs constant.

The initial `C800:0003` proposal was rejected by r38 before CPU execution:
the local 41,472-byte VGA ROM occupies Bochs's 2 KiB ROM pages through
`CA7FF`, and `load_ROM` correctly refuses a second mapping at `C8000`.  The
ROM BIOS's original general-ROM scan continues in 2 KiB steps; `CA8000`
(`CA80:0003`) is the first free aligned location.  This is a composition
placement correction, not a firmware, device, adapter-ownership, or guest
state change.

The adapter API is intentionally two-stage:

1. `prepare_deferred_startup_from_environment()` installs and validates the
   selected session but returns no payload and mutates no guest state.
2. `take_deferred_startup_plan(event, cpu, window, plan, payload, bytes)`
   succeeds once only when the copied exception record equals the declared
   contract.  It returns the unchanged `startup_plan_v1`; every mismatch or
   second call returns false.

The Bochs consumer receives only that plan/payload pair.  It has no profile
object and therefore cannot know why a boundary matched.  It must preflight
all ranges before any write, capture the existing preserved bytes, write the
payload, restore those bytes, apply the plan's existing CS:RIP delta, and
leave the exception loop.  A preflight/copy failure consumes no successful
handoff and must not create a fallback entry.
