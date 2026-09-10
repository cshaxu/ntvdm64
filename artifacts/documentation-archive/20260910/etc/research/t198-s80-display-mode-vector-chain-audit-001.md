# M0 T198 S80: Display-Mode and Native-INT10 Vector Chain Audit

## Question

Does the observed transfer to `C000:014A` require a Bochs VGA machine slice,
or does it expose an omitted OpenNT display-mode composition step that has a
contained CLI-shaped alternative?

## Inputs

- Runtime facts from S74/S78: `AX=0E43h`, predecessor far transfer from
  `073B:0939`, then `#UD` at `C000:014A`.
- OpenNT `dos/v86/doskrnl/bios/spckbd.asm` and `sysinit1.asm`.
- Historical host `nt_bop.c`, `keybd_io.c`, `nt_reset.c`, `nt_fulsc.c`, and
  SoftPC BOP table in `base/bios/bios.c`.

## Procedure

1. Follow NTIO's resident keyboard/video table from its BOP `5F` call through
   the historical dispatcher and consumer.
2. Compare all three original screen states with the resident INT 10 branch.
3. Correlate the observed `AX` and far transfer with source semantics without
   assigning an unsupported source offset to `073B:0939`.
4. Separate the CLI-relevant policy choice from Bochs machine ownership.

## Source-Backed Chain

1. `spckbd.asm` initializes a 33-word `kio_table`, including offsets for the
   resident INT 10 caller/vector and the one-byte `use_host_int10` field, then
   invokes BOP `5F` with version `BEEFh`.
2. The historical SoftPC BOP table maps `5F` to `MS_bop_F`; historical
   `nt_bop.c` calls `kb_setup_vectors`.
3. Under the historical monitor composition, `kb_setup_vectors` derives the
   resident table's physical addresses from the BOP CPU state, validates its
   version, remembers those offsets, and writes `getNtScreenState()` to the
   resident `use_host_int10` byte. It also performs timer/keyboard/interrupt
   lifecycle work, so `nt_bop.c` is source authority, not a direct modern link
   candidate.
4. `getNtScreenState()` returns the old NT host console state: `WINDOWED=0`,
   `FULLSCREEN=1`, or `STREAM_IO=2`. `host_reset()` selects `STREAM_IO` when
   the old host stream-I/O facility is enabled; otherwise its Win32 console
   setup selects windowed or fullscreen behavior.
5. The resident `int10h_vector` branches on that byte:

   | State | Original branch | Resulting dependency |
   | --- | --- | --- |
   | `FULLSCREEN` (1) | Far-jump through saved `host_int10` | Existing native INT 10 vector / C000 video ROM |
   | `WINDOWED` (0) | BOP `42` for normal video requests | SoftPC EGA/video host plane |
   | `STREAM_IO` (2) | `sw_video_io` handles `AH=0E` or `AX=13FF` in resident code | Guest B000 stream buffer; BOP `42` only for overflow or unsupported/mode-changing requests |

6. The observed `AX=0E43h` is exactly the `AH=0E` teletype case accepted by
   `sw_video_io`. The default resident byte is `FULLSCREEN`, and the
   fullscreen branch is a far jump through the saved original INT 10 vector.
   This gives a strong semantic explanation for the C000 transfer. It is not
   an exact address attribution: no retained listing/map binds `073B:0939` to
   the `jmp_native` instruction.

## Ownership And Boundary

The policy decision is historical host-display composition, not an intrinsic
CPU, Bochs or DOS-kernel rule. A future CLI profile may define an explicit,
contained display disposition only by preserving the original BOP-`5F`
table/return contract and documenting the substituted host capability. It may
not set the resident byte as an out-of-band guest patch.

`STREAM_IO` is a promising minimum-profile direction for the reached
teletype-only path because the guest-resident handler owns that fast path; it
does **not** close all video behavior. Buffer overflow, mode changes, other
INT 10 functions and BOP `42` remain a separate SoftPC video-plane boundary.
`FULLSCREEN` instead requires a source-authorized C000 video ROM and the
admitted Bochs VGA/device mechanics proven absent in S79.

## Disposition

S80 proves that the C000 transfer is a missing historical display-mode
composition, not immediate evidence that the minimal machine must import a
VGA device. It does not choose a CLI state, implement BOP `5F`, enable BOP
`42`, alter NTIO bytes, or claim an exact source location for `073B:0939`.

## Interpretation And Confidence

High confidence: BOP `5F` is the original initialization seam for
`use_host_int10`, and its three-state disposition explains the two different
machine/video directions. High confidence: the current `AH=0E` is within the
resident stream-I/O fast-path contract. Medium confidence: the exact current
run missed the intended BOP-`5F` host composition rather than receiving a
different state; a focused pre/post byte observation is required before any
provider work.

## Follow-up

The next task must observe the resident `use_host_int10` byte immediately
before and after canonical BOP `5F`, without changing it, and classify whether
the current composition reaches, declines or bypasses that original
initialization call. Only then may a holistic BOP-`5F` machine-composition
provider or a VGA feature package be considered.
