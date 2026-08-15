# M0 T205 S5 — C5 INT15 machine-capability admission decision

## Question

Can the admitted no-device Bochs machine provide the native interrupt lifecycle
required by XMS `52:09`, without reconstructing historical keyboard, PIC,
firmware, IVT or recursive-CPU composition in `bx-vdm`?

## Inputs

| Input | SHA-256 | Relevant fact |
| --- | --- | --- |
| `src/opennt/base/mvdm/xms.486/xmsmisc.c` | `10AD205D9DEDE714749A9746DD99B3389546B8AAF3A63DD0236386A9D4CE5245` | `xmsNotifyHookI15` calls `UpdateKbdInt15(CS, AX)` before returning XMS KiB in `CX`. |
| `src/opennt/base/mvdm/softpc.new/base/keymouse/keybd_io.c` | `C17523F12F895EB144B38692F4D91DC97D539106A8D3E4B8CF23BC4338CD4896` | The update reads IVT `15h`, retains keyboard callout state, and later selects `BIOS[0x15]` or historical recursive execution. |
| `src/opennt/base/mvdm/softpc.new/base/inc/tape_io.h` | `CCA3E19561B62ECEBE674F180FAF6D0760F581433563E84711DC742D871FB309` | INT15 spans BIOS wait, move, EMS, keyboard and device-completion behavior. |
| `src/bx-mantle/minimal_port_space.cc` | `9B0A3EF1B7C29E4372BBDC2CC7FFD9B4F7AAEC90AA8476222CC26B61895635B8` | The admitted mantle provides empty default ports and stubs, not PIC, keyboard or firmware delivery. |
| `src/bx-core/cpu/exception.cc` | `7A32AB0E4A6B0A1BF6F57959B43280390C5EF5C0EBF57F54AC47D9A3B78F0103` | The admitted #UD bridge is a CPU-fault seam, not a BIOS interrupt or IRQ mechanism. |

The prior T199 C5 map is retained corroborating evidence, not a substitute for
this current-packet decision: [T199 S26](t199-s26-c5-int15-owner-abi-failure-map-001.md).

## Procedure

1. Follow the original `52:09` handler into its first non-XMS owner.
2. Inspect the later use of the retained INT15 vector and identify all machine
   owners it assumes.
3. Compare those owners with the admitted no-device mantle lifecycle.
4. Reject any proposal that represents only `(CS, AX)` while silently creating
   firmware, IVT, PIC or keyboard state elsewhere.

## Observations

`xmsNotifyHookI15` has no standalone interrupt effect: its caller delegates to
`UpdateKbdInt15`.  That function verifies that IVT vector `15h` still equals
the previously installed keyboard handler before it caches the newly supplied
target.  The cache is subsequently consumed by `INT15`: `AH=4F/91` calls the
historical `BIOS[0x15]` handler; all other paths alter keyboard ownership,
load the historical INT15 entry, and call `host_simulate` recursively.

The required lifecycle is therefore the machine's IVT, keyboard, BIOS and
execution/interrupt composition, not merely the XMS capacity value returned
in `CX`.  The current mantle has deliberately admitted none of these owners:
its port space is default-unmapped, it initializes no keyboard/PIC/firmware,
and its generic #UD handling cannot serve as interrupt delivery.

## Decision

**C5 is not admitted for the current no-device profile.**  T205 creates no
INT15 callback record, IVT shadow, PIC/keyboard enablement, firmware table, or
adapter interrupt implementation.  XMS `52:09` remains explicitly deferred
as a machine-gated XMS service, with its existing deferred provider outcome.

The optional capacity result cannot be separated from the original callout
ownership without changing the service's observed meaning.  A future task may
admit this only by proving a first-profile native machine interrupt lifecycle,
its owner/reset behavior and default-off absence test; it may not import the
historical SoftPC keyboard or place its semantics in the adapter.

## Follow-up

T205 may now proceed to S6 family convergence: verify that all callable XMS
identities enter the single package session and that `06..09` retain their
explicitly deferred, non-success disposition.  A native trace remains outside
scope because the current no-device profile has not admitted either C3 or C5.
