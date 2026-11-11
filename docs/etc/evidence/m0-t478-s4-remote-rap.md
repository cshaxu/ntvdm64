# T478 S4 — RAP boundary and original public server enumeration

## Question and source inputs

Finding 26 asked whether unavailable remote RAP was missing original host
wiring. The [prior audit](../../../artifacts/mvdm-audit/shard-3/remote-rap-role-review.md)
correctly identified RxpTransactSmb, but the historical exclusion also included
XsNetServerEnum2 as though every branch required private transport. Current
original VrNetServerEnum sets ClientTransportName to NULL. In original apisrv.c,
that selects public NetServerEnum, not I_BrowserServerEnumForXactsrv. This is a
concrete recoverable original host connection within the admitted finding.

Baseline: dab9aa8c4 (S3). Original read-only OpenNT revision:
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. Its selected apisrv.c, transact.c,
rxuser.c and lmbrowsr.h were verified unmodified. No source fetch, guest-source
change, original guest rebuild or media patch occurred. Copyright attribution
is retained in the new original subset; this adds no external runtime dependency.

## Per-boundary disposition

| Boundary | Source evidence | S4 disposition |
| --- | --- | --- |
| VrNetTransactApi / VrNetNullTransactApi -> RxpTransactSmb | Original VrpTransactVdm; original rxcommon/transact.c invokes NetpRdrFsControlTree with FSCTL_LMR_TRANSACT | Retain explicit ERROR_CALL_NOT_IMPLEMENTED (120). The NT4 private transport is not a drop-in public Win32 dependency. No new SMB stack is admitted. |
| VrNetRemoteApi -> VrRemoteApi -> RxpTransactSmb | Original descriptor conversion and Unicode server-name allocation/free remain selected | Same retained failure; actual DOS entry exercised. This does not imply local NetAPI is unavailable. |
| RxNetUserPasswordSet / GetLanmanSessionKey | Original rxuser.c uses the remote/private cohort, including FSCTL_LMR_GET_CONNECTION_INFO | Retain error 120. No credential/network transaction was fabricated. Provider object tested; actual DOS password change is not claimed. |
| VrNetServerEnum -> XsNetServerEnum2, NULL transport | Original vrnetapi.c sets NULL; original apisrv.c branches to public NetServerEnum and original conversion | Restore two original functions, remove the project-owned unavailable Xs handler. Both guest forms 5F4C/5F53 share this host owner; actual guest witness uses 5F4C. |
| Non-NULL transport -> I_BrowserServerEnumForXactsrv | Separate original branch with private Browser import | Keep only this import unavailable, with the original WORD/cdecl signature. It is not reached by the current guest wrapper. |
| Local workstation/use APIs | Existing separate original XACTSRV owners | Preserve their implementation; actual local workstation query succeeds after the RAP failures. |

A private-transport limitation is not silently reclassified as an implemented
feature. Conversely, its historical label must not hide a usable public branch.
No requirement to connect a real remote RAP server or implement a new transport
stack is established within this S. Other private cohorts remain explicit,
accepted platform boundaries; finding 26 is dispositioned, not a promise of full
LAN Manager remote compatibility.

## Implementation, ABI and mirror comparison

`src/non-mvdm/ds/netapi/xactsrv/apisrv.c` is an original subset containing
XsNetServerEnum2, XsConvertServerEnumBuffer and their level-0/1 descriptors.
The original response conversion delegates to the already selected XsFillEnumBuffer
and XsPackReturnData. It is selected in original-opennt-xactsrv.lib; the old
XsNetServerEnum2 symbol is removed from the project provider. Product DLL
imports contain NetServerEnum; dumpbin identifies both recovered functions in
apisrv.obj and only I_BrowserServerEnumForXactsrv in the private provider object.
This verifies the production binding separately from the controlled fixture.

OPENNT-HOST-296 registers the finite changes: use the established local-NetAPI
header facade plus the private Browser declaration; initialize entriesFilled
and Header->Converter before early error cleanup; exclude the original DBG
body with #error while retaining its conditional boundary. Other server APIs
and unused level-2/3 descriptors are omitted. No generated mirror is a build
input. Runtime algorithms are original, not copied into an adapter.

The initialized values prevent undefined arguments to XsSetDataCount on early
failure. The imported converter's two level-specific local descriptors retain
compiler warnings about a theoretical other level; its sole selected caller
validates level 0/1 first. That is a reviewed invariant, not a reachable test
failure or a claim to support other levels.

Byte comparison and normalized-newline comparison both differ because this is
an intentional registered subset; neither is a format-only difference. Selected
original code retains CRLF. A negative compile enables DBG only after normal
headers are loaded and rejects at apisrv.c:625, the first original excluded
body line. The continuation maps back to original line 631. Enabling DBG for
the whole translation unit is already rejected by the existing ntpsapi.h gate;
that earlier rejection alone was not used as proof of this new exclusion. Original and subset SHA-256:

