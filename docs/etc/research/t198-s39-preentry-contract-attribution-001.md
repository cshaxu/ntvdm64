# T198 S39 NTIO pre-entry contract attribution 001

## Question

Can the current finite startup placement and preservation contract explain the
unexpected retained transfer `9346:4D53 -> 0000:0000` without inventing a
Bochs device, firmware behavior or BOP provider?

## Static placement ledger

The current `bx_ntvdm_ntio_preentry_v1_prepare` accepts the fixed 33,792-byte
NTIO image, copies it into the finite request at physical `0x0700`, enters
`0070:0000`, and declares a four-byte preserved span at `0x0714..0x0717`.
`bx_ntvdm_run_finite_bare_bytes` initializes its one-MiB bare machine, copies
that span out of ordinary RAM, writes the whole NTIO aperture
`0x0700..0x8AFF`, then writes the copied four bytes back before CPU entry.

This makes the preserved span live input, not a harmless overlap workaround.
The source-built NTIO ledger establishes that its first `FCLI` and later
`FSTI` consume that exact span before the first SVC.  The same first prefix
reads IVT entries at `0x40..0x6F` and BDA `0x496`; it writes some IVT entries
later.  Therefore all of these are pre-entry machine/profile inputs:

| Input | Current finite request | Source requirement |
| --- | --- | --- |
| IVT slice `0x40..0x6F` | no initialized input | read by NTIO initialization |
| BDA `0x496` | no initialized input | read by NTIO initialization |
| virtual interrupt `0x714..0x717` | copied from bare RAM and restored | read/written before first SVC |
| BDA memory KiB `0x413..0x414` | no initialized input | recorded later for original `12h` dependency |

## Available, profile-owned evidence

The pinned CPU5 stock-firmware evidence record
`adapter-r18-startup-snapshot-evidence-export.md` already supplies the exact
opaque inputs for the selected ROM/floppy profile:

| Physical range | Recorded bytes |
| --- | --- |
| `0x40..0x6F` | `4a0100c04df800f041f800f0fee300f039e700f059f800f02ee800f0d2ef00f0959600f0f2e600f06efe00f053ff00f0` |
| `0x496` | `10` |
| `0x714..0x717` | `000000c0` |

Its decoded vectors include `10h=c000:014a`, `15h=f000:f859`,
`19h=f000:e6f2`, and `1Bh=f000:ff53`.  These are profile evidence, not
Bochs constants and not new firmware implementation.

## Current deficiency and attribution

The bare-memory initializer allocates its RAM vector with `new Bit8u[...]`
and initializes ROM/bogus storage and metadata, but does not seed the listed
ordinary-RAM inputs.  The finite runner subsequently reads the four preserved
bytes before it has written any defined value there.  It also supplies no IVT
or BDA profile input.  The current fixture therefore does **not** meet the
already documented NTIO pre-entry contract.

S38's `9346:4D53` physical position (`0x981B3`) remains outside the NTIO
aperture, so this audit does not prove that a particular uninitialized value
created that transfer.  It does prove a more immediate correctness failure:
the current execution is not entitled to claim a valid NTIO startup path
until the complete evidenced pre-entry snapshot is installed.  The observed
transfer must consequently be classified as **startup-input contaminated**,
not as evidence for a missing device, firmware map, BOP handler or adapter
host service.

## Required successor boundary

The next implementation must recover the whole fixed startup-input set as one
profile-selected, opaque adapter-owned record and have the mantle apply its
checked ordinary-RAM writes before NTIO placement/entry.  It must reject
absent, malformed, mixed-profile or partially applied input.  Bochs core
remains selector- and profile-blind; the adapter may not synthesize BIOS or
DOS semantics.  A new task must define the fixed ABI, initialization order,
atomic preflight and negative tests before changing the runner.
