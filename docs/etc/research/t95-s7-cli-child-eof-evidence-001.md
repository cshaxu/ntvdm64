# T95 S7 CLI Child EOF Evidence 001

## Question

Can the non-invasive CLI prevent the original Win32 Bochs post-exit ENTER
prompt from retaining its engine child, without changing Bochs, the adapter,
or guest state?

## Inputs

- `src/cli/ntdos64_run.c`, the sole CLI child-process owner.
- `tests/runner/runner_engine_probe.c`, a test-only child.
- Existing `ntdos64-run-policy` integration fixture.
- The r48 controlled-stop trace, which proves `cpu_loop()` returns before the
  original Win32 wrapper waits on `stdin`.

## Procedure

`run_process` opens inheritable read-only `NUL`, assigns it as the child's
standard input with `STARTF_USESTDHANDLES`, retains inherited standard
output/error, and closes the parent's handle after `CreateProcessW` returns.
The probe calls `ReadFile(STD_INPUT_HANDLE)` and requires a successful
zero-byte result before returning its pre-existing success code.

The prior MSYS UCRT GCC fixture could not be used: its `cc1.exe` exits
`0xC0000135` before diagnostics. The independently installed modern CLI
toolchain used for this evidence is WinLibs MinGW-w64 GCC 16.1.0,
`x86_64-msvcrt-posix-seh`, msvcrt CRT:

```text
cmake -S . -B artifacts/build/t95-s7-cli-child-eof-winlibs-gcc-001 -G Ninja
  -DCMAKE_C_COMPILER=<WinLibs gcc.exe> -DCMAKE_BUILD_TYPE=Release
cmake --build artifacts/build/t95-s7-cli-child-eof-winlibs-gcc-001
  --target ntdos64-run runner-engine-probe
ctest --test-dir artifacts/build/t95-s7-cli-child-eof-winlibs-gcc-001
  -R ^ntdos64-run-policy$ --output-on-failure
```

## Observations

Configuration identified GNU 16.1.0. The target build compiled and linked
both CLI executables. The sole selected CTest passed in 0.42 seconds. Its
engine probe validates the existing path-free environment handoff and now also
proves that standard input is immediate EOF.

## Interpretation

High confidence: the CLI can make the original Bochs convenience `fgets` see
EOF after an ordinary CPU-loop return, without a Bochs source intrusion. This
is process composition only: it does not create a Bochs-to-CLI command-line
contract or prove a real Bochs invocation through `ntdos64-run`. That remains
the queued CLI engine-materialization work, and must preserve the existing
public/child argument separation.

## Follow-up

Retain the MSYS UCRT compiler failure as external toolchain evidence. Before a
product claim, admit a real engine-launch contract that gives Bochs its own
arguments while the adapter receives only child environment metadata, then
run an end-to-end CLI trace. Do not add a BOP, guest service, adapter policy,
or Bochs patch for this purpose.
