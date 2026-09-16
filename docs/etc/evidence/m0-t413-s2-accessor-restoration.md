# T413 S2 — Original accessor restoration

## Inputs and recovery decision

Baseline 23a4fa83c and the [S1 audit](m0-t413-s1-accessor-layout-audit.md).
Owner admits repair. Profile is x86 /MT CCPU40 plus the complete C-VID host
accessor dispatch, not instruction execution, x87 or a replacement renderer.

Recovery ladder: (1) unchanged original cpu/src/evid/vglob.c actually compiles
and links; (2) the existing private product binder selects its 38 pairs, with
20 one-line typed conversions for ten distinct pointer/signed pairs; (3) no
new mirror intrusion is needed; (4) no new field-access algorithm is needed.
Original two latch providers and direct selector functions remain selected.
Source provenance and copyright are unchanged; no new external input imported.

The wrong-layout unused ccpu386/vglob object is no longer selected. The final
ntvdm map resolves all 76 original accessors from original-softpc-cvidc:vglob.
The shared source retains its S1 SHA-256. Original GDP offset 1280 is selected,
not CCPU-local 1535. Existing initialization calls and public layouts remain.

## Changes and footprint

Seven observation-only files and unselected ntstubs restore original text.
CCPU/C-VID combined normalized mirror distance decreases 506 to 419: 87 lines,
eight files restored; 21 changed files remain. No instruction algorithm is
changed. The independent IP-mask finding remains explicitly outside this S.

The private runtime binder grows by nine source lines (125 to 134 total overlay
lines in the audited four-file cohort). Its generated conversions are retained
binding code, not hidden recovery savings. Generator/test growth is counted
separately from mirror/overlay distance. No new public interface is introduced.

The native observer now gates scripted COMMAND input on its actual visible DOS
prompt, rather than the retired diagnostic INTx marker. It does not alter guest
input or invent a product completion result. Existing exception/journals remain.

## Build and focused verification

Fresh build root: build/M0-T413/S2/formal. Configure with
New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -BuildRoot <root>, then
run-ninja-parallel.cmd run16.exe basesrv.exe ntvdm.exe
cvidc-vector-binding-fixture.exe. Shared APP_VERSION is 0.0.413, protocol 3.

The first sandboxed Ninja did not progress; it was interrupted and the normal
native toolchain run completed. The first full fixture link exposed a missing
test-only worker-phase symbol. Its asserted-unreached failure seam now exits
with ERROR_CALL_NOT_IMPLEMENTED, rather than adding product behavior. Final
formal links and the 4208-byte VDM_TIB ownership gate pass.

The fixture now calls original setup_global_data_ptr/setup_vga_globals and
links whole production owner libraries. It no longer calls/links an overlay
directly or supplies inert Video/Cpu tables. Results:

- 38 pairs times four values: zero, 0x5a, high bit, all bits; independent numeric
  offsets from original generated setters, whole 64-KiB write guards and all
  32 scratch registers pass.
- Complete 81-slot shape, retained original latch set/get, original read/write/
  mark selection, null selector slots, CPU/SAS links and repeated setup pass.
- Negative control: the original generated dirty_total setter demonstrably
  clobbers r21; the selected direct setter does not.
- 100,000 concurrent refresh calls preserve all scratch sentinels.
- Verify-T411CvidcVectorContract passes original 154 public / 55 private CPU
  metadata, early/setup binding and unavailable-slot checks.

This is focused repair evidence, not a claim that real-window stress passed.
S3 owns integrated verification/publication and retains all failure evidence.

| Built product | SHA-256 |
| --- | --- |
| run16.exe | E713F23D55112628982CB12A84F42E312C9952573765B0CEE2324A09223BB17B |
| basesrv.exe | D43E13F12065500F2FFB6D150F82E0BB6A175F6716E53F93814E5845DBCF9601 |
| ntvdm.exe | 2B4CEEF79EA1BD6AEA3E15E7D390B757A84CFC2A28207AF84853F561F1036C88 |
