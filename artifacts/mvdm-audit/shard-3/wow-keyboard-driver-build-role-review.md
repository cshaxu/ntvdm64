# S10 shard 3: Win16 keyboard driver build definition and resources

Read-only role review of pending `wow16/drivers/keyboard/kbd.def`, `keyboard.rc`, `KEYBOARD.RES`; no build, guest modification or input test. Earlier `parallel-review-current.md` already proved retained `keyboard.drv` equals the approved archive and installed driver by full hash; this does not prove source generated that binary or keyboard behavior.

* `kbd.def` SHA-256 `82AB6A0F693E941BB4ED4D5A48ABD781B9B2C96E8FB6D34D130CD86415D14170`: historical enhanced-keyboard definition. The observed WOW driver makefile derives `$*.def` from target `keyboard.drv`, thus uses `keyboard.def`, not `kbd.def`; the separate WOW import-lib makefile explicitly copies `drivers/keyboard/keyboard.def`. `kbd.def` includes different segments/import `KERNEL.205` and enhanced-driver description. The 20 explicitly numbered names and ordinals happen to match `keyboard.def`, but do not imply the two implementation paths or segment/lifetime behavior are equivalent. Proposed **alternate historical definition role**, not an exclusion of keyboard APIs; other historical build configurations remain unexamined.
* `keyboard.rc` SHA-256 `85AE02330E6FA0D472A448E3186D75B3AAA11CDFC68E71916FDAD9CABCF65DF0`: contains `#include "keyboard.rcv"`; the latter declares WOW keyboard driver version metadata and includes `common.ver`. Historical makefile builds `keyboard.res` from these.
* `KEYBOARD.RES` SHA-256 `ED251DA372C5D5576B58D7A9CC9E65723BA8061AAD7860B0F33ADD0852F48743`: historical `.res` artifact, consumed by `rc16 -t $*.res $*.exe` in that WOW makefile after linking. Proposed **guest build/resource carrier**, not a host executable input. The recipe does not prove those source/resource bytes were used to construct the archived driver.

`keyboard.drv` retained and `O:/winnt/system32/keyboard.drv` both hash `C34D7D8D71453DD7567C6DB28958A0D4D11E4B198CFC436F4D430D93A5A3584C`; the approved manifest records that identity. Earlier `keyboard-abi-role-review.md` and `wow-keyboard-thunks-lifecycle-review.md` trace guest thunks and selected host providers, with layout, dead-key, callback and buffer semantics still pending. No runtime coverage percentage follows.

Replay:

```powershell
Get-FileHash src/mvdm/wow16/drivers/keyboard/kbd.def,src/mvdm/wow16/drivers/keyboard/keyboard.rc,src/mvdm/wow16/drivers/keyboard/KEYBOARD.RES,src/mvdm/wow16/drivers/keyboard/keyboard.drv,O:/winnt/system32/keyboard.drv -Algorithm SHA256
Get-Content src/mvdm/wow16/drivers/keyboard/makefile
rg -n 'keyboard\.def|kbd\.def' src/mvdm/wow16 -g 'makefile'
```
