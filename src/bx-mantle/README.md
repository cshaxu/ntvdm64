# bx-mantle

Bochs-only lifecycle and machine assembly over `bx-core`: SIM/logging, minimal
machine construction, finite run/stop and native mechanical device lifecycle.

It may depend on `bx-core` only. It must not contain or import OpenNT, DOS,
VDM, WOW, BOP, SoftPC/CCPU or Win32 semantics. S3 removes every current file or
statement that violates this boundary.

The future static library name is `bx-mantle`; its public include surface is
the Bochs mechanical assembly contract consumed by `adapter-softpc` and `app`.
