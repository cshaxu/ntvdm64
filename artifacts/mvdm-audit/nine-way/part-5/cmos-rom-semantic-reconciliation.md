# Part 5 semantic reconciliation: ROM and CMOS provider ownership

## Actual selected owner

`softpc.new/base/support/main.c` calls `cmos_pickup()` and normal termination calls `cmos_update()`, but those names alone do **not** mean the captured product reads or writes `cmos.ram`. The original NT-specific `softpc.new/base/system/sources` and current Ninja graph select **cmosnt.c**, not generic cmos.c. Original `cmosnt.c::cmos_pickup` is intentionally empty because it uses statically initialized CMOS plus POST; `cmos_update` is intentionally empty. Published NTVDM's `_cmos_pickup`, `_cmos_update`, `_cmos_init`, and `_cmos_post` bind to the actual `cmosnt.obj` functions in the companion observer; the linker folds the two empty entries at the same address. The selected device registers ports and RTC EOI, and static state/post remains the authority. Thus there is no established requirement to copy either original `cmos.ram` into the package for this captured NT provider. This is a provider-specific conclusion, not proof that alternate generic SoftPC profiles or any other possible CMOS-file consumer are unused.

ROM has a different owner. Current original `rom.c` uses the original generic residency sequence under registered `MVDM-HOST-DIV-165`: fills/marks the ROM range, loads video and BIOS, then protects it. Original `nt_rez.c` routes `ROMS_REZ_ID` to project `mvdm_softpc_firmware_read_embedded_rom`, which reads named resources in the main module. The earlier part-5 resource observer independently compares the published NTVDM's three RT_RCDATA payloads byte-for-byte with both `data/` and `roms/` copies. Missing/oversized/failed resource causes the helper to return zero; original `read_rom` skips copying and callers do not check the result. This establishes a failure-path question, **not** a reproduced production failure or authority to change failure semantics.

## Owned files: actual role and limits

| Part-5 key | Owner, proof and scoped recommendation |
| --- | --- |
| `softpc.new/data/bios1.rom` | Exact duplicate of roms/bios1 and full published `SOFTPC_BIOS1` payload. Partial content binding; guest execution unproved. |
| `softpc.new/roms/bios1.rom` | Selected named source for same published resource; exact bytes. Partial resource/content, not runtime acceptance. |
| `softpc.new/data/bios4.rom` | Exact duplicate of roms/bios4 and published `SOFTPC_BIOS4` payload. Partial content binding. |
| `softpc.new/roms/bios4.rom` | CPU40 original ROM name; exact published resource. Partial content binding, alternate profiles separately scoped. |
| `softpc.new/data/v7vga.rom` | Exact duplicate of roms/v7vga and published `SOFTPC_V7VGA`. Partial content binding, not proof of VGA modes or cursor behavior. |
| `softpc.new/roms/v7vga.rom` | Resource source with exact published bytes. Partial resource evidence, test Video7 pointer/modes/teardown separately. |
| `softpc.new/data/cmos.ram` | Byte-identical to roms/cmos.ram but not read by selected `cmosnt` pickup. Exclude only this file's selected-NT-CMOS input role, preserve alternate providers/other consumers and RTC capability. |
| `softpc.new/roms/cmos.ram` | Same as preceding; selected owner is static `cmosnt` state, not this file. Same limited exclusion. |
| `softpc.new/data/profile.spc` | Duplicate of roms/profile.spc; non-ROM host file path exists, active selected consumer and runtime state not established. Pending. |
| `softpc.new/roms/profile.spc` | Same source identity; pending actual profile selection/consumer. |
| `softpc.new/roms/bios2.rom` | Original non-CPU40 ROM branch; current embedded provider names bios4. Pending alternate profile and runtime applicability, not general exclusion. |
| `softpc.new/roms/bios1.asm` | Original assembly retained; provenance to published BIOS1 image is unproved. Pending. |
| `softpc.new/roms/bios4.asm` | Original assembly retained; provenance to published BIOS4 image is unproved. Pending. |

The remaining 20 owned `softpc.new/bios/` files are individually inventoried in `bios-resource-family-review.md`: historical NTIO makefile/link source roles are build inputs; current ZIP and installed NTIO are byte-identical but source-to-image lineage and calls are not proven. No guest code is modified or rebuilt.

## Existing evidence and finite follow-up

- `tests/observation/s8_embedded_rom_resource_probe.c` checks names and sizes and rejects cmos/profile resources; the new resource observer additionally compares exact bytes, neither is guest execution.
- `tests/session/softpc_firmware_resource_fixture.c` checks existing/missing path, short output and unknown ROM name, not failure of an actually required embedded resource.
- Prior `artifacts/mvdm-audit/shard-3/cmos-rtc-device-audit.md` records 24-assertion PIC/RTC reset fixture coverage and an earlier periodic register-C guest witness; these are bounded historical evidence, not proof of current full IRQ8/alarm lifecycle.
- S13 owns a finite device verification group: test required resource absent, unreadable and oversize on an isolated matched build; observe the original `rom_init`/outer owner outcome rather than inventing success or forcing a guest image change. Exercise real IRQ8 register-C consume, reset/cancel/requeue, alarm/time changes and task/worker retirement with deterministic fixture boundaries, then a guest witness where supported. Any semantic policy change after establishing the failure requires its proper authority; no code change is asserted here.
- S19 reruns the applicable published-resource and device regressions against the matching final artifact. The exact negative and guest failures, any unavailable device cases and artifact hashes must remain explicit.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-5/cmos-nt-owner-observer.py` and compare JSON; the 33-file resource observer verifies the actual resource payloads. No production test was newly run and no product, guest, external source, build tree or shared ledger was modified.
