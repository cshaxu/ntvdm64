# T189 S3/S4 two-object derivative closure 001

Date: 2026-08-12  
Packet: M0 T189 S3/S4  
Disposition: exact derivative generated and built; no guest execution.

`tools/build/New-T188S7CopiedStateTraceDerivative.ps1` created
`artifacts/build/current/t189-boot-file-stage-derivative-r1` from the retained
T187/r5 root. Its manifest declares exactly `cpu/exception.o` and
`adapter/bx_ntvdm_adapter_runtime.obj` as targets, retains 71 adapter/CLI
objects and all Bochs archives/device inputs, and hashes the current diagnostic
sources.

The sole MSVC x86 command built those two objects and linked the final engine.
It exited zero. Retained warnings were C4244, LNK4042, LNK4098 and LNK4217; no
warning was repaired or reclassified. No archive, device, configuration, full
build, CLI build or guest run occurred.

| Output | SHA-256 |
| --- | --- |
| Engine | `6F3801280DACD1D76125DEB3227053AFFBB957D2B61A026855F63CD010FF783D` |
| Map | `68B8CD07D6B961BACFB0785900B2126FC3F4B4F3C3149925A29A9DB41796E7D3` |
| exception object | `666FBD710DE3568A8560E9692A134740E28759DF8E8149965990016A53299016` |
| adapter runtime object | `4953087A8AD11B28AE67BF20DCF7B8B4744A93C427441CDC36CC46EF50247C8C` |
