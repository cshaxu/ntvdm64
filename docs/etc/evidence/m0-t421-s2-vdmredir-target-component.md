# M0 T421 S2 — VDMREDIR target-component recovery

## Question

Can `VDMREDIR.DLL` own its own finite standalone bindings without allowing the
worker to own DLL initialization, remote-transport policy, or DLL-local guest
copy publication?

## Change

`src/vdmredir-dll/` now owns exactly three non-mirror translation units:

- stdcall entry adaptation to the unchanged original `VrDllInitialize`;
- bounded numeric guest-copy requests into the one worker; and
- the source-proven unavailable private RAP transport imports.

The worker retains only its process-local guest-memory-lease copy provider and
asynchronous completion provider.  No original source moved or changed except
the one `vrnetapi.c` include spelling, which now resolves its target-owned
header through the component include root.

## Fresh build and ABI proof

The formal x86 graph was freshly generated in
`build/M0-T421/S2/formal-x86-r2` with Node 22 and executed through the
project's declared serial-Ninja fallback (514 commands, exit 0).  It linked
`VDMREDIR.dll` with `vdmredir-dll-bindings.lib`; `ntvdm.exe` does not link that
archive.

`dumpbin` confirms x86 machine `14C`, the original ten public `Vr*` exports,
and worker imports `mvdm_redirector_worker_copy_from/to`, asynchronous
completion helpers, and session-thread binding APIs.  Thus the parent worker,
not a second DLL session, remains the owner of guest state.

## Matching runtime evidence

The matching `ntvdm.exe` and `VDMREDIR.DLL` were copied to `O:\winnt` after
backing up the prior two product files under `O:\winnt\builds\M0-T421-S2`.
Their SHA-256 values equal the formal outputs.  No guest asset was changed.

Fresh DOS probes were temporarily placed for normal COMMAND lookup and moved
afterward to that same test-build archive.  The ConPTY observer passed:

- pipe read, transact, call, async read and async write;
- mailslot; local NetAPI, enumeration, workstation, message, service,
  assignment and NetUse lifecycle;
- the established interactive `COMMAND -> MEM -> EDIT -> MEM` regression.

Every observer returned zero and its guest COMMAND returned normally after its
follow-up `MEM`.  The pipe trace records `READ-RETURNED` and
`VDMREDIR-TYPE-OK` in
`O:\winnt\logs\m0-t421-s2-vdmredir-pipe.raw`.

## Result

S2 is complete: the DLL is a named component with a target-only manifest and
no worker lifecycle ownership leaked into it.  Its reachable original
redirector behavior and its approved RAP exclusion are unchanged.
