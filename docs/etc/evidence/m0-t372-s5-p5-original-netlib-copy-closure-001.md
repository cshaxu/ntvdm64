# M0 T372 S5 P5 — Original NetLib copy/alloc closure

## Question and scope

Original `mvdm-host/vdmredir/vrnetapi.c` calls
`NetpCopyWStrToStr`.  This P closes that directly reached conversion path by
selecting original OpenNT NetLib source, rather than retaining or extending an
adapter conversion provider.

## Inputs and source identity

- `OpenNT:ds/netapi/netlib/copystr.c` and
  `OpenNT-4.5:nt/private/net/netlib/copystr.c` are byte-identical:
  SHA-256 `69efa31cf86cbcc958a8b7ffa8a6072abcbb024dd3bb4c551a7bc01cea5d66fe`.
- `OpenNT:ds/netapi/netlib/allocstr.c` and its OpenNT-4.5 counterpart are
  byte-identical: SHA-256
  `211efbd9d283a2284d1dfd3ee262c874bf2d0001120a08479eea25fe7441d216`.
- `OpenNT:ds/netapi/netlib/initoem.c` and its OpenNT-4.5 counterpart are
  byte-identical: SHA-256
  `d794e8ca744af7c778854b23e561e68c81b7e874016d933a2da2606638c5842b`.

Those three complete original translation units are mirrored under
`src/opennt-host/netapi/netlib/` and compose into the existing original
NetLib library.  No provider body was copied into an adapter.

`allocstr.c` also contains the historical `NetpAllocTStrFromString` branch.
Its only residual edge is the old `NetapipBufferAllocate`; that function's
original body is the direct forwarder at the end of
`OpenNT:ds/netapi/api/apibuff.c` (OpenNT/OpenNT-4.5 SHA-256
`420a2973fd10924de9d6f709a88a79ef05bad74c61eaea1ea5b678b181c31161`).
The selected `src/opennt-host/netapi/api/apibuff.c` is a registered true
subset retaining that body unchanged and forwarding to the public modern
`NetApiBufferAllocate` contract.  The omitted source functions require the
otherwise unreached historical RPC/MIDL allocator product shell, so are not
admitted by this Redirector slice.

`ntrtl.h` receives only the two original declarations required by `copystr`.
They remove implicit-declaration C4013 diagnostics without changing the
original caller.  Historical C4996 and the original `LONG`/`PULONG` C4057
diagnostics remain visible; they are not suppressed or altered because they
are not an x86/x64 interface recovery in this CPU40/x86 packet.

## Procedure and observations

The formal CPU40/x86 graph was regenerated with Node 22 and compiled:

```text
ninja -C build/M0-T371/S2/formal-x86 -j 8 \
  original-opennt-netlib.lib original-opennt-netapi-api.lib \
  original-mvdm-redir.lib
```

All selected NetLib/NetAPI and original Redirector objects compiled and their
static libraries were emitted.

The whole-archive parent/DLL import audit was then rerun with the new original
NetLib and selected NetAPI library.  The former four NetLib unresolved names
(`NetpCopyWStrToStr`, `NetpAllocStrFromWStr`, `NetpAllocWStrFromStr`,
`NetpInitOemString`) and the consequent `NetapipBufferAllocate` tail are all
absent.  The exact residual set is now twelve owner-group imports:

```text
RxpTransactSmb
XsNetMessageBufferSend
XsNetServiceControl
XsNetWkstaGetInfo
XsNetUseEnum
XsNetUseAdd
XsNetUseDel
XsNetUseGetInfo
XsNetServerEnum2
RxNetUserPasswordSet
GetLanmanSessionKey
```

(`RxpTransactSmb` is referenced by two original Redirector callers but is one
owner symbol.)  These are unchanged S5 P1 `rpcxlate`/`xactsrv` groups.  This
is source/link evidence only: it neither emits the final `VDMREDIR.dll` nor
claims guest `57:xx` execution, both of which remain S6 work.

## Interpretation, confidence and follow-up

The original NetLib conversion/alloc group is closed as an admitted source
mirror plus public NetAPI binding.  The remaining twelve imports retain their
separate original-owner dispositions; no private NT4/RAP transport is silently
replaced by this P.  Confidence is high for source identity and formal x86
compile/link classification; no statement is made about a final dynamic DLL or
guest network behavior.  S5 continues with the explicitly separate `rpcxlate`
and `xactsrv` owner groups, and S6 owns whole-image and guest observation.
