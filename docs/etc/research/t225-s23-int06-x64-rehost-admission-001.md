# T225 S23 P4 — INT 06 x64 Source-Derived Rehost Admission

## Decision

`src/bx-vdm/bx_ntvdm_machine_composition_v2.c` is not an admissible product
implementation.  Its optional path recognizes selectors, installs TLS callback
mechanics, and invokes external x86 `illegal_op_int`/`unexpected_int` objects
through legacy SAS/CCPU-shaped shims.  That mixes selector routing with the
historical host implementation and cannot be linked into the MSVC x64 `/MT`
product.

The S23 continuation therefore needs a small source-derived x64 implementation
of only the original `illegal_op_int` plus `unexpected_int` algorithm.  The
implementation is not a new interpretation: OpenNT source remains the
operation-order and error-contract authority.

## Recovery-ladder evidence

1. **Original source reuse:** the existing `src/opennt` historical gate builds
   unmodified `unexp_nt.c` with clang-cl/i686 as an object-only input.  Its
   three semantic imports are `_inb`, `_outb`, and `_c_sas_store`; see
   [T113 closure](../../../history/m0-t113-closure-20260812.md) and the
   [compile projection](t113-s1-unexpected-interrupt-compile-projection-001.md).
   `illegalp.c` belongs to the same default-disabled system-provider gate.
2. **Direct product composition:** rejected by concrete ABI evidence.  The
   historical island is i686 and requests dynamic `msvcrt.lib`; the formal
   runner is MSVC x64 `/MT`.  The prior CRT audit records the conflict and
   forbids treating an apparently linked research island as a general product
   pattern; see [T132 audit](t132-s1-original-handler-crt-island-audit-001.md)
   and [S22 build/ABI admission](t225-s22-int06-handler-build-abi-admission-001.md).
3. **Smallest compatible seam:** a source-derived rehost retains only the
   two functions' data order, byte layout, PIC transaction order, BDA address,
   and fail-closed outcome through fixed copied values.  It must not reproduce
   legacy SAS/CCPU globals or callbacks.
4. **External-code intrusion:** the needed native PIC lifecycle has already
   been separately admitted under `BX-MANTLE-082`; no new Bochs intrusion is
   implied by this handler rehost.
5. **New behavior:** none is admitted.  Any operation absent from the two
   original functions is out of scope rather than a reason to invent a
   substitute.
## Original algorithm, fixed in order

`illegalp.c:79-134` performs the following sequence:

1. Compute real-mode stack physical address from `SS:ESP`.
2. Read saved `IP` at offsets `0..1` and saved `CS` at offsets `2..3`.
3. Retain the historical diagnostic branch as observation only; it must not
   determine mutation or resume.
4. If saved `IP == FFFFh`, increment saved `CS` and store it first at `2..3`.
5. Increment saved `IP` and store it at `0..1`.
6. Invoke `unexpected_int` before the guest `iret_com` consumes the frame.

`unexp_nt.c:50-99` then:

1. Writes OCW3 `0Bh` to master command port `20h`, and reads master ISR from
   `20h`.
2. If ISR is zero, treats the event as software and changes the BDA cause to
   `FFh`; no PIC EOI or mask write occurs.
3. Otherwise reads master mask `21h`, combines it with ISR, clears cascade
   bit 2, samples slave ISR using `A0h`, and if nonzero reads/masks slave `A1h`
   and sends slave EOI at `A0h`.
4. Writes the master mask at `21h`, sends master EOI at `20h`, then stores the
   master ISR at physical BDA address `046Bh`.

All reads/writes use native machine resources.  A typed rehost must preflight
the four saved-frame bytes and BDA byte before any mutation, and must fail
closed (no resume) if an ordinary-RAM or native-port operation cannot occur.
It must never replace a PIC operation with the empty-port fallback.

## Ownership boundary

| Concern | Owner | Rule |
| --- | --- | --- |
| Recognize `C4 C4 06` and decide whether the machine route is admitted | `bx-vdm` | selector-only ingress; no PIC, frame, or OpenNT algorithm implementation. |
| Execute native PIC port traffic and checked ordinary-RAM accesses | `bx-mantle` | receives a fixed request and returns copied fixed result; no selector/BOP/OpenNT/DOS name. |
| Original handler algorithm and diagnostic disposition | future `src/opennt` host rehost unit | source-derived only, with no legacy x86 object, CCPU/SAS, TLS callback, or host-system dependency. |
| CPU `#UD` event and typed resume | `bx-core` | existing selector-blind event/outcome mechanism only. |

The current four-library Ninja manifest deliberately excludes an `opennt-host`
static library.  P4 therefore does **not** place source-derived OpenNT semantic
code into `bx-vdm` to bypass that omission.  The next implementation admission
must extend the formal module ownership deliberately (or prove a standalone
original host unit is directly composable), then route it through the existing
selector-only bx-vdm ingress.

## Explicit rejections

- No direct x86 compilation/link of `illegalp.c` or `unexp_nt.c`.
- No reuse or enabling of `bx_ntvdm_machine_composition_v2`.
- No callback, TLS active-mechanics, raw host pointer, or legacy SAS/CCPU ABI.
- No selector/BOP/OpenNT name in `bx-core` or `bx-mantle`.
- No added Bochs device, BIOS image, firmware table, or guest-byte workaround.
- No Direct/Readonly/Overlay policy branch: this handler mutates guest RAM and
  native PIC state only, not a host namespace.

## Admission consequence

The next S23 implementation unit must first provide a fixed mantle machine
request/result ABI that has no selector vocabulary, and a formal `opennt-host`
module admission with source/ABI/failure mapping.  Only then may the bx-vdm
selector route compose the complete pair and run its whole-package regression.