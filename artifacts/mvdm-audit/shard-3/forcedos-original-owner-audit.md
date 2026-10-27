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

Original `base/win32/client/process.c` handles an OS/2 NE status
(`STATUS_INVALID_IMAGE_NE_FORMAT`) differently on x86 if forced and sends
`STATUS_INVALID_IMAGE_WIN_16` to DOS rather than WOW when the flag is set.
A successful native PE image section does not establish that it is forced;
no such claim is made. The queued external launch-loader proposal does not
specify this explicit force bit or the forced Win16/OS2 NE distinction. Normal DOS launches or a PIF launch alone do not prove the complete
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


## Complete current entry/flag path and testable plan

The original flag value is `0x00002000` (`public/sdk/inc/winbase.h`).
Current `nthook32-dll/create_process.cpp::legacy_application` admits only
DOS, Win16 and PIF selected by the normal classifier and does not inspect
that flag. Its `finish` retains only `CREATE_SEPARATE_WOW_VDM` in the
one-hop `native_hook_packet` when handing a redirected launcher to Run16;
`installer.cpp` and `context.cpp` explicitly reject other flag bits. The
corresponding `run16_frontend_scope_creation_flags` is read only for separate
WOW; `run16` chooses DOS/WOW using the unmodified classification and
`launch_vdm` only sets new-console/separate-WOW flags. In particular, an
OS/2 NE classified as `SCS_OS216_BINARY` is not among the hook's admitted
legacy classes. This is a source-chain finding, not a dynamic test of native
OS/2 launch or success of a forced DOS stub.

The RPC values do already include `creation_flags`: the NTSRV check path
serializes `message.u.CheckVDM.dwCreationFlags`; original `srvvdm.c` stores
it in the VDM record and copies it into the `GetNextVDMCommand` response;
original client `GetNextVDMCommand` restores it to the worker `VDMInfo`.
The GET request validation requires its incoming creation-flags field to be
zero; that validation is for the request, not the service response. Existing
broker fixture tests ordinary scalar roundtrip values but no `CREATE_FORCEDOS`
case was found. Do not assert this transport is end-to-end tested just
because the field exists.

A promising minimal **within-runtime** route is to extend the existing
authenticated, one-hop launch context's allowlist with the original flag,
allow normal explicit-force selection for original Win16/OS2 NE statuses,
have Run16 apply the flag in the existing original DOS classification and
`BaseCheckVDM` request, then let the existing RPC and original COMMAND state
handle descendant policy. This is an implementation hypothesis: it needs
source review of OS/2 classification on both host bitnesses, incompatible
creation flags and first-process/console semantics. It must not reinterpret
an otherwise valid native PE as forced DOS just because a suffix matches.
The standalone historical utility is not automatically intercepted when
launched externally; a separate original-UX-compatible entry using a reviewed
launch boundary is necessary. Whether to expose an additional Run16 CLI
option or use only the existing private launch-context path is a product
interface decision, not silently settled by this audit. This is not permission
to introduce an extra intercomponent control transport.

For noninteractive validation, use authored disposable fixtures, never a
replacement guest image:

1. Extend the existing packet-validation/`nthook_install_test.cpp` style
   fixture with both widths and allowed/rejected bit combinations; assert
   one-hop flag retention and native PE normal-routing contrast.
2. Extend original-server lifecycle roundtrip with the actual 0x2000 bit;
   assert exact `CheckVDM -> GetNextVDMCommand` field and original
   `cmdGetNextCmd -> cmdCheckBinary` behavior, including PIF overwrite.
3. Use immutable known DOS/Win16 input and an independently authored bound
   OS/2/Win16 NE-with-DOS-stub probe that prints distinct DOS/NE markers.
   Compare ordinary and explicit-force routes, then have its DOS stub launch
   a second distinguishable image to prove descendant policy; verify actual
   output, completion, exit status, working directory, inherited redirected
   streams and negative flags. The authored probe is a test input only.
4. Check normal native PE, normal Win16, forced Win16 and OS/2 routes and
   `CREATE_NEW_CONSOLE`/detached semantics separately. No screenshot or
   field-only fixture is enough for a full product claim.

No probe was run for this continuation and no source/product/guest was
changed. T-wide file classification remains pending.

## 2026-10-26 scalar codec probe

`forcedos-codec-probe.c` (SHA-256 `5C238657788AC5352107B1AFE6084393D6E1624FD970922ECCB5F9840DA1AE89`) independently links the production x86 cached `worker/obj/opennt-base-bindings/values.obj` (SHA-256 `7E7F718851FA4F033060930D5424FDE035A9A607442FC2EB98662B5D69A67879`); the corresponding production `base_values.c` SHA-256 is `6E2A8AE58114A030D5D9FC2F2CDFFED86B5E3558AF399FD86D9EE6B4147CC597`. Compilation reused the flags returned by `ninja -C build/.ninja/msvc-x86-release/worker -t commands obj/opennt-base-bindings/values.obj`, replacing only source and output, via `build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd`. Linking used `link /nologo` with the probe object and the production values object; executable `build/t473-s10/shard-3-forcedos/probe.exe` returned 0.

