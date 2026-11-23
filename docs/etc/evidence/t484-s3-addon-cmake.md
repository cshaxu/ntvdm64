# T484 S3 P1: native addon producers

This batch migrates all14 independently authored addon outputs to
`tools/build-addon.cmd` and native CMake/Ninja targets. Nine existing utility
BUILD.CMD entries delegate to that single producer. The three PowerShell
driver producers are removed. Original Microsoft guest and mirror bytes are
unchanged. S3 is not closed: remaining test compilation callers and the old
worker-generator consumers still require migration.

The five driver targets are compat95, mouse101, mouse31, insignia and ntvdm64.
The nine AMD64 utility targets retain their existing CL flags, CRT and link
libraries. The optional compat95-control variant remains separate. Sixteen
authored mouse probes are opt-in test targets, never release inputs. The two
observation runners now invoke those targets; their existing ABI assertions
remain independent tests, not mandatory production build interpreters.

Driver receipts use the existing native HASH utility and a small CMD writer.
They bind the final binary to build-time source/tool hashes. Configure-time
source-inputs.json is not accepted as a build receipt. Compat95 staging reads
the new receipt and retains the four-case runtime evidence requirement before
granting verified acceptance. Its old acceptance is not silently promoted.

The documented historical WLINK path was absent. Installed Open Watcom WLINK,
its DLL/configuration and license were copied into the owned tool cache and
hash-checked. No source was imported and future builds do not access the
neighboring project. The installed linker reports November11 2026; it is not
claimed identical to the historical November5 tool. Despite that tool change,
both rebuilt VxDs are byte-identical to the published drivers. Tool hashes and
the recovery receipt are retained in the evidence bundle.

## Validation and publication

- All14 outputs built. Five driver binaries are byte-identical to the prior
  release. Nine utilities have exactly8 changed bytes each, proved to be the
  PE COFF and debug-directory timestamps, with every other byte identical.
- Both mouse NE ABI checks pass;16 authored probes assemble. This is not a
  fresh guest runtime acceptance claim. Compatibility-control assembly passes.
- Final cached full host build PID17368 passed. All11 raw host inputs still
  match the published manifest; both destinations retain the verified
  d8983d2b-3fe1-4d18-8ea9-64613c84346c package, all22 hashes checked.
- Publication/test PID15396 updates the9 utilities in assets/release, verifies
  all14 addon hashes, and passes published PIF/HASH, PIF root-update, SZDD
  boundary/round-trip and six LNK synthetic cases. Original Win95 LNK fixtures
  were not supplied to this run; originalLinks=0 is not broader acceptance.
  Driver hashes and acceptance records are preserved. Addons remain under
  assets/release as required; they are not installed into host system32.
- Edge checks PID27040 pass outside-repository invocation with a spaced output,
  unknown-target failure, missing-image receipt rejection, missing runtime
  evidence publication rejection, no-op image hash/mtime stability, all five
  driver source/tool receipts, and no forbidden interpreter in addon commands.

Earlier failures were resolved and are not counted as passes: default CMake
Debug compiler probing hit C1902, fixed by selecting the actual Release probe;
one test invocation used an incorrect Python path; CMD receipt COPY initially
received forward-slash paths, fixed by normalizing only those path arguments.
No product runtime workaround was added.

The [evidence bundle](../../../artifacts/build-migration/t484/s3-addon-cmake/verified-comparison.json)
contains byte-level comparison, build/publication/edge receipts and ABI reports.
The containing commit delivers this batch. S3 test-producer coverage, S4
publication migration and S5 clean interpreter-free acceptance remain open.
