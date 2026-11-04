# T475 S4 — NTHook dependency cleanup

## Admission and P1

The owner directs removal of NTHook private dependencies. The post-S3 audit
found Run16, NTVDM and NTVWM selecting actual NTHook-owned context and/or
installer objects. It also found that S3's Redir header check covered the 18
component objects but missed the private SoftPC forced header on actually
selected OpenNT archive members and on selected Hook32 shared objects. That
prior S3 header-closure claim is corrected; S4 P3 must address both closures.

P1 moves the existing context structure, payload reader and package-path
construction from NTHook into `common/native-hook`, retaining the packet GUID,
names, calling conventions and data handling. Run16 uses the common header
and archive; NTHook and both workers reuse the same source. The launcher
compiler dependency and private-header negative control passed.

The x86/x64 product build, 63 immutable guest files, 274 export ordinals and
all 12 selected Control gates passed. Actual x86 and x64 Hook fixtures each
passed 118 assertions; the x64/x86 alternating descendant run passed 168,
including payload, handle transfer, invalid inputs and lifecycle failures.
The first isolated x86 fixture lacked its required pinned Run16 image and
failed; a fresh directory with that prerequisite passed. Local package
`563bc615-556c-49e6-869e-6d7ea0004396` has ten verified release hashes.
Full was not run; DOS4GW/NESticle was absent. Logs, candidate and prior asset
backup are under `build/t475-s4-p1`. Main integration and push remain deferred
by the owner.
