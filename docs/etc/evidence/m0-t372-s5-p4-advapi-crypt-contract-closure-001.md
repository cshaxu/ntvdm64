# M0 T372 S5 P4 — Advapi crypt-contract closure

OpenNT `crypt.h` deliberately names its LM password helpers
`SystemFunction006`, `SystemFunction012` and `SystemFunction016`.  Their
original `vrnetapi.c` declarations use the C default calling convention on
x86; this is preserved exactly by the adapter binding.

Modern x86 `Advapi32.dll` still exports these three names, forwarded to
`CRYPTSP`, while the SDK import library deliberately does not expose them.
`adapter-mvdm-host-out/win32/source/opennt_support_rtl.c` consequently retains
the original names, `NTSTATUS` results and parameter structures and resolves
only those DLL exports at runtime.  An absent export returns
`STATUS_NOT_IMPLEMENTED`; no password material, result or alternative crypto
algorithm is invented.

The formal CPU40/x86 `softpc-win32-bindings.lib` rebuilt successfully.  A
same-input whole-archive `VDMREDIR` audit then reduced unresolved imports from
sixteen to thirteen.  None of the three `SystemFunction*` entries remains.
The residual set is exclusively:

* `RxpTransactSmb`, `RxNetUserPasswordSet`, `GetLanmanSessionKey` and
  `NetpCopyWStrToStr` — the original `rpcxlate`/NetLib owner group;
* `XsNetMessageBufferSend`, `XsNetServiceControl`, `XsNetWkstaGetInfo`,
  `XsNetUseEnum`, `XsNetUseAdd`, `XsNetUseDel`, `XsNetUseGetInfo` and
  `XsNetServerEnum2` — the original XactSrv RAP-server owner group.

The forced audit remains evidence only and is not a product `VDMREDIR.dll`.
