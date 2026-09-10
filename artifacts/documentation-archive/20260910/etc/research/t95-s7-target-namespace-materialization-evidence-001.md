# T95 S7 Target Namespace-Materialization Evidence

## Implemented Scope

After a v3 profile has passed launch-declaration validation, adapter
installation re-verifies the selected target using `byob_image_load_exact`.
The resulting immutable bytes remain adapter-private. The adapter then
appends exactly one canonical entry, `\\TARGET.COM` or `\\TARGET.EXE`, to its
closed read-only namespace on the same selected drive as `\\COMMAND.COM`.

The existing `\\COMMAND.COM`, `\\CONFIG.SYS`, and `\\AUTOEXEC.BAT` entries are
unchanged. The target append rejects a null or empty image, an absent/wrong
placement, a wrong target filename, a different drive, a non-canonical path,
a duplicate entry, and any attempt while a namespace file is open. It grants
no host path, directory enumeration, host handle, or guest pointer.

The target is deliberately loaded only after the v3 launch kind has been
checked against the profile. Thus a missing or mismatched child declaration
cannot cause target-image materialization.

## Verification

Fresh root `artifacts/build/t95-s7-target-namespace-c11-001` was configured
with CMake 3.30 and GCC 16.1.0. The UCRT runtime directory
`C:\\msys64\\ucrt64\\bin` was prepended to `PATH`; without it, compiler discovery
could launch `gcc.exe` but could not compile CMake's probe. The build ran
only these two focused targets:

```
bx-ntvdm-readonly-namespace-test
bx-ntvdm-adapter-runtime-test
```

Both passed:

```
bx-ntvdm-readonly-namespace-test: canonical read-only file lifecycle verified
bx-ntvdm-adapter-runtime-test: identity-to-pending-transaction path verified
```

The namespace fixture proves that append fails while `\\COMMAND.COM` is open,
then permits one target append after close, and that open/read returns exactly
the two target bytes. It also proves a second append fails. The runtime
fixture retains the v3 missing-launch rejection and matching `com` acceptance,
thereby exercising the installation ordering before target loading.

## Boundary and Next Gate

This is materialization only. It does not implement `BOP_CMD:01`, alter a
guest register or memory byte, expose the namespace to a guest, change Bochs
or OpenNT, or run a guest. The next admissible step is a source-led response
contract for the reached `BOP_CMD:01` (`SVC_CMDGETNEXTCMD`): identify its
request/response memory and CPU effects from OpenNT, then decide the smallest
bounded transaction that can return the already copied command declaration.
