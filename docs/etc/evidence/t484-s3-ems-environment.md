# T484 S3 P162 native EMS environment dependency

B0038 original EMS runtime verification now uses the shared native environment
reader from P161. CMD/native already owns the authored COM and PIF producers;
CMD now also builds logon-environment.exe. Remove the external C# LogonHelper
parameter and record the native image/shared-reader hashes. Original INT67
implementation, probe instructions, EMS profile and all assertions remain unchanged.

Update the two actual forwarding callers, verify-wow-page-cost.ps1 and
verify-wow-paired-latency.ps1: remove their unused helper parameter, prerequisite,
input hash and forwarded argument. Their other workload and performance behavior
is untouched. They remain open pending their respective complete-chain evidence;
this change does not claim their runtime scenarios passed.

Background PID17172 owns all three syntax checks, actual Mapping and CallReturn
private-desktop guest runs, cached full and11/22 image hash checks. Raw evidence:
build/t484-s3-ems-environment/result.json, Mapping/result.txt, CallReturn/result.txt
and full-build.log. The existing real short-stage helper owns runtime cleanup.
No original guest, formal installed package or assets/winnt.zip was modified.
