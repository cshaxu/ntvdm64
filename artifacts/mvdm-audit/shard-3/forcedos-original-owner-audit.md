# Original ForceDOS executable and selected worker semantics

## Evidence and scope

`src/mvdm/vdmutils/forcedos/forcedos.c` is the byte-identical selected
OpenNT reference source for a standalone user utility, not a host library
member. Its original `sources` declares `UMAPPL=forcedos` and includes
`forcedos.c` and `forcedos.rc`. The shard also includes its header, resource
script and a historical `.lib` artifact. No `forcedos.exe` was found at the
inspected installed `O:/winnt` and Windows System32/SysWOW64 paths or as a
member of `assets/winnt.zip`; this observation does not exclude other
external copies or launch paths. No process was launched or file modified.

The file's introductory comment describes PIF-based forcing, but its actual
NT4 implementation calls `CreateProcess` with `CREATE_FORCEDOS`; original
`dos/command/cmdmisc.c` explicitly says NT4 replaced the earlier PIF method.
The original utility also supports `/D`, OEM conversion, PATH/extension
search, inherited handles and environment, a bounded command line, error
resources, and infinite child completion followed by the child's exit code.
Those contracts should not be reduced to “launch any DOS executable”.

## Original runtime path and current evidence

1. Original request `CREATE_FORCEDOS` becomes the original VDM command
   record's `dwCreationFlags` via `BaseCheckVDM` and service message.
2. Original `cmdGetNextCmd` sets `DontCheckDosBinaryType` from that flag.
   `cmdCheckForPIF` can first set it when `SubSysId == SUBSYS_DOS`,
   but both pinned OpenNT and current `cmdGetNextCmd` unconditionally
   overwrite that state afterwards from `CREATE_FORCEDOS`.
3. Original `cmdCheckBinary` returns DOS classification without querying the
   child image when that state is true. This implements forced treatment of
   later child spawns, rather than only classifying the root request.
4. Current `run16.exe` dispatches using original `OpenNtBaseGetBinaryTypeW`
   and forms its `BaseCheckVDM` flags from CREATE_NEW_CONSOLE /
   CREATE_SEPARATE_WOW_VDM. A repository source search found no project
   runtime producer of CREATE_FORCEDOS outside the retained original utility
   and its original consumer. The service schema does carry
   `dwCreationFlags`; this is not evidence that the requested bit reaches it.
5. `tests/mvdm/oemuni/oemuni_pif_caller_fixture.c` demonstrates that a
   selected PIF setup can set the flag in its isolated source-shaped setup.
   `tests/component-integration/fixtures/t231_s6_command_check_binary_direct_fixture.c`
   contains a historical direct bypass check, but it is not evidence of
   current end-to-end ForceDOS execution. That fixture invokes `cmdCheckForPIF` alone and does not test the later
   `cmdGetNextCmd` overwrite; current complete PIF behavior remains
   dynamically unverified.

The queued external launch-loader proposal intentionally passes native PE
requests through, so it does not itself implement this explicit forced-DOS
contract. Normal DOS launches or a PIF launch alone do not prove the complete
ForceDOS command-line, inherited-stream and recursive-child behavior.

## Candidate recovery boundary, not an implementation approval

A minimal direction worth auditing is to retain the original standalone
utility's parsing/error/return owner and connect its `CREATE_FORCEDOS`
request to the existing launch/service VDM request path, including the
original creation flag rather than introducing a second classifier. Trace
whether a finite compatibility call on modern x64 can make that request
without depending on the host's absent NT4 kernel VDM; a replacement launcher
or additional user-facing option is not assumed. The alternative PIF route
must be measured against the same root and descendant semantics before
calling it equivalent. Preserve native input/output, PATH, current directory,
exit result and the existing NTSRV ownership model.

A source-shaped host fixture should first verify creation-flag transport and
original command bypass in positive/negative cases. Independently authored
DOS/NE test programs should then verify an explicit forced root and nested
child plus a normal non-forced contrast, using unchanged guest media. Only
then can the standalone utility, header and resource entries receive a final
classification. No claim is made that this is a confirmed product bug until
that original capability has a supported actual caller and dynamic witness.
