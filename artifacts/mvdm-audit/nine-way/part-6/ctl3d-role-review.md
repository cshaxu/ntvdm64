# Part 6: CTL3D historical inputs versus deployed guest DLL

The 13 owned `wow16/ctl3dv2` keys include two definition files, resource script, dialog template, bitmaps/icons, load and version headers, documentation and `.dif` files. Their individual original/current source identities are in `mvdm-identity-observations.json`. The `.def` files describe different library names (`Ctl3d` and `Ctl3dV2`) with related subclass/control exports; `ctl3d.rc:65` explicitly includes `3dcheck.bmp` and defines variant version names. Their presence is not evidence that each variant was selected for the deployed DLL.

The upstream `ctl3dv2/readme.txt` states that this source was kept for debugging and the special WOW binary was binplaced separately with `SPECIAL_WOW_VERSION`. The currently inspected `assets/winnt.zip` contains `WINNT/SYSTEM32/CTL3DV2.DLL`; this is an observable packaged guest capability, not proof that the stored DLL came from any of these source inputs. `ctls.dlg`, two `.dif` files and separate icons require their precise source recipe/variant before claims of use. Do not dismiss all items as irrelevant just because the host build does not build them.

No DLL was loaded, inspected for exports, modified, rebuilt, or run in this review; runtime control subclasses and cleanup remain unverified. Preserve source/resource roles and keep per-item deployment/provenance pending.
