# VDMREDIR DLL target component

This directory owns only the non-mirror binding edge of `VDMREDIR.DLL`.
Original source, resource, definition and export ownership remain in
`src/mvdm-host/vdmredir/`.

`source/` contains three finite bindings:

- the x86 stdcall loader entry to original `VrDllInitialize`;
- bounded guest copy orchestration using the worker's shared address and lease contracts;
- the source-proven unavailable private RAP transport imports.

It does not own a session, TLS, CCPU executor, guest-memory lease, BaseSrv
state or any original VDMREDIR algorithm. Compatibility copy exports and
asynchronous completion bindings remain in `src/ntvdm-exe/redir/`.

The OEM copy boundary accepts capacity in encoded bytes including NUL and
returns the existing LAN buffer-too-small status before any guest write. Its
caller preserves the original unchecked BX=0/CX=0 cases. CD-name copies read
each far destination after the preceding result write, preserving original
aliasing order rather than freezing the packed pointer trio. These are
project-boundary corrections, not new guest error or atomic-write semantics.

The public server-enumeration route is owned by original
`non-mvdm/ds/netapi/xactsrv/apisrv.c`: `XsNetServerEnum2` calls public
`NetServerEnum` for the NULL transport supplied by original `VrNetServerEnum`.
The former unavailable Xs handler is removed. Only its private
`I_BrowserServerEnumForXactsrv` import remains in the unavailable boundary;
its WORD/cdecl signature is declared in `include/remote_browser.h`.
The raw RAP transaction, remote password and Lanman session-key providers
still return `ERROR_CALL_NOT_IMPLEMENTED`; no SMB or credential implementation
is synthesized. Public enumeration reports actual native success/failure,
not the old blanket `STATUS_NOT_IMPLEMENTED`.
