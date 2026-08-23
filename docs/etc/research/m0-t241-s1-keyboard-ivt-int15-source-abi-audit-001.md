# M0 T241 S1 Keyboard/IVT/INT15 Source And ABI Audit

## Question

What is the smallest source-faithful route that lets original XMS `52:09`
(`xmsNotifyHookI15`) execute without turning the existing `UpdateKbdInt15`
link-only no-op into a false success?

## Inputs and procedure

- Original XMS: `src/opennt/base/mvdm/xms.486/{xmsdisp.c,xmsmisc.c}`.
- Original SoftPC keyboard owner:
  `src/opennt/base/mvdm/softpc.new/base/keymouse/keybd_io.c`.
- Current imports/shims: `src/bx-vdm/bop/opennt/xms/xmsmisc.c` and
  `src/bx-vdm/bop/shim/xms_shim.{c,h}`.
- Existing machine lifecycle and generic checked-RAM action, plus the live
  tracker rows `BOP-XMS-52-09` and `BOP-DEPENDENCY-039/044`.

## Original contract

1. `xmsdisp.c` maps XMS service 9 to `xmsNotifyHookI15`.
2. `xmsmisc.c` executes, in order:

   ```c
   UpdateKbdInt15(getCS(), getAX());
   setCX((USHORT)xmsMemorySize);
   ```

   It has no failure register path. Thus a successful source invocation must
   update the keyboard INT15 optimization state before it reports CX.
3. `keybd_io.c:UpdateKbdInt15` reads IVT vector 15 from physical bytes
   `0x54..0x57`; it compares the little-endian offset/segment pair against
   the keyboard owner's saved `sp_int15_handler_off/seg`. If either differs,
   it clears both saved words; otherwise it stores the supplied `CS:AX` pair.
4. `keybd_io.c:kb_setup_vectors` establishes the saved pair from IVT 15,
   alongside unrelated timer, disk, keyboard, mouse and video state. Later
   `INT15()` consumes the pair only for keyboard-specific AH `4Fh/91h`
   optimization; other cases recurse through historical `host_simulate`.

## Four-rung disposition

| Rung | Result |
| --- | --- |
| Original translation unit | `xmsmisc.c` is already directly mirrored and composable. Full `keybd_io.c` is not: it imports SAS/CPU globals, PPI, keyboard hardware ownership, timer/idle, mouse/video and `host_simulate`; importing it would violate T241 scope. |
| Smallest original fragment + shim | Admissible S2 candidate: a direct mirror of only `UpdateKbdInt15` with a named shim for its two IVT reads and saved-pair lifecycle. The mirror keeps the original comparison, clear and update ordering. |
| Bochs intrusion | Not indicated. Existing checked RAM can read the IVT, but it cannot own the keyboard-specific saved pair across BOP calls and reset. A new **mantle-owned, selector-blind IVT-watch capability** may be needed; it carries only vector number and fixed-width segment/offset values, no BOP/XMS/DOS/OpenNT/SoftPC vocabulary. This is mantle composition code, not an adopted-Bochs intrusion. |
| New behavior | Rejected. A bx-vdm no-op or direct CX-only resume would fake success and discard the original state transition. |

## Required S2 boundary

The candidate capability is deliberately smaller than a keyboard device:

- stage initialization samples one configured IVT vector into an opaque
  expected pair;
- update atomically re-reads that vector, compares it with the expected pair,
  then either clears the opaque pair or replaces it with the supplied pair;
- reset discards state; no port, IRQ, queue, host input, timer, CPU recursion
  or callback is exposed.

`bx-vdm` owns recognition of `52:09` and invokes the existing imported XMS
body. The source mirror invokes its named `UpdateKbdInt15` shim, which calls
the opaque capability. `bx-mantle` cannot mention XMS, BOP, DOS or SoftPC;
the eventual keyboard consumer is a later owner package and must not be
smuggled into this S2.

## Failure rule and tests

The historical `UpdateKbdInt15` itself has no return error. In this modern
composition, an inactive stage, unreadable IVT or unavailable capability must
make the whole `52:09` provider decline before `XMSDispatch`; it must not set
CX. Once active, matching and mismatching IVT cases must both execute the
mirrored function body: the first updates the opaque pair and returns source
CX; the second clears it and also returns source CX. Tests must cover initial
sampling, both branches, reset/teardown, malformed/non-real BOP decline,
AX/CS input versus CX output, and a vocabulary scan of bx-core/mantle.

## S2 recommendation

Admit the narrow source mirror and opaque IVT-watch lifecycle only. Do not
admit keyboard controller, `BOP 5C`, COMMAND input, `host_simulate`, IRQ1,
INT15 execution, or a general IVT service. This resolves the XMS source body
and preserves the later keyboard owner boundary rather than reimplementing a
virtual machine in the adapter.
