# T478 S3 — ForceDOS host flag integration

## Question, inputs and ownership

Finding 20 had a real missing producer: an already instrumented native parent
could request CREATE_FORCEDOS, but Hook retained only separate-WOW metadata,
and Run16 chose WOW/GUI ownership before reading that metadata. The original
COMMAND consumer already existed. This S connects the supported parent route;
it does not import a standalone utility or change guest media.

Source baseline is cd91c8a57 after rebase onto main cd8c8583e. Original OpenNT
base/win32/client/process.c and COMMAND are the reference owners described in
the [investigation](../operations/m0-t478-s3-forcedos-plan.md) and its linked
historical audit. No upstream mirror or original guest file changes in this S;
there are no newly selected mirror files requiring a format/divergence sweep.
The independently authored NE test is not an original guest modification.

## Implementation and review

- Hook retains FORCE_DOS and SEPARATE_WOW only for a redirected launcher;
  INTERCEPT children have zero creation flags. The packet validator accepts
  both individual flags and their combination only in LAUNCHER mode, rejecting
  unrelated bits. Wire layout/version 5 is unchanged; use the coherent package.
- The classifier accepts OS2 NE only with FORCE_DOS in addition to its existing
  DOS/WOW/PIF classes. Native PE is never turned into DOS by extension or flag.
- Run16 consumes its one-hop seed before frontend/GUI selection. Forced WOW or
  OS2 classification becomes DOS. The launcher owns and releases attached
  handles once, including dispatch failures; the seeded scope borrows them,
  retaining its own frontend capability through the existing broker route.
- launch_vdm forwards the original bit to BaseCheckVDM. Existing NTSRV CHECK/GET
  transport and COMMAND cmdmisc.c retain their original implementation. The
  command assignment derives DontCheckDosBinaryType from each new request;
  native descendants do not acquire a persistent ForceDOS policy.
- No second execution algorithm, new RPC, public Run16 switch or replacement
  COMMAND implementation was introduced. There was no duplicate local
  ForceDOS algorithm to delete: the missing producer and dispatch were fixed.

Original limitations remain explicit: Hook does not redirect new-console,
DETACHED_PROCESS, CREATE_NO_WINDOW or debugger creation modes. Their existing
source gates are unchanged, not newly supported by this S. PIF may overwrite
DontCheckDosBinaryType according to its original subsystem logic; no PIF repair
is claimed. A 64-bit original classifier maps invalid OS2 NE to DOS through its
non-_X86_ path, while the 32-bit classifier has SCS_OS216_BINARY. The OS2
conditional dispatch was reviewed; no real OS2 application execution is
claimed by the Win16 NE test. This is ForceDOS DOS-stub support, not an OS2
subsystem implementation.

## Reproduction and expected observations

Build release targets through the stable msvc-x86-release and
msvc-x64-release trees, as recorded in build/t478-s3/build-native.cmd,
build-x86.cmd and build-rest.log. Compile packet_test.c with packet.c for both
widths, and compile forcedos_parent_probe.c with /MT for each native caller.
The existing console_startup_observer.c provides the process/exit observation.
GENHOST.DLL is built from the independent wow_generic_callback_probe.c fixture using
the export arguments in verify-wow-generic-call.ps1; the DLL is only a test dependency, not a product input.

The retained test/integration/verify-forcedos.ps1 takes explicit PackageRoot,
BuildRoot, Observer, NativeParent and GenericHostDll arguments. It assembles the
independent NE using NASM; Case selects forced, normal, nested, native or
sequence, with -Ansi selecting CreateProcessA instead of W. Each test uses an
isolated physical runtime and cleans up its owned process scope. Outputs and
guest probes stay below the disposable test/runtime roots.

- Forced: the DOS stub prints FORCEDOS_STUB_MARKER and returns 53; no Win16
  GENPROBE.BIN is created.
- Normal: the same image enters WOW and creates the 54-byte generic-call report,
  returning zero. This is the contrast that establishes a real WOW input.
- Nested: the forced DOS stub EXECs FCHILD.EXE by relative name; both markers
  appear and the outer DOS program returns 55 after checking child return 53.
  This covers COMMAND's original bypass, inherited output and working directory.
- Native: the native parent image is the target; it returns its no-argument
  status 90, preserving native PE execution even with FORCE_DOS.
- Sequence: one parent launches the same NE first forced (53), then ordinary
  (0 and the Win16 report), proving the flag is not a descendant/request policy.
  COMMAND's same-worker reset is separately source-verified at cmdmisc.c's
  per-command assignment; the sequence does not claim reuse of a DOS worker.

## Verification and local delivery

Release builds passed. Full passes 16/16 in 435463 ms, with identical ten-image
identity throughout (build/t478-s3/verify-Full/timings.json). This includes
WOW frontier, Console/Window, RPC, native GUI, version negatives, strict DIR,
EDIT return, cooked return, rapid/interactive relaunch, session isolation,
retirement and nested I/O handoff. DOS4GW/NESticle retains the owner's exclusion.
The retained ForceDOS runner passes all 18 cases: four cases across A/W and
x86/x64, plus two same-parent sequence cases. Each width's packet test passes
four valid and thirteen invalid cases. All six ordinary/sequence guest reports
also pass the three GEN1 phase/status checks. Logs remain under build/t478-s3.

Published ten images to this worktree assets/release; every SHA-256 matches
the tested candidate manifest. Exact prior files are backed up under the task
build root. P1 contains the implementation, fixtures, evidence and local assets;
documentation governance and staged diff checks gate the local commit. Owner scope
is this worktree assets/release only, local commit and no push. O:/winnt is not
a publication target. Immutable inputs remain unchanged. S4 is not admitted.

## Candidate identity

Package ID: 398f8811-ff8a-45dc-8c00-abef023ccf2c.

| Image | SHA-256 |
| --- | --- |
| run16.exe | 4598734DC286BEBC628257F54B87A653F30EC61C6721DC7819F293DD61C55E62 |
| ntsrv.exe | 2265BB63EE6B36A06855DC39DE7DFF8BBE4604F84DE62C5FE4E13E59D5998D23 |
| ntcon.exe | 3BC67F55567C3991081F2C93F0CE969778DACB793891EFBC26652EEAF7E2953E |
| ntvdm.exe | 81B25F2544C2268920A34F6A0680E9967F39FD298EDE58D4F047D937A2EE2C2B |
| ntvwm.exe | 9A4CB682ED3F279CB13E20B38E1FFE30AF95C9B76A3CF2BC9DE456796821D017 |
| ntmon.exe | C3888AB0950B3CA8272AB266E8307B194BB5B5B0C800F9C08A81106ECD2FC6B3 |
| WOW32.DLL | 236F9112048FF579197097E800D88AFB36FF45310B132B92602F6D80F3F26531 |
| VDMREDIR.DLL | 8D76F05864248C072DA2CB004293D721DF9E75F84AC4E3569B3D7FD36DF1B1AD |
| nthook32.dll | 179FCDDC35E4C1502D5538E7B0A67EB6906563DA746FF6B5BAA4AEC7DFF3D4B7 |
| nthook64.dll | 563E08F7E5138F882518E5D2954D4BA0DDA40AB9885D85643E35EEB0BF402599 |
