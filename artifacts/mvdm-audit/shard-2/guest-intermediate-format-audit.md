# Shard 2 immutable guest intermediate-format audit

The companion CSV independently checks all 179 previously role-excluded OBJ/MAP/SYM/LNK files, with exact frozen/live SHA-256, byte length, format markers and original same-family recipe candidates. The checks support only historical build/debug file roles. No related guest executable or algorithm is excluded by the format result.

All 156 OBJ files parse completely as length-framed OMF records from THEADR/LHEADR (80/82) to MODEND (8A/8B), 5,252 records total. Five files have a total 236 records with nonzero byte-sum modulo 256; the counts are recorded per file. This is not a finding of corruption: legacy OMF producers may leave checksum disabled, and this review does not establish checksum policy. Framing is not proof of original source linkage or binary behavior.

All five MAP files contain readable linker start/length/name tables. Eight LNK files contain textual object/output response directives. Ten SYM files have a length-prefixed readable module signature consistent with symbol-table artifacts; this is not a full SYM parser. Same-family original makefile/sources/link candidate paths are recorded, with absence explicitly indicated. Original recipes demonstrate output conventions for DOSX, KERNEL, USER, TIMER and other families; they do not prove any individual output was produced by a particular source revision.

Executable/source identity and guest capability remain independent pending work. RES and LIB were not reclassified; no guest was changed, rebuilt or run.
