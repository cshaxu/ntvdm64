# M0 T310 S8 P4 — owner-contract execution order

## Purpose

The x86/x64 ABI audit covers only `src/mvdm-host/**` and
`src/mvdm-support/**`.  Its remaining diagnostic rows are evidence inputs,
not independent implementation tasks.  This ordering reduces them by the
original SoftPC owner and callable contract so that one source review covers
all declarations, definitions, table assignments and selected callers of the
same mechanism.

The authoritative raw inputs are the whole-tree static ABI inventory, the
compiler warning ledger, the contract review index, the reconciled warning
worklist **and every existing P4 closure/disposition evidence file**.  The
latter currently contains 830 records grouped into the sixteen clusters
below.  Counts are planning indicators only; closure requires source evidence,
not a count reaching zero.

An already-repaired mirror line is not exempt from its owner-contract review.
At the beginning of every cluster review, its existing `DIVERGENCE` entries,
overlays, adapter bindings and earlier evidence are placed beside the original
declaration, definition, slot/initializer and selected callers.  The review
must either retain that repair as the smallest consistent form, replace it
with an available original package/interface, or remove it.  This prevents a
locally clean warning from being mistaken for a complete callable contract.

## Ordered clusters

1. **CCPU execution contract** — `SPC-CCPU-EXECUTOR-DISPATCH` (268 records).
   This is the selected CPU/FPU instruction, host-IP and exception contract.
   Resolve native pointer deltas, callable declarations and genuine host-word
   crossings first; retain fixed Intel scalar conversions as visible
   non-width evidence.
2. **Memory and core system controller contracts** —
   `SPC-SYSTEM-CONTROLLER-CALLBACKS` (27),
   `SPC-MEMORY-MAPPING-BINDINGS` (0 pending) and
   `SPC-HOST-SYSTEM-INTERRUPT-BINDINGS` (0 pending).  These own reset,
   timer/PIC/A20 and physical guest-memory boundaries.  Any actual guest or
   host identity crossing uses the existing session mapping manager.
3. **Firmware/startup contracts** — `SPC-BIOS-FIRMWARE-BINDINGS` (23).
   Keep immutable firmware/media inputs and original unavailable direction;
   do not replace controller algorithms with startup shims.
4. **Storage contracts** — `SPC-STORAGE-CONTROLLER-CALLBACKS` (26) and
   `SPC-HOST-STORAGE-BINDINGS` (7).  Review disk/floppy declarations,
   controller tables and host media binding as one device family.
5. **Input contracts** — `SPC-INPUT-CONTROLLER-CALLBACKS` (58) and
   `SPC-HOST-INPUT-BINDINGS` (11).  Review keyboard/mouse callbacks with
   their host input shape; no guest pointers may become host pointers.
6. **Video contracts** — `SPC-VIDEO-CONTROLLER-DISPATCH` (154) and
   `SPC-HOST-VIDEO-BINDINGS` (134).  Keep this after startup devices: no
   video mapping is enabled until generated and base/video callable contracts
   are jointly demonstrated on x86 and x64.
7. **Communications contracts** — `SPC-COMMS-CONTROLLER-CALLBACKS` (17).
   Recover serial/parallel contracts only after their selected host endpoint
   direction is established.
8. **Host-platform residuals** — `SPC-HOST-PLATFORM-BINDINGS` (18).
   Separate public Win32 bindings, session controlled-stop mappings and
   explicit kernel/CSRSS/fullscreen hard boundaries; do not invent direct
   NT4 private imports.
9. **MVDM service/owner residuals** — `MVDM-DOS-OWNER-PACKAGE` (68),
   `MVDM-SIM32-OWNER-PACKAGE` (2) and `MVDM-XMS.486-OWNER-PACKAGE` (1).
   These are reviewed only where the selected SoftPC machine invokes them;
   BOP/provider completion remains with its owner package.
10. **Other machine source and configuration** —
    `SPC-OTHER-MACHINE-SOURCE` (14) and
    `SPC-HOST-CONFIGURATION-BINDINGS` (2).  They are last because they do not
    establish the CPU-to-device startup path.

## Per-cluster exit rule

For every source contract in a cluster, including previously repaired source,
record exactly one of: unchanged
fixed-width/source form; minimal mirror correction; mirror overlay; existing
adapter-backed binding; mapping-manager boundary; original unavailable in the
selected profile; or not selected/not host runtime.  Then run the smallest
formal x86 and x64 object or link target that covers the changed contract.
Only cluster completion updates the shared worklist; unrelated warnings stay
visible.