The probe sets `dwCreationFlags=0x2000`, asserts exact encoded flags and decoded flags for both `BROKER_VDM_CHECK` and `BROKER_VDM_GET_NEXT`. This establishes only the scalar codec can carry the bit; it does not exercise RPC, service policy, process classification, existing guest or actual DOS execution and cannot establish end-to-end ForceDOS behavior. It does not access the shared NTSRV or modify guest/product code. The prior pending classifications remain unchanged.

## 2026-10-26 one-hop consumer boundary probe

`forcedos-context-probe.cpp` SHA-256 `030602A205746EE0CC867A87FC537791C23942E8D3AE16E627E15E0B4102D873` links the existing x86 production `worker/obj/nthook32/context.obj` SHA-256 `3F4A57C93CDB64B9D0564DF5C30D41301E2E19D893371E71CCA4E8AE7C01729E`; corresponding `context.cpp` SHA-256 `26F19EE85D40F846B878B053778CEC9CAEDCC86CF1F4D50145464933DC1AABF7`. Built with x86 `msvc-x86.cmd cl /nologo /EHsc /MT /I src /c` and linked via `msvc-x86.cmd link /nologo` with that production object. Probe exits 0: valid launcher packets with no flags and separate-WOW flag are accepted, while packets with 0x2000 alone and combined with separate-WOW return `ERROR_INVALID_DATA`.

Only `DetourFindPayloadEx` is stubbed to supply an otherwise well-formed packet; the actual production consumer validates it. This is a real consumer boundary test, not proof of live Detours delivery, actual CreateProcess routing, installed server, or guest behavior. No NTSRV accessed and no guest/product modified. Together with scalar codec probe it proves that compatible wire representation does not imply one-hop context acceptance. Pending ForceDOS capability classification remains pending.

## 2026-10-26 selected classifier boundary probe

A second isolated x86 test links production `worker/obj/nthook32/classification.obj` (SHA-256 `38412D967AF49481394FCC17C377C8850F8E22FE89AD278B48E95ED3F4308E2D`), corresponding to `hook_classification.c` SHA-256 `6601D0A13A4A3AF1D19C6A94A799FB025DCF19E92C95D81B29F582122CF47FA4`. Probe `forcedos-classification-probe.c` SHA-256 `D8A6B0706AD3B8CAB2775B85980181D9F9C6540319F8ADB24ECBCDB15D82457D` compiled with the flags from `ninja -C build/.ninja/msvc-x86-release/worker -t commands obj/nthook32/classification.obj`, substituting only its source and output; linked with the production object via the existing x86 compiler wrapper. Exit 0 proves `nthook_legacy_type` accepts DOS, WOW and PIF classifier results, but returns `ERROR_NOT_SUPPORTED` for OS/2 NE and native PE results. The underlying original classifier is stubbed, so this is a production dispatch boundary test, not classification of any actual binary and not execution or full integration. Context flag rejection and this independent type gate are two distinct obstacles: merely admitting 0x2000 into the one-hop packet cannot redirect OS/2 NE.

For an end-to-end witness use a small independently authored NE image with a DOS MZ stub and distinct DOS-stub marker, not a modified guest or a native PE; compare direct normal `run16` classification against `CREATE_FORCEDOS` sent through a hook-instrumented native parent using the real NTSRV, with captured output and completion status. The explicit flag path is not currently expressible through the public Run16 CLI; a test parent can call `CreateProcessW` with 0x2000 while already launched under Run16 hook propagation. This tests the current negative behavior without silently inventing a public interface. Coordinate shared service before executing, and do not infer original forced execution from failure on a modern host.

## 2026-10-26 real launch attempt and negative evidence

Independent x86 parent `forcedos-parent-probe.c` SHA-256 `A1229E46D1C6213CB5F685E4D0FFAB00AF6B68F7F860737B97F1075E74031B00` was launched through deployed `O:\winnt\system32\run16.exe`. It calls CreateProcessW on unmodified `O:\winnt\sol.exe` (SHA-256 `68B196C6FD496BAC0E2F30F162EE0B4B4D6EB5BFEC1AF086C8EA759222211F31`), first flags 0x2000 then zero, and observes the returned handle. Both attempts: CreateProcess succeeded; returned process had not exited after 15 seconds; the test terminated only that owned returned process and recorded ERROR_TIMEOUT (1460). The original DOS/Win16 game can remain open by design, so this is not evidence of an error or of which program executed, and does not determine whether 0x2000 reaches the VDM. Each attempt's owned NTSRV and NTVDM were stopped after completion, with no previously existing processes found. Logs are ephemeral `build/t473-s10/shard-3-forcedos/sol-{forced,normal}.txt`.

The independently authored `forcedos-ne-probe.asm` SHA-256 `11B749E0A571FA4710CFF7875BB1CEA09DA5B3E61156E968B499C3CE61D0CD81` assembles, but its candidate binary is classified by host `GetBinaryTypeW` as DOS (1), whereas the real SOL is classified WOW (2). It is therefore not a suitable NE forced-execution witness and was not used for a launch claim. Search of existing tests/tools/available assets by OS2/OS216/ForceDOS names found no explicit OS/2 NE fixture. The end-to-end observable ForceDOS/NE question remains pending; production context and classification boundary unit witnesses above remain valid and bounded. No original guest or product modified.
