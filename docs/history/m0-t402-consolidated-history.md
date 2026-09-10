# m0-t402 historical record

## M0 T402 closure — x64-only residue removal and x86 source restoration

T402 removed the non-mapping-manager native-x64 compile residue from the
current product graph. It reverted the T392 C-VID GDP native-width rule
projection, removed its slot-state overlay and generator, and restored the
original x86 contiguous 64 KiB GDP allocation. It also removed the Win64 CRT
private-name block and x64 termination-context branch.

The product stager now accepts only `--architecture x86`, validates PE I386,
publishes only `ntvdm32.exe`, and removes the retired `ntvdm64.exe` name. The
formal x86 CCPU40 candidate graph compiled and archived successfully; its
explicit `original-softpc-process.exe` link completed after reconciling
pre-existing redirected, BaseSrv, and VDD owner paths left by the earlier
source-tree migration.

Verification passed on 2026-09-10:

- `ninja` completed the generated x86 candidate graph (459 actions).
- `original-softpc-process.exe` linked successfully as Win32/x86.
- The stager rejected `--architecture x64` with exit 2.
- The staged files `build/output/ntvdm32.exe` and
  `O:\ntvdm64\ntvdm32.exe` match SHA-256
  `f65a8ac0523281c35320fd4e6f63e229c1cec33a69b3ca86f0efc1a2a5b62dac`.
- The owner successfully exercised `O:\ntvdm64\ntvdm32.exe` in the real
  runtime package.
- Documentation governance, relative-link validation, and `git diff --check`
  passed.

No mapping-manager surface changed. The retained external/OpenNT historical
headers and historical helper evidence are not active product x64 paths.
