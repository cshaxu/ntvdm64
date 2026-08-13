# M0 T194 S35: BOP-Register Listener Bundle

Packet: M0 T194 S35  
Disposition: immutable bundle assembled; no process started.

`New-T190S5CommandBootstrapBundle.ps1` assembled
`artifacts/build/current/t194-s35-bop-register-bundle-r1` from the S32 engine
and S27 ROM root. Its manifest records engine SHA-256
`49D950AE537B0F9343F2B07E0A2B92DD6B18EB612D572A459559EC0FB3B2AB5D` and
the unchanged BIOS, VGA and option-ROM hashes from S27. The tool copied and
hashed inputs only; no runner, shim, controller, engine, CPU loop or guest was
executed.
