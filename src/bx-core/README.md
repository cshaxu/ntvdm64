# bx-core

Adopted Bochs 2.6 core: CPU/decode, memory, exceptions and admitted native
machine mechanics.

It may depend only on adopted Bochs/internal machine inputs. It must not know
OpenNT, DOS, VDM, WOW, BOP, SoftPC/CCPU or Win32 policy.

## Local intrusion register

Every modification to adopted source is registered here with upstream path and
identity, exception identifier, necessity and focused verification. The
existing detailed register remains
[`adapter-external-intrusion-exceptions.md`](../../docs/etc/research/adapter-external-intrusion-exceptions.md)
until S3 transfers each applicable record into this component register. No new
core modification is admitted without a row here.

The future static library name is `bx-core` and its public include surface is
only the explicitly declared Bochs mechanical API consumed by `bx-mantle`.
