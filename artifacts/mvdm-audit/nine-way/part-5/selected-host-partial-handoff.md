# Parts 2/5/8: selected-host provider partial-classification check

The frozen ownership table has 120 MVDM keys in each part. Cross-checking `original_shard` and the four current shard review files for `selected-product-source` identifies exactly these two owned source paths in the requested parts:

| Exact path | Current evidence | Strict recommendation |
| --- | --- | --- |
| `softpc.new/base/support/get_env.c` | Captured worker Ninja compiles it into the support archive and its object has code; original PIG-guarded diagnostic is its sole source-text caller. Exact current cached `worker/ntvdm.exe.map` has no IBOOLgetenv/ISM32getenv/STRINGgetenv/ccpupig symbol. | **Pending**, not partial: no image-bound callable body or real caller proved; preserve alternate diagnostic profile. See `get-env-semantic-reconciliation.md`. |
| `softpc.new/base/video/ega_trcr.c` | Captured worker builds its archive object; object has no executable code; independently reviewed map has no mapped body. EGA_DUMP branch has file/hook lifecycle in alternate profile. | **Pending**, not partial or globally excluded. See `video-trace-fullscreen-semantic-reconciliation.md`. |

The current cached `build/.ninja/msvc-x86-release/worker/ntvdm.exe` SHA-256 is `84F72C0B33D0D0BA8E6837A25B296C17D115157FC75A4E9DC394330CC6FAC5E6`; current `assets/release/ntvdm.exe` is `EF546743393DAD7E45BB7BDA605775524ADB26B94DAE4BDEFE32951EB0510294`. The map has not been bound against both images, so absence in the cache map is only an exact-map observation. It cannot certify release absence, alternate configuration absence or runtime capability exclusion. This result deliberately yields **zero defensible new partial rows**, not an inflated restoration count. Original selected providers analyzed in earlier reviews but owned by other logical parts are cross-context evidence, not reclassification candidates for this part.

The prior read-only DPMI audit sought prebuilt D38M.COM in repo build/assets/tests and installed `O:/winnt/tests`; on follow-up, `rg --files --no-ignore O:/assets` found no loose matching probe or s38-memory-info artifact. ISO/7z/container members were not scanned. T420 specifies the now-unavailable build path and O:/winnt logs, not a proven O:/assets archive. No probe was rebuilt or runtime launched. Existing DOS/host tests are only historical candidate evidence unless their matching binary/package is found and independently isolated.

Replay the ownership/current-shard filters, `rg -n -i "IBOOLgetenv|ISM32getenv|STRINGgetenv|ccpupig|ega_trcr" build/.ninja/msvc-x86-release/worker/ntvdm.exe.map`, and SHA-256 the two named worker images. Run part-5 host-input observer read-only. No guest, product, build, process or shared ledger changes.
