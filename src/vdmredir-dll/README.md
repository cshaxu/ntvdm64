# VDMREDIR DLL target component

This directory owns only the non-mirror binding edge of `VDMREDIR.DLL`.
Original source, resource, definition and export ownership remain in
`src/mvdm/vdmredir/`.

`source/` contains three finite bindings:

- the x86 stdcall loader entry to original `VrDllInitialize`;
- bounded numeric guest copy calls imported from the one `ntvdm.exe` worker;
- the source-proven unavailable private RAP transport imports.

It does not own a session, TLS, CCPU executor, guest-memory lease, BaseSrv
state or any original VDMREDIR algorithm.  Worker-local copy and asynchronous
completion bindings remain in `src/ntvdm-exe/redir/`.

The OEM copy boundary accepts capacity in encoded bytes including NUL and
returns the existing LAN buffer-too-small status before any guest write. Its
caller preserves the original unchecked BX=0/CX=0 cases. CD-name copies read
each far destination after the preceding result write, preserving original
aliasing order rather than freezing the packed pointer trio. These are
project-boundary corrections, not new guest error or atomic-write semantics.
