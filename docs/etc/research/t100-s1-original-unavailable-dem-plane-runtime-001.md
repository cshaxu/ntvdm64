# T100 S1 original-unavailable DEM plane runtime 001

## Question

Can the whole original OpenNT `demNotYetImplemented` DEM table family enter
the current adapter runtime only by common ingress, registry and DEM-plane
selection, and does the reached `50:42` member preserve its no-op resume
contract?

## Inputs and procedure

`demdisp.c` supplies the eight original table slots (31, 36, 38, 40, 43, 64,
66 and 67) and `demNotYetImplemented` supplies the CF-clear outcome. The
existing provider and exhaustive negative test were added to both the focused
CMake closure and the narrow T98 engine derivative source list. The runtime
performs one common classification chain before calling that provider.

Focused CMake targets passed:

```text
bx-ntvdm-adapter-runtime-test
bx-ntvdm-dem-provider-v1-test
bx-ntvdm-boot-namespace-provider-test
```

The new r1 narrow derivative at
`artifacts/build/bochs-2.6-t100-dem-noop-msvc-r1/` then compiled adapter/CLI
objects and linked successfully. Its binary SHA-256 is
`2B3C892EB479C84DF36C7AA8B5EBF4894DB7D50D9C83E80C86FCC75ACB06CDC1`.
The retained Bochs inputs were copied from r5; no Bochs source, archive,
device, firmware or configuration was rebuilt. One five-second, one-million
instruction observation was recorded at
`artifacts/analysis/t100-s1-dem-noop-v4-probe-001/`.

## Observations

The focused provider test exercises all eight services and proves DEMREAD
`50:16` is not owned. The runtime trace observes `50:42` and records:

```text
cs=9346 eip=00006351 ... flags=00000006 rip=6351 next=6355 delta=00
```

Thus the boundary resumes exactly four bytes after the BOP with CF cleared.
The same run subsequently observes `50:02` normal close, then reports the
pre-existing Bochs prefetch direct-read veto at physical `0x00000000000af6af`.
It has 17 BOP observations and three committed transactions; the watchdog
ends it with exit code 1.

## Interpretation and confidence

This closes the whole original-unavailable family as one source-derived
rehost of the original failure/no-op contract. It does not implement fast
read/write, access guest file data, add a host capability, or make a
continuous-execution claim. Confidence is high that `50:42` is no longer the
current BOP blocker; the next blocker is the Bochs prefetch-map veto, whose
owner and cause require a separate mechanics audit.

## Follow-up

T101 performs a read-only, source/trace correlation of the reached physical
address and minimum-machine map. It may not enable a device, change a Bochs
feature, add a memory mapping, or run another guest trace until it has a
source-backed owner/disposition conclusion.
