# M0 T198 S87: SPCKBD Stream-State Publication Transaction

## Result

The adapter now contains the source-derived, two-stage transaction for the
already selected `STREAM_IO=2` profile value.  It is not wired into the live
composition yet: S87 deliberately excludes CLI profile selection and runtime
binding.  Consequently, this is a source/test closure of the capability, not
a claim that the current CLI run selects stream I/O or produces output.

## Source Contract

- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/spckbd.asm` defines
  `STREAM_IO equ 2`, publishes `kio_table`, and writes the
  `use_host_int10` offset at `[si+34]` before the `C4 C4 5F` handoff.
- `src/opennt/base/mvdm/softpc.new/base/keymouse/keybd_io.c` consumes that
  word at table index 17 and stores `getNtScreenState()` at the resulting
  one-byte address.
- S86 selected the non-invasive value `2`; it did not authorize a text sink,
  BOP `42`, VGA, ROM, PIC, keyboard, timer, or a CLI option.

## Implementation Boundary

`src/bx-vdm/bx_ntvdm_spckbd_init_service.c` adds a closed API which accepts
only an explicit `BX_NTVDM_SPCKBD_DISPLAY_STREAM_IO` value.  It recognizes
only real-mode `#UD`, `AX=BEEF`, and bytes `C4 C4 5F`.

1. `prepare` derives physical `DS:SI + 34`, validates the two-byte range in
   the existing one-MiB ordinary-RAM aperture, and requests a checked read.
2. `complete` verifies the exact copied request/result, interprets the
   little-endian offset, validates `DS:offset` in the same aperture, and only
   then produces a one-byte typed write of `2` plus the existing CF and
   `RIP+3` continuation.

It takes neither a guest pointer nor an arbitrary address.  An invalid form,
undeclared mode, invalid table range, or invalid returned target returns no
success transaction.  The caller must execute a write only after a successful
complete result; the service has no direct guest-memory access.

## Verification

`tests/bx-vdm/bx_ntvdm_spckbd_stream_state_test.c` source-builds with MSVC
x64 `/MT` and verifies:

- `DS:SI+34` is read as two bytes, then yields exactly one same-DS target
  write of `2`;
- the completed result sets CF and resumes at `fault RIP + 3`;
- an absent stream declaration is rejected;
- a table read extending beyond the 20-bit aperture is rejected; and
- an otherwise valid table whose returned offset lies beyond the aperture is
  rejected without changing the caller's output payload.

The existing exact BOP-`5F` continuation test was rebuilt from the same
sources and still passes.

| Evidence | Result |
| --- | --- |
| `artifacts/build/t198-s87-spckbd-stream-state-r1/compile.log` | MSVC x64 `/MT` source build succeeds with `/W4 /WX`. |
| `artifacts/build/t198-s87-spckbd-stream-state-r1/run.log` | Focused transaction test exits `0`. |
| `artifacts/build/t198-s87-spckbd-stream-state-r1/continuation-compile.log` | Existing continuation regression source build succeeds. |
| `artifacts/build/t198-s87-spckbd-stream-state-r1/continuation-run.log` | Existing BOP-`5F` CF/RIP continuation regression exits `0`. |

## Limitation And Transfer

The current `byob_profile_selection` has no display-state member.  Making the
first CLI profile carry the fixed enum and binding it into the composition is
a new, separately admitted step.  Until then no live runtime path calls this
API, and no guest-memory write is performed by S87 itself.
