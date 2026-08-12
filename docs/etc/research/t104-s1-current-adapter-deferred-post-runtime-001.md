# T104 S1 current-adapter deferred POST runtime 001

## Inputs and procedure

T104 generated the fresh root
`artifacts/build/bochs-2.6-t104-current-adapter-deferred-msvc-r1` from the
retained r5 engine.  Its manifest records two Bochs object replacements only:
`main.o` from current `main.cc`, and `cpu/exception.o` from current
`exception.cc`; the current exception-intercept header is copied as a compile
dependency only.  All CPU/device/memory/GUI archives remain retained inputs.

The declared MSVC/x86 command was run once:

```text
nmake /f ntdos64-t98-current-adapter.mak ntdos64-t98-current-adapter.exe
```

It succeeded.  The binary is 2,580,480 bytes with SHA-256
`9433196B03A10E31C85AB90C2BB33D221A51B957098E48BE32B85B839B49D3C9`.
The generated shim has `main.o` with direct/deferred macros `0/1`, and
`cpu/exception.o` with deferred enabled.  The full link line places the
rebuilt `cpu/exception.o` before retained `cpu/libcpu.a`; no archive was
rebuilt or repacked.

One standard deferred observation then ran with the native option ROM and
`-benchmark 1`.  Its exact profile and ROM hashes are respectively
`3606D8D0570F4D7C42B0DA181EE81A31F18D92E2377396CFE2AFF65BA771D982` and
`E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB`.

## Observation

`artifacts/analysis/t104-s1-current-adapter-deferred-post-001/observation.json`
records `watchdogTerminated: false`, `exitCode: 1`, 31 BOP observations, and
four existing bounded commits.  The log has no `requested execution plan
applied` marker and no `prefetch` panic.  Its first real BOP is the expected
NTIO `50:11` at `0070:0475`, immediately after the existing ordinary-RAM
NTDOS transaction.  It subsequently reaches the same real DOS/COMMAND path
as the retained native-POST evidence, including `50:42`, close, notification,
and `50:36`.

Thus the test proves lifecycle reachability and preserves the non-goals: it
does not prove successful CLI completion, a file-service implementation, or
that every observed BOP has a provider.  Exit code one is the existing bounded
Bochs benchmark outcome, not a guest crash or a service result.

## Decision

The direct-entry / missing-POST problem is closed for the current adapter
composition.  The highest ROI successor is a single longer bounded observation
using this exact deferred binary, before changing any BOP provider.  Its sole
purpose is to identify the first post-`50:36` execution boundary that prevents
the selected CLI workload from completing.  It must not turn a newly observed
selector into an ad-hoc implementation.

