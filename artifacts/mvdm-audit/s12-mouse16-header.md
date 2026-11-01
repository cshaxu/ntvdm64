# S12: unreferenced mouse16 header

After MVDM-HOST-DIV-532 removed the unselected legacy mouse16 call sites,
project code, tests, tools and build declarations have no remaining
reference to `mouse16b.h` or `mouse16bInstall`; the remaining references are
historical archive evidence and this audit. Current worker and WOW32 Ninja
dependency logs do not list the header. The separate mouse31 driver uses the
selected INT 33 interface described in `s12-mouse16-legacy.md`.

The original header matched OpenNT byte-for-byte. Its unused declarations and
data structure are replaced by a numbered marker while its original include
guard and provenance comments remain. The companion JSON records original file
and removed-body hashes. Independent reconstruction verified the entire
resulting file. The cached NTVDM and VDMREDIR targets reported no work, and
documentation governance passed. No runtime image changed, so no runtime
test or publication was repeated. Historical archives were preserved.
