# m0-t399 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t399-x86-product-retirement-closure-20260909.md

# M0 T399 closure — x64 product retirement and x86 source restoration

T399 closes the owner's x64-product retirement instruction. The formal
generator accepts only x86, the current product PE is I386, and both
publication paths reject any `ntvdm64.exe` / x64 product request. The only
runtime/acceptance executable is `ntvdm32.exe`.

Older x64-capable focused-helper generators remain historical evidence. They
are neither active product composition nor a reason to produce x64-only
repairs. The sole future exception is a demonstrated architecture-neutral
mapping-manager correctness defect, which requires its own admission.

The full commands, hashes and residual-helper classification are in the
[T399 S1 audit](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t399-s1-x86-product-retirement-audit-001.md).
No numeric M/T/S packet is active after this closure.

