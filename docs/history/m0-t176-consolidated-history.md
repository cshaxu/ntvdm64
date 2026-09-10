# m0-t176 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t176-bop-recovery-closure-20260812.md

# M0 T176 Closure — BOP Recovery And Normal-Return Boundary

## Result

T176 closes the bounded recovery of the reached machine BOP `06h` without
claiming continuous execution or CLI completion.

- S21 established that `06h` is original SoftPC `illegal_op_int`, not an
  adapter host service.
- S22 established and tested the minimal same-island mechanics contract.
- S23 linked unchanged `illegalp.c.obj` and `unexp_nt.c.obj` in the narrow
  derivative without rebuilding Bochs archives.
- S24 made one controlled observation. It reached the prior real-mode stack
  fault at `0000:019d` before any observed `06h` entry.

## Boundary

No adapter BOP provider, optional device, retry, synthetic reachability path,
or OpenNT rewrite is admitted by this closure. The next admitted work is a
read-only ownership attribution package, not a repair package.


## m0-t176-closure-20260812.md

# M0 T176 closure - 2026-08-12

T176 closes the normal DOS return fixture audit without creating or executing a
guest artifact. It distinguishes QUIT.COM's exact FE controlled stop from
normal DOS termination, identifies SHARE.EXE as the canonical OpenNT
source-built normal-return candidate, and retains FASTOPEN.EXE as an equivalent
cross-check. Both historical manifests are present and agree on a no-transform
882-byte output with SHA-256 69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC,
but their referenced binaries are absent from the current workspace. They are
therefore not runtime-admitted. T177 is admitted to recover or reverify one
source-policy-compliant SHARE.EXE fixture before exactly one bounded normal
return observation; it must not fabricate an input or change BOP, Bochs, DOS,
or CLI semantics.


