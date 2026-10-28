# WOW GDI core owner

`wow32/wgdi.c` is byte-identical to both original editions. The
[observer](wow-gdi-core-observer.py) and [receipt](wow-gdi-core-observations.json)
resolve 119 initial GDI ordinal slots, their table relocations and the selected
original object function bodies in released WOW32. Some functions occupy more
than one ordinal; these are 119 slots, not 119 independent implementations or
behavior tests. The same underlying observer was replayed on its earlier GDI
and palette cohorts after adding this owner.

The original thunks own guest argument conversion, handle translation and
callback/copyback preparation. Modern Windows owns actual DCs, GDI objects,
device drawing and host APIs. The selected file includes, among other things,
bitmaps, regions, DCs, coordinate conversion, GDI escape, DIB and enumeration
callbacks. Source/image correspondence does not show actual guest dispatch or
prove that host and Win16 behavior agree on device capabilities, signed
coordinates, region/bitmap limits, escape payloads, callbacks, object identity,
errors or teardown. Those remain S16 obligations; original source retention is
100%, while behavioral-restoration percentage is not established. No guest or
product image changed.