- Original: `070437121d8a1ee57ea07672ea9aba3e85a8f4b80b802034cc208383dca4e5cd`.
- Subset: `30543997ed936c2e9b1000e1927755850edd24a2b244b726f4fd3359770d6968`.

## Focused verification and interpretation

`test/observation/verify-redirector-server-enum.ps1` compiles actual apisrv.c
with its production CCPU40/x86 flags, replacing only public NetServerEnum and
NetApiBufferFree at the fixture boundary. It links real original conversion
helpers; the map verifies XsNetServerEnum2 from apisrv-test.obj and XsFillEnumBuffer
from xssubs.obj. The retained run passes 37 assertions: levels 0/1, native-to-RAP
name/version conversion, capacity guards, short and empty output, partial results,
native error, invalid level, private branch, defined error converter and exactly-once
retirement. Controlled native results prove conversion behavior; they do not
claim successful network enumeration on the current machine.

`test/integration/redirector_remote_unavailable_test.c` links the actual product
provider object. It checks 4000 calls (four boundaries, 1000 repetitions) for
error 120 and unchanged output bytes/lengths. It also calls public NetServerEnum
as an independent native comparator. In the sandbox the OS returns access denied
(5); under the same normal user context as the product test it returns 6118.
The acceptance comparison uses the latter, not a hardcoded interchangeable error.

`test/integration/verify-redirector-remote.ps1` assembles an independent COM
probe and runs it through the coherent package in an owned physical O:/tmp stage.
The real original guest dispatch, CCPU40, VDMREDIR and host binding execute:
32 remote API + 32 raw transactions + 32 null transactions return CF=1/AX=120;
receive data endpoints and receive-length sentinel remain intact for the selected
shared-parameter request. Server enumeration then returns CF=1/AX=6118, matching
the direct public native call, with BX=CX=0. Finally local NetWkstaGetInfo succeeds
and the DOS probe exits zero. Repetition/normal completion proves observable
lifecycle continuity; it is not a heap instrumentation proof for every RAP descriptor.

Initial test corrections are retained: the old enum stub maps NTSTATUS
STATUS_NOT_IMPLEMENTED to ERROR_INVALID_FUNCTION (1) through original ntstatus.c,
so expecting 120 for it was wrong. After recovery, public enumeration can take
longer than the initial 30-second observation bound. The timed-out owned stage
was identity-checked and cleaned; with a 120-second bound the final real guest
run completed normally. No timed-out or mismatched run is counted as passing.

The raw original failure paths may preserve or copy input parameter data;
this S does not invent a blanket guarantee that every guest output is zeroed
on failure. Native error and count semantics follow the original owner.

## Build, full verification and delivery

Stable x86/x64 release builds complete; the recovered VDMREDIR DLL links the
original XACTSRV archive and public netapi32. The final candidate package is
8f9f583a-a682-4175-b82e-1f926c9dbba4. Full verification passes all 16 selected gates in 413423 ms. The
owner's DOS4GW/NESticle exclusion remains; it is not a new acceptance claim.
Only this worktree assets/release is authorized for publication, followed by a
local commit. No O:/winnt write or push. S5 is not admitted by this S's work.

## S4 closure and local P1

The tested final package is published only to this worktree assets/release.
All ten SHA-256 values match the staging manifest; exact previous release files
are backed up below build/t478-s4. No installed-runtime publication or push.
P1 contains the original subset, removed local Xs stub, private import boundary,
fixtures, source/build registration, release assets and closure evidence.
Documentation governance and staged diff checks gate the reviewed local commit.

| Released image | SHA-256 |
| --- | --- |
| run16.exe | BA50D31658AED2BFF7605A6FD00AF5E428F39A3EE31C3D90F9808668AC2E9C56 |
| ntsrv.exe | DEB359E41FE8F1730E214A292F025A5E32E23836E7ADDC4FE73942DA77D53474 |
| ntcon.exe | 152DEDCC57F785BDC9D01026DCC09BEAF81074E0BEDE779B816FF46332FBE4DC |
| ntvdm.exe | BA61A9FEBFB19A523FB2151D3C6A1288691B261D7E345C5076E8A3C544975C82 |
| ntvwm.exe | D409B501B93096B176399E0AB4DA902CCD5C390B124D2BDA0929DA5C245029C9 |
| ntmon.exe | 14397DC7AEAFAF19CA5074DF3F5615844AD07847B132F375E21E4E62EB3B4926 |
| WOW32.DLL | 860389495A82F5F0B85EA650255675C0588B01CD95DD5CCCECB8DF71CC476A81 |
| VDMREDIR.DLL | 51AB09340C1321A22E26DBAFBCFDB644ABEF95902F3BCB3DFDDD781067951388 |
| nthook32.dll | 49E10558CF3D2251AD674B318B04394F3CDA320724B0F75161F474CC88873177 |
| nthook64.dll | E0C76D4D4D18E7458C258CE869A35DA178AFD2D9021C29318DE5857F8BA0D0E1 |
