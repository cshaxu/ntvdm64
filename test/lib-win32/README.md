# Shared Win32 component tests

These tests follow their provider's `src/lib-win32/<component>` directories.
They exercise public component contracts; they do not depend on a product
launcher, service, installed package, or another repository at runtime.
The same subtree works under either `test/lib-win32` or `tests/lib-win32`.

The owning project supplies its compiler, architecture, runtime and cache
configuration. Add this directory with CMake `add_subdirectory`, or configure
it as a standalone project. Build `shared-win32-tests`, then run CTest with
`-R "^(shared-|unwind|late_batch|propagate)" --output-on-failure`.

| Provider | Coverage |
| --- | --- |
| handle | Duplication, inheritance, invalid handles and ownership |
| process-image | Machine selection, restricted process access, remote PE bounds/protection and file-handle DLL validation |
| process-launch | Attribute construction/failure cleanup, raw argument tails, suspended creation, exit and launch failure |
| process-hook | Staging, packet validation, unwind, batching, propagation, late-guard races and cross-width payload installation |

After building both architectures, run `verify-matrix.ps1 -X86 <test-output>
-X64 <test-output> -LogRoot <task-evidence>`. It checks all four caller/target
width pairs, invalid payload rejection, process access and low-address
exhaustion using only its own fixture processes. Its deadlines bound tests,
not production behavior. In this repository CMake outputs reside below
`build/.ninja/<configuration>/shared-tests`.

`process-hook/verify-upstream.py` checks the pinned source manifest. Source
identity is independent of these tests: generalized tests are intended for
both consumers and are not asserted to match the upstream test tree until
that project adopts them.
