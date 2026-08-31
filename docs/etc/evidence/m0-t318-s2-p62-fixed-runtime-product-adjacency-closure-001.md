# M0 T318 S2 P62 — fixed-runtime product adjacency closure

## Problem

The application deliberately resolves `dos/`, `win16/`, and `softpc/roms/`
relative to `GetModuleFileNameA(NULL)`.  The fixed stage already contained the
formal product together with those media, but `ObserveSoftpcStartup.mjs`
mistakenly passed the separately located formal-build executable to the console
observer.  The child therefore did not meet the product's own installed-package
contract, even though its working directory was the stage.

## Recovery

`ObserveSoftpcStartup.mjs` now:

- accepts the historical format-1 stage manifest and the new format-2 form;
- verifies every fixed media file against the manifest before a launch;
- copies only the supplied formal product into the stage's fixed product name;
- launches that stage-resident executable; and
- records both the exact product hash and a fixed-media-only manifest hash.

`Stage-OriginalSoftpcRuntime.mjs` now emits format 2, which separates the one
mutable product identity from immutable media.  This changes neither SoftPC
source selection nor command-line, console, timeout, firmware, or guest media.

## Verification

- Both Node modules pass `node --check` with the repository Node runtime.
- The existing format-1 `runtime-r33-config-system-media` was accepted without
  migration.  Its fixed media hash was verified before launch.
- The non-debug, console-owning observer launched
  `runtime-r33-config-system-media/original-softpc-process.exe` with the
  unchanged `-f -o --ordinary-child` and 8000 ms contract.  The report records
  product SHA-256
  `13c7f382ec28c65de4ffafd10a9f63504d2638531ba2ca47400d0ca2f15546e0` and
  exits `0xC0000005`.

## Result

The retained access violation is no longer explainable as launching a formal
EXE away from its required adjacent media.  This establishes only a valid,
package-shaped fixed-container failure boundary; it does not identify the
failing original instruction or prove NTIO, NTDOS, EXEC, or parent return.
