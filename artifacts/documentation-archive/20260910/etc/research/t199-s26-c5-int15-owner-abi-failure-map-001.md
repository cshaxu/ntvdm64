# M0 T199 S26 C5 INT15 and Fault-Delivery Owner, ABI, and Failure Map

## Question

Can C5 provide XMS `52:09` INT15-hook notification through the minimal machine without recreating SoftPC keyboard, firmware, or PIC composition?

## Inputs

| Input | SHA-256 | Relevant fact |
| --- | --- | --- |
| `xmsmisc.c` | `10AD205D9DEDE714749A9746DD99B3389546B8AAF3A63DD0236386A9D4CE5245` | `xmsNotifyHookI15` calls `UpdateKbdInt15(CS, AX)` and returns XMS size in `CX`. |
| `keybd_io.c` | `C17523F12F895EB144B38692F4D91DC97D539106A8D3E4B8CF23BC4338CD4896` | Update validates IVT entry 15, stores SoftPC keyboard callout state, then selects `BIOS[0x15]` or recursive CPU behavior. |
| `tape_io.h` | `CCA3E19561B62ECEBE674F180FAF6D0760F581433563E84711DC742D871FB309` | INT15 is a broad BIOS family including wait, move, EMS, keyboard and device completion. |
| `minimal_port_space.cc` | `9B0A3EF1B7C29E4372BBDC2CC7FFD9B4F7AAEC90AA8476222CC26B61895635B8` | The finite machine has empty ports and stub device models, not PIC/keyboard/firmware delivery. |
| `exception.cc` | `7A32AB0E4A6B0A1BF6F57959B43280390C5EF5C0EBF57F54AC47D9A3B78F0103` | Generic #UD is a CPU fault seam, not hardware IRQ/BIOS delivery. |

## Dependency Map

| Requirement | Historical owner | Minimum-machine fact | Disposition |
| --- | --- | --- | --- |
| `(CS:AX)` INT15 vector and XMS KiB result | `xmsNotifyHookI15` | No XMS provider result ABI exists | Deferred with XMS. |
| Verify vector 15 before caching | `UpdateKbdInt15` plus SAS IVT | No firmware/IVT lifecycle | Not admissible. |
| Use cached handler for `AH=4F/91` | SoftPC keyboard, `BIOS[0x15]`, recursive CPU | No keyboard, PIC or firmware | Not admissible. |
| Wait and device completion | SoftPC BIOS device paths | Deliberately absent no-device owners | Excluded. |
| CPU faults | Bochs core | Native faults are not BIOS vectors or PIC | Retain core ownership. |

## Admission Decision

**C5 is deferred, not implemented.** `52:09` informs a historical keyboard/firmware optimization about a guest handler that it later calls. A mantle record containing only `(CS, AX)` would create shadow IRQ/firmware state; an adapter implementation would move keyboard/BIOS semantics; and enabling PIC, keyboard or firmware solely for it violates deny-by-default machine admission. No callback, host pointer, BIOS table, synthetic IVT, PIC, keyboard or source change is admitted.

## Follow-up

Keep XMS `52:09` deferred with the whole package. C4 protected-mode state is the next capability analysis; no native trace is warranted by this result.
