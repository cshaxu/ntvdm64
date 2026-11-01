# S12: unselected HUNTER branches

The selected MVDM product command graphs do not define `HUNTER`; neither
the project source nor the recorded compiler predefined macros define it.
Thirty-seven outermost positive HUNTER branches in the observed compiled
sources and header dependency closure matched the OpenNT source. Their bodies
were removed, and the original preprocessor shells retained. The JSONL records
the affected paths, baseline line ranges and SHA-256 hashes. This does not
claim that every unobserved historical profile has been assessed.

Independent reconstruction from baseline `47ddecabe` verified every removed
body hash and all 18 resulting source files. Cached x86/x64 product builds and
documentation governance passed. Each of the ten product images matched the
published image exactly or differed only in the four linker timestamp bytes;
runtime tests and republishing were not repeated for equivalent images.
