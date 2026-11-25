# H024 Full checkpoint

The product35 Full run stopped at WOW startup. The pre-migration runtime displayed Solitaire on the same private desktop. The product35 launcher returned `0x45A` after Win16 called `WK32ExitKernel` before `InitTask`. Retaining the XP COMMAND environment and replacing the Win16 core in diagnostic stages did not restore startup.

Two DPMI paths required the existing software CPU semantics on x86.

* After a real-mode DOS service, DS still read `0217`, but its cached base was `2170` instead of the LDT base `14D20`. At `KRNL386:9F8F`, LDS therefore read `1B8A:89FF` instead of `00C7:0DEB`. The INT 21h translation return reloads unchanged DS and ES selectors after the service.
* Win16 traverses the DOS system file table by changing the base of its active ES descriptor. The XP `_X86_` no-op left the old base cached in the software CPU. `GrowSFTToMax` counted 113 handles and jumped to `ExitKernel`. The CCPU build now uses the existing active-segment reloads. The compiled-code invalidation loop is for translating emulators.

The combined x86 candidate displayed Minesweeper, Solitaire and Write main windows. Evidence under `build/t485-s4-h024-full` includes `fault-cpu.log`, `sol-path.txt`, `old-thunk-sol`, `thunk-sol` and the three window reports under `refresh-frontiers`.

`tools/build.cmd` completed for both architectures. Product36 contains 13 images, package `E64BABDE-4183-4CFA-85A9-89B82CDD5494`. Both `assets/release` and `O:\WINNT\system32` matched the staged SHA256 values and manifest. The source for this checkpoint excludes the in-progress H025 directory changes. The release baseline before this checkpoint was `21c844801`.

The released package passed `wow-frontiers` with the original three applications. `Console-17` and `Window-17` passed with the qualified XP COMMAND banner (`Microsoft(R) Windows DOS`). The NT4 profile still expects `Microsoft(R) Windows NT DOS`. The native terminal observer recognizes either banner as evidence of entry into DOS. The RPC rejection fixture now receives NTVDMD.DLL in its private package, matching its compiled list of required siblings. Its original rejection assertions passed. Prior attempts and their first failures are retained under `full36`, `full36-banner` and `full36-rpc`.

The final Full run passed all 20 gates in 380.539 seconds on product36. The nested window handoff observer recognizes the XP and NT4 COMMAND banners and still requires DOS output, the parent-return marker and exit code 23. DOS4GW displayed the NESticle menu, and Escape hid and restored it. The final runner reported an identical 13-file package. Per-gate outcomes are in `build/t485-s4-h024-full/full36-final/timings.json`, with the graphics witness in its `dos4gw-nesticle` directory.
