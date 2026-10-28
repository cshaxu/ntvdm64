# Part 3 MVDM: multimedia guest/host ABI carriers

Five owned keys: `WOWMMCB.INC`, `WOWMMED.INC`, `wowmmed.h`, `WOWSND.INC`, `wowsnd.h`. Observer / JSON compares current and both original source paths; retain each distinct key even for paired declarations. This is read-only, no guest build, playback or task teardown test.

| File | Specific usage and proposed disposition |
|---|---|
| `inc/WOWMMCB.INC` | Defines interrupt 72h, callback args and 16-entry callback storage; historical Win16 `mmsystem/isr.asm` and `stack.asm` include it. **Pending callback lifecycle and guest image linkage**; no asynchronous completion or overflow semantics are established. |
| `inc/WOWMMED.INC` | Guest assembly multimedia structure layouts; `mmsystem/stack.asm` includes it. **Pending per-structure ABI and actual usage**, not host build nonselection. |
| `inc/wowmmed.h` | Host C multimedia argument definitions, packed structures; selected WOW32 `precomp.h` includes it. Old `wmmedia2.c` converter bodies lie under `#if 0`; active `wmmtbl2.h` selects CallProc32 bridge instead. **Pending selected consumers/guest ABI**, do not count inactive converters as recovery. |
| `inc/WOWSND.INC` | Guest SOUND thunk ordinal/layout definitions included by `sound.asm`; historical sound makefile assembles it. **Pending source-to-guest image and function behavior**. |
| `inc/wowsnd.h` | Host C SOUND definitions included by WOW32 precomp. Original `wstbl2.h` routes DOBEEP to `WS32DoBeep` → `MessageBeep`; other entries are original UNIMPLEMENTED. **Pending per-operation behavior**, not a claim all audio is unsupported. |

Prior `shard-3/multimedia-guest-abi-role-review.md` and `sound-abi-role-review.md` contain more detailed original source/hash and observed symbol evidence. Approved manifest lists MMSYSTEM.DLL and SOUND.DRV; no source-to-guest build is established. `m0-t466-s2-sync-multimedia-bridge.md` proves a specific synchronous timer packet, not wave/MIDI device completion or callback ring teardown. Guest task ownership, caller-thread callback sequencing, failures, invalid pointers, overflow and task exit remain later S obligations. No complete runtime coverage percentage follows from header identity.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/nine-way/part-3/multimedia-abi-observer.py
rg -n -i 'wowmmcb\.inc|wowmmed\.inc|wowsnd\.inc|wowsnd\.h|wowmmed\.h' src/mvdm/wow16 src/mvdm/wow32
Get-Content src/mvdm/wow32/wstbl2.h
Get-Content docs/etc/evidence/m0-t466-s2-sync-multimedia-bridge.md
```
