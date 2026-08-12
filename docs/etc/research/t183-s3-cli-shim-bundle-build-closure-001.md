# T183 S3 CLI shim/bundle build closure 001

Date: 2026-08-12  
Packet: M0 T183 S3  
Disposition: source/test/build/assembly closure complete; no native engine or guest process started.

## Current CLI build

Fresh MinGW GCC 16.1.0 configuration at
`artifacts/build/current/t183-cli-bundle-mingw-r1` built exactly
`ntdos64-run.exe`, `ntdos64-bochs-engine.exe`, `runner-engine-probe.exe`, and
`bochs-engine-probe.exe`. No Bochs, adapter, native engine,
machine-composition or guest source was compiled. The current
`byob-profile-test.exe` passed.

## V5 runner compatibility correction

The fake-parser policy revealed that accepted v5 profiles did not set
`selection.has_target_placement`, although the runner's fixed DOS-engine gate
still reads that field before deriving the v2 plan. The v5 declared sequence
already owns the target placement, so `byob_profile.c` now projects declared
slot zero into that compatibility field. It neither creates a second target
nor changes the two-slot v5 sequence. The profile test explicitly verifies it.

The fixed-bundle policy fixture was upgraded from obsolete v3 to current v5,
including exact `QUIT.COM`, declarations and metadata. Its successful
fake-parser run proves expected invalid-directive and missing-VGA rejection,
then final fixed shim handoff. It does not start the real bundle engine.

## Fixed bundle assembly

`tools/New-T183S2FixedBochsBundle.ps1` generated
`artifacts/build/current/t183-fixed-bundle-r1`. It contains exactly the named
engine, three fixed ROMs and the five-line content-addressed manifest:

| Member | SHA-256 |
| --- | --- |
| Engine | `B4AAA70645DE2061B34B2D072D852419290BAD4C7FC710DF95C8380F7510898D` |
| BIOS | `D974DE50C94D76445123FD1AC886612351BED7CC5E3C8D07C556814DA72BEBD6` |
| VGA BIOS | `CD9FDD6A789DCD22B8A6B3B152788D43238DE49CCE674CFF57BDEB94580246C6` |
| Option ROM | `E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB` |

This establishes the current runner/shim/bundle process boundary. It does not
establish engine start, BOP arrival, `TARGET`/`QUIT.COM` execution, or CLI DOS
result transport.
