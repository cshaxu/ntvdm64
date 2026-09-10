# T194 S15 v6 CLI runner/shim source-build closure 001

Date: 2026-08-12  
Packet: M0 T194 S15  
Disposition: CLI-only source/build/test closure complete; no real Bochs engine
or guest process started.

## Inputs and build boundary

The current checkout was configured in
`artifacts/build/current/t194-s15-cli-v6-mingw-r1` with MinGW GCC 16.1.0 and
the matching CMake 4.3 installation. The build named only
`ntdos64-run`, `ntdos64-bochs-engine`, `runner-engine-probe`,
`bochs-engine-probe`, `byob-profile-test`, and the separately requested
`byob-launch-plan-v2-test`. It did not name, compile, link, or start Bochs,
the adapter, the r2 engine, or guest code.

## Produced current pair

| Member | SHA-256 |
| --- | --- |
| `ntdos64-run.exe` | `706882B0EE453CB5E4A64A81A9E357375766DB4B99BDBAD74932577C1570CB37` |
| `ntdos64-bochs-engine.exe` | `57AD850F46782FFF798DBEA39EFE6C7EB69FAD3B192DDF2DA5850E696F3F6A41` |
| `byob-launch-plan-v2-test.exe` | `95554E7EE662CABAB6769021BD16DCC94CD42A1F6E0D547C2572F793640BEC6B` |

The new runner contains the `ntdos64-byob-profile-v6` identity that was absent
from S14's retained binaries. The shim retains the fixed T183 eight-argument
ABI and bundle-only native-child invocation.

## Verification

1. `byob-profile-test` passed, including its v6 parser and one declared
   non-terminal target checks.
2. `ntdos64-bochs-engine-policy` passed. It exercises the fixed shim ABI,
   manifest checks, private configuration materialization, inherited profile
   and root environment, and a test-only fake native parser. It does not
   execute the r2 engine.
3. `byob-launch-plan-v2-test` passed and printed that its immutable one- and
   two-slot handoffs were verified. Its one-slot expectation is
   `2,1,c,00`; the S13 EXE profile correspondingly serializes the same
   fixed-width form with `e`.
4. A new temporary fake bundle containing only the existing
   `bochs-engine-probe.exe` was assembled under the S15 build root. The current
   runner accepted the exact S13 v6 profile/root/TARGET.EXE, passed it to the
   current shim, and the fake parser returned the expected test exit `47`.
   The fake parser validates the shim's derived fixed Bochs command line and
   inherited bridge/profile/root environment. It is not the r2 engine, has no
   CPU loop, and receives no guest execution.

The retained `ntdos64-run-policy` test was also run and failed. Its fixture is
v3 and its probe still requires withdrawn `NTDOS64_ADAPTER_LAUNCH_KIND` and
`NTDOS64_ADAPTER_LAUNCH_TAIL` variables. The current runner correctly uses
the v2 immutable plan variable instead. This is a stale test-fixture contract,
not a passing result or a reason to substitute a v5 runner. The v6-focused
verification above is the acceptance evidence for this S; fixing the broad
legacy policy fixture is deferred outside this packet.

## Follow-up

T194 S16 may perform exactly one bounded observation using the current pair,
the immutable S10 r2 bundle and the S13 root. It must use the existing v6
controller, retain all output, use one watchdog and no retry, and must not
claim normal return unless its `54:11` marker is observed.
