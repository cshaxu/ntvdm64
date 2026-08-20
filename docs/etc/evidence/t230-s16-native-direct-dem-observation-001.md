# T230 S16 native Direct DEM observation

## Question

Does the v2-only native engine bind the Direct OpenNT DEM session through the
mantle checked-memory transport and execute one real DEM BOP without a v1
provider, dispatcher, result bridge, or startup composition runtime?

## Inputs

- Formal MSVC x64 `/MT` Ninja graph:
  `build/M0-T230-S15/v2-startup-r2`, generated from
  `tools/build/t225-s7-full-module-manifest.json`.
- The hash-checked primary original-toolchain input set staged only under the
  disposable `build/M0-T230-S15/v2-startup-inputs-r1` root.
- The v2 startup correction in
  `src/bx-vdm/bop/dem_v2_startup_composition.c`: initialize the embedded
  selector-blind `startup_action` ABI before populating the copied NTIO bytes.

## Procedure

1. Incrementally rebuild `bin/ntdos64-native.exe` in the formal Ninja graph.
2. Run the existing hash-locked BYOB profile and target with a 128-tick budget
   to verify machine-stage entry.
3. Run the same input with a 10,000-tick budget plus
   `--observe-bop-sequence --observe-ud-sequence`.

## Observations

- Before the correction, the native request failed at stage preparation with
  `terminal=2 detail=3`.  The v2 code had populated `startup_action` fields
  without calling `bx_ntvdm_mechanical_action_v1_clear`, so its required fixed
  ABI magic/version/size fields were zero.
- After the correction, the 128-tick run initialized CPU/RAM and returned the
  ordinary watchdog outcome `terminal=4 detail=0`; it no longer rejected
  composition.
- The 10,000-tick observation recorded first:

  ```text
  bop[0] cs=0070 eip=00000475 selector=50 service=11 has-service=1 disposition=1
  ud[0]  cs=0070 eip=00000475 mode=1 disposition=1 window=c4c45011...
  ```

  This is the real NTIO `50:11` call to imported OpenNT `demLoadDos`; v2
  accepted it and supplied the checked resume outcome.
- The next observed `12:00` event declined (`disposition=0`).  No v1 fallback
  occurred.  The following repeated non-DEM #UD is a separate machine/BIOS
  continuation condition, not a DEM implementation result.

## Interpretation

The native engine reaches the v2 Direct session and executes one real OpenNT
DEM handler through the mantle checked-memory transport.  This completes the
bounded native observation required by T230; it does not claim that the guest
continues beyond the next non-DEM/machine owner boundary, nor does it authorize
a trace-driven leaf repair.

## Follow-up

T230 has no remaining DEM implementation transfer.  The observed `12:00` and
the later machine condition belong to their independently admitted owner
packages, not to a new DEM task.
