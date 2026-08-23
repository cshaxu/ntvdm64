# bx-mantle

Bochs-only lifecycle and machine assembly over `bx-core`: SIM/logging,
headless native device composition, parameter tree, port-space and PIC
lifecycle.

It may depend on `bx-core` only. It must not contain or import OpenNT, DOS,
VDM, WOW, BOP, SoftPC/CCPU or Win32 semantics. T260 S3 moved all former
`bx_ntvdm_*` bridge, finite-run, observation and machine-session files to
`adapter-softpc`; the retained interfaces use neutral Bochs-native names.

The future static library name is `bx-mantle`; its public include surface is
the Bochs mechanical assembly contract consumed by `adapter-softpc` and `app`.
