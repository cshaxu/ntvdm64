# T177 S1 source-built normal-return fixture and observation 001

## Objective and inputs

T177 selected only the T176 SHARE.EXE candidate. The source is
src/opennt/base/mvdm/dos/v86/cmd/share/share.asm at OpenNT revision
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. Its normal termination sequence is
MOV AH,4Ch; XOR AL,AL; INT 21h; RET.

## Reproducibility

A fresh isolated build at artifacts/toolchain-runs/t177-share-normal-return-r1
copied the exact source and response file, then used the historical tools:

- MASM.EXE SHA-256 E54AFAA9565E046CD862C29A015D685DE43311ACCCF796469030E162CED6D00D
- LINK16.EXE SHA-256 910ED26E6C8D72D64FAA82A7EEABB6D5C002B347024C8F1D778FDDBBB8AC1B00

MASM reported zero warning and severe errors. LINK16 exited zero. The output is
882 bytes with SHA-256
69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC,
which exactly equals the preserved historical manifest. The stage-local
recovery manifest records source, role, tool, size, hash and internal-only
redistribution disposition.

## Immutable observation setup

The observation directory
artifacts/analysis/t177-s1-share-normal-return-observation-001 contains a
v4 profile cloned from the retained native-POST evidence only for its fixed
machine and boot-resource contract. Its target descriptor is TARGET.EXE,
882 bytes and the recovered SHARE hash. The target path is \\TARGET.EXE and
the copied launch declaration is EXE with an empty tail. The preflight record
pins all component, option-ROM, profile and binary identities.

## Single observation result

Exactly one native launch was requested against the retained T174 current
adapter binary with a 10-million benchmark and a 30-second watchdog. It did
not yield a Bochs log or observation record. The outer controlled execution
was interrupted at 45 seconds; its attempted process-tree termination returned
Access denied. A follow-up read found neither the target process nor bochs.log.

This is a control-plane failure before any auditable guest marker. It does not
mean that SHARE failed to load, INT 21h/AH=4Ch failed, 54:11 was absent, or the
CF-clear continuation failed. The T177 packet forbids a retry, so none was
performed.

## Disposition

The source-built SHARE artifact is reproducible and is a valid future
profile-pinned candidate. Runtime admission remains suspended until a separate
native observation-controller audit proves that a bounded child launch can
reliably retain its process outcome and log without ambiguous external
termination. No BOP, Bochs, DOS, adapter, CLI, host capability or result ABI
changed.

