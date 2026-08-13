# T198 S21 NTIO Pre-Entry Contract Map 001

## Question

What is the smallest selector-blind contract needed to enter the original NTIO
prefix with the current minimal machine, without importing historical CCPU/
Win32 startup composition or inventing BIOS/DOS semantics?

## Source-Locked Sequence

`src/opennt/base/mvdm/softpc.new/host/src/nt_msscs.c:114-221` performs the
historical order:

```text
read fixed virtual-interrupt word at 0x714
normalize its MIPS bit
reset machine
read NTIO.SYS to 0070:0000 (physical 0x700)
restore the saved word at 0x714
set CS:IP to 0070:0000
start CPU
```

The original code reaches these steps through CCPU/SAS pointers, a system
directory path, Win32 HANDLE I/O and VDD/reset product lifecycle.  Those are
not reusable runtime inputs.  The source establishes only ordering, byte
ownership and entry semantics.

The selected source-built NTIO role is 33,792 bytes (`0x8400`), and its fixed
research identity is recorded in `first-guest-startup-descriptor-v0.md`.
`0070:0000` begins with `E9 6D 03`, transferring to the original `init` at
offset `0x370`.  That prefix reads and changes the *guest-memory* word at
`0x714`, saves IVT entries `10h/15h/19h/1Bh`, installs its own guest vectors,
and reaches `50:11`; those are guest execution facts, not host actions.

## Current Mechanical Capability

`bx_ntvdm_run_finite_bare_bytes` already orders machine initialization,
checked `copy_to_ordinary_ram`, PC-time initialization, real-mode CS:IP entry,
bounded run and cleanup.  It cannot represent two independent pre-entry
publications or preserve a read value across reset: its `entry_bytes` are both
the only publication and the executable entry payload.

The existing mantle mechanical-action ABI is a better boundary: it transports
only checked physical ranges and copied bytes.  It has no BOP, OpenNT, DOS,
BIOS or host-path meaning and can preflight all ranges before any mutation.

## Required Next ABI

S22 may replace the fixture-private single-entry copy with a versioned
**pre-entry request** consumed by the mantle before `apply_real_mode_entry`:

| Field | Owner | Rule |
| --- | --- | --- |
| Immutable NTIO image and its profile identity | `bx-vdm`/OpenNT input boundary | Selected through `byob_image_load_exact`; no path or HANDLE crosses into mantle |
| Pre-entry writes | `bx-vdm` prepares; mantle executes | One action list, preflighted as a whole: NTIO at `0x700`, then restoration of the four source-authorized bytes at `0x714` |
| Preserved word | mantle read/action result, retained only for this request | Read from reset machine before NTIO write; no guessed value and no host-global virtual-interrupt flag |
| CPU entry delta | mantle | Exactly real-mode `CS=0070`, `IP=0000`; all other post-reset visible state remains untouched |
| Stop boundary | adapter test fixture | Bound execution at copied `50:11`, or controlled stop before a BOP if a precondition fails |

The read and both writes must be preflighted before the first write.  A failed
read, invalid identity, oversize image or overlapping/out-of-aperture range
must abort without publishing NTIO or changing the entry state.

## Exclusions And First Stop

S22 must not supply IVT/BDA values, execute guest initialization manually,
answer selector `12`, add a device, reuse the old startup bridge, or capture a
historical CCPU register candidate as a new reset contract.  The native Bochs
reset state remains owner-controlled; only `CS:IP` changes at entry.

The first acceptable runtime witness is therefore:

```text
minimal Bochs reset
  -> read/reset-owned 0x714 bytes
  -> all-or-nothing NTIO + 0x714 publication
  -> enter 0070:0000 with other state preserved
  -> original NTIO reaches the already-admitted 50:11 provider
  -> bounded stop before selector 12 is interpreted
```

That proves startup composition and the first BOP handoff, not a DOS boot.
