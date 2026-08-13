# M0 T194 S40/S41: Focused Test Compile Finding And Correction

S40 configured the isolated build root
`artifacts/build/current/t194-s40-dem-ioctl-metadata-test-r1` with WinLibs
MinGW GCC 16.1.0. Only target
`bx-ntvdm-dem-ioctl-metadata-provider-v1-test` was requested.

Compilation stopped before link or execution with:

```text
invalid use of void expression
```

The sole cause was using `bx_ntvdm_instruction_window_v1_capture`, declared
`void` in its ABI header, in a Boolean expression in the new test. S41 moves
that call before the existing Boolean validation chain. No assertion, provider,
CMake, ABI, runtime, engine, Bochs, CLI, or guest behavior changes.

A focused scan finds no remaining Boolean use of that void capture call in the
test. S42 is required for the isolated compile/run retry; it may reuse the
same build root but may not alter checked-in source or build any other target.
