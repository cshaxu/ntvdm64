# T483 S10 component layout

Baseline 55643b6c5. The owner admitted frontend-base promotion and shared Hook component renaming, with cached builds and targeted verification.

- Source and unit tests moved to src/frontend-base and test/frontend-base. The x86 and x64 generated graphs use frontend-base.lib and obj/frontend-base. NTCON and NTVDM consume the same channel, service and video sources. The generic library and worker backend ownership gates cover the new directory.
- src/nthook-dll owns both original module definition files. Both staged DLLs match their preceding release in machine type and exported names and ordinals (three per DLL).
- Cached x86 and x64 release builds returned 0 on the final production sources. Both manifests and all eleven images matched at assets/release and O:/winnt/system32. Package c3e16355-bebc-4896-a22b-b77a3553ae2c; local build and verification records under build/t483-s10-p1.
- Explicitly built and ran the three frontend-base test targets (channel, service and video) on x64 and ntvdm-vm-frontend-test.exe on x86 (233 assertions). The default build does not produce all test executables.
- The isolated nested-window broker I/O integration ran actual CMD/DOS input, returned to its parent and delivered exit code 23. Frontend link ownership, BaseSrv ownership and both native Hook context/installer ownership checks passed including negative controls. The BaseSrv verifier now accepts the actual x64 Hook graph path while retaining its existing default for combined build trees.
- Targeted verification per owner request. Historical delivery records retain their original paths.

## Closure

Owner accepted S10 after P1 was pushed to main at 14969ed80. The closure check confirmed both published manifests and all eleven image hashes at both destinations, both successful build receipts, and the passing targeted test witnesses. T483 remains open between subtasks.
