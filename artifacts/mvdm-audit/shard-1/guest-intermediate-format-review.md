# Immutable guest intermediate format and role review

Frozen source universe SHA256 F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196. `guest-intermediate-format-review.csv` gives every requested path, actual/frozen hash, format evidence, original recipe references, current host Ninja target and package-c matches, archive matches and decision. Current `assets/winnt.zip` SHA256 7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7 matches the retained guest manifest. No checked intermediate matches a whole archive or package file.

All 147 OBJ files parse as complete OMF record sequences with THEADR/LHEADR first and MODEND last. Framing is checked by each record's type and little-endian length to exact EOF; no record with a nonzero checksum byte failed its checksum. 166 records have a zero checksum byte, which is recorded separately and not declared invalid. Framing does not prove linker compatibility or code semantics.

Eight MAP files and one LST are text-form linker/assembler listings. Ten binary SYM files have direct original makefile mentions and are treated as debugging-symbol outputs, not executable guest images. One `wow16/regedit/debug/regedt16.sym` lacks an exact original recipe reference and remains pending; do not claim binary zero bytes alone establish its format.

The 166 verified files are excluded only as retained guest intermediate files from the checked current host/runtime package; the guest source, algorithms, debugger capability and alternate build uses are not excluded. Fifteen RES/LIB entries remain pending and were not subjected to this exclusion. Original guest media was neither modified nor compiled, and no guest ran.
