# M0 T418 S3 BaseSrv ownership

The live service entry, transport and private BaseSrv bindings moved from the
former `app`, `broker` and `adapter-opennt-host` roots into `src/basesrv`:
`main.c`, `console_query.*`, `transport/` and `opennt/{include,source}/`.
No service wire layout or original record policy changed. `srvvdm.c` remains in
the strict OpenNT mirror.

Fresh graph `build/M0-T418/S3/basesrv-owner-x86-001` selected no former
service-root path and completed all 506 x86 commands. The only final unresolved
names are the existing intentional `/FORCE` original-closure audit. CCPU and
C-VID fixtures passed. The separately rebuilt reservation fixture passed its
worker claim/generation/release checks, and the BaseSrv fixture passed original
Check/Update/Get/ExitVDM lifecycle through authenticated worker binding.
