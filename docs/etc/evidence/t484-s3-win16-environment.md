# T484 S3 P163 Win16 native environment dependency

The multimedia timer and shell/hook runtime entries still loaded an external C#
LogonHelper. Reuse the P161 shared reader and existing CMD/native logon-environment
producer; remove the unused external helper parameters and replace their input
hashes with the actual native executable/shared helper hashes. No current checked-in
caller forwards the removed parameters. Guest probe source and assertions remain
unchanged; original MMSYSTEM and other guest files are copied without modification.

The ordinary test probe variants already build via win16-probes or win16-shell-probes;
optional OLE peer uses win16-ole-peer. Optional WAV construction is runtime test data,
not a hidden compiler or generated program. Original guest remains immutable.

Background PID17140 runs actual private-desktop multimedia periodic callback/cancel,
NoTimer control and hook scenarios, then cached full and11/22 package hashes.
Evidence: build/t484-s3-win16-environment/result.json and per-case exit.txt,
guest-report.json/result.txt, inputs and process records. Scope of this runtime
verification is these scenarios, not every optional shell/media/optical variant,
performance acceptance or a new Full regression. Other variant producers retain
previous byte-identity evidence; this batch changes only environment acquisition.
