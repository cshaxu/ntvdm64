# S10 shard 3: Win16 sound version resource inputs

Scope: pending `wow16/drivers/sound/sound.rcv` and `SOUND.RES`. Read-only audit, no guest mutation/build or sound playback. Both match the OpenNT and OpenNT 4.5 inventory hashes. This covers resource provenance and use, not implementation of sound behavior.

| File | Role and recommendation |
|---|---|
| `sound.rcv` | SHA-256 `F0ECA8CB6BA34DA40995D8529EBDA0D133F278B78D8BFD9126C0F03F3782FAC2`. `sound.rc` includes it, and it provides VERSION resource attributes declaring a sound driver and `SOUND.DRV`, then includes `common.ver`. Historical makefile:91 builds `sound.res` from the RC/RCV/version inputs. **Immutable historical guest build input; don't classify its absence from host Ninja as missing guest sound.** |
| `SOUND.RES` | SHA-256 `D73CFE9DF6A7716D4A6469C18DFB65EA2D53178F68F85038F91A3CC4A3AD5EAD`. Historical makefile:103-106 consumes `$*.res` via `rc16 -t` after linking and renames the output to `sound.drv`. **Retained historical resource artifact; exact association to the currently deployed guest binary is not proven.** |

The approved guest manifest lists `WINNT/SYSTEM32/SOUND.DRV` as a 1,744-byte NE file with SHA-256 `d5932cbd4e0155a07d4d0f64c0f8afcdc3aefe90e06e7265c6651b10c2f03801`, from the preserved approved baseline; this does not prove this specific resource was compiled into it. The separate `sound-abi-role-review.md` traces thunks and identifies the limitations of their host dispatch. No sound runtime, ordinal execution, resource extraction, or playback was tested here. Runtime coverage and source-to-guest build identity remain pending.

Replay:

```powershell
Get-FileHash src/mvdm/wow16/drivers/sound/sound.rcv,src/mvdm/wow16/drivers/sound/SOUND.RES -Algorithm SHA256
rg -n -i 'sound\.rcv|sound\.res|sound\.drv|rc16' src/mvdm/wow16/drivers/sound
rg -n -i 'sound\.drv' assets/winnt-guest-manifest.json
rg -n 'wow16/drivers/sound/(SOUND.RES|sound.rcv)' artifacts/mvdm-audit/shard-3/file-review.csv
```
