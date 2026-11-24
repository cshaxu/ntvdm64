# T484 S3 P155 native import wrapper retirement

## Decision and scope

B0341 Stage-NativeWorkerImage.ps1 no longer has a product/build consumer.
Native-family CMake directly links worker, monitor, launcher and service.
The remaining four callers only test the former copy/import operation.
Remove the wrapper and native_service_build_inputs.ps1 (entirely its self-test).
In the monitor/worker package tests remove only the former import assertions;
retain their Stage-System32ProductPackage checks. In launcher ABI verification
remove import-only cases and their now-unused PreviousLauncher/Hook64 arguments;
retain native/x86 capture, scope, RTL, search and image-classification cases.
No current caller of that runner passes the removed arguments.

## Verification and limitations

Source/caller audit establishes no current staging graph consumes the wrapper.
Post-edit tools/test/root CMake searches find no reference to either deleted
entrypoint. Background PID19616 passes PowerShell syntax checks for all three
edited runners and compares their retained actual package/ABI caller lines
against HEAD. Manual diff review confirms only retired import blocks changed.
This is static evidence, not a fresh execution of package or ABI cases.

[Receipt](../../../artifacts/build-migration/t484/s3-native-import-retirement/result.json).
No product source, build graph, guest or released image changed; no rebuild,
runtime Full or republication is claimed. Current packaging migration remains
in S4; it is not completed by deleting this former import helper.
One inventory disposition completed:285 of355,70 remain. S3 remains active.
