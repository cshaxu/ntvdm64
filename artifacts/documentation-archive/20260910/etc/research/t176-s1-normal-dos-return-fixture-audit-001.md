# T176 S1 normal DOS return fixture audit 001

## Question

Which existing OpenNT guest source and artifact evidence can support the first
honest observation of a normal DOS child return into COMMAND service 54:11?

## Inputs and procedure

The audit read the OpenNT SHARE and FASTOPEN source, their make/link inputs,
the DOS/WOW16 source-closure inventory, the staged-artifact audit, the
historical toolchain manifests, source policy, and the already-audited COMMAND
return continuation in tcode.asm. It did not create, import, rebuild, copy,
or execute any guest image.

## Candidate classification

| Candidate | Source behaviour | Closure evidence | Current runtime disposition |
| --- | --- | --- | --- |
| QUIT.COM | BOP BOP_UNSIMULATE, encoded C4 C4 FE. | Existing exact controlled-stop profile and T161--T169 observations. | Rejected for this question: it proves a controlled stop, not normal DOS return. |
| SHARE.EXE | At origin 0100, set AH to 4Ch, set AL to zero, invoke INT 21h, then RET. OpenNT documents it as an NT no-op stub. | Inventory classifies it source-built at OpenNT revision 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c; manifest records 882 bytes and SHA-256 69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC. | Candidate only. It is explicitly not runtime-admitted by source-build evidence. |
| FASTOPEN.EXE | Identical normal termination sequence and NT no-op role. | Same 882-byte SHA-256 and source-built classification under its own source/build manifest. | Equivalent candidate, retained as a cross-check rather than a second target. |
| Fabricated smoke COM | Could trivially invoke INT 21h/4Ch. | No OpenNT provenance or declared source-built closure. | Rejected by source policy. |

SHARE.EXE is selected as the canonical future fixture because its source states
the intended NT no-op role and its provenance is separately recorded. The
selection does not make it an admitted runtime input.

## Provenance and availability result

The two historical manifests are present:

- artifacts/toolchain-runs/share-tools16-opennt-v1/share-build-manifest.json
- artifacts/toolchain-runs/fastopen-tools16-opennt-v1/fastopen-build-manifest.json

Each records the historical MASM.EXE and LINK16.EXE inputs, no guest-source
transforms, the 882-byte output size and the hash above. Their Output fields
refer to the former D:\home workspace paths. The corresponding SHARE.EXE and
FASTOPEN.EXE files are absent from this current O: workspace. Therefore the
manifests establish source-build evidence but supply no current binary eligible
for BYOB runtime admission.

## Expected normal-return continuation

The source termination request is INT 21h/AH=4Ch with AL zero. If the guest
DOS path completes normally, COMMAND tcode.asm records the child return code,
places it in DX, obtains the current drive through INT 21h/AH=19h, carries its
RdrInfo pair in CX:BX, and emits CMDSVC SVC_RETURNEXITCODE, the four-byte BOP
form C4 C4 54 11. The T174 paired provider must then copy DX into contained
adapter lifecycle state, advance exactly four bytes, clear CF, preserve AX,
and resume. In the source no-next branch, CF clear continues at reent_exit for
the 32-bit shell path or d16_loop for the 16-bit shell path. This is an
expectation for a future trace, not a claim that the current guest DOS path
already reaches it.

## Result and next task

No existing normal-return guest binary is runtime-admitted. The smallest
admissible next package is a reproducibility-first SHARE.EXE recovery and one
bounded declared-target observation: verify or reproduce the exact source-built
output with source-policy metadata before profile admission; then, only if the
artifact and profile validation succeed, run one bounded trace looking for the
normal 54:11 call and its CF-clear continuation. It must stop rather than
substitute semantics if toolchain recovery, image loading, DOS termination, or
the expected continuation is absent.

## Confidence and limitations

High that SHARE and FASTOPEN have the source-shaped normal termination
contract and historical source-build provenance. High that neither is a
current runtime asset because the manifest-referenced output files are absent.
No runtime, BOP-provider, Bochs, CLI, or host-capability conclusion is made.

