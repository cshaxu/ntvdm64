# S12: legacy display alternatives and adapted BACK_M branches

Under the recorded product conditions in `s12-legacy-devices.md`, seven
original REAL_VGA alternatives are unselected. Three additional BACK_M bodies
in `nt_emm.c` were introduced by project commit `eae9f28c2e` for reverse
address adjustment in the alias-aware load, store and move adaptations.
The flag is absent in the observed product contexts; only those three
adjustments were removed. The live alias checks, memory operations, error
handling and their existing divergence records remain. The companion JSONL
identifies original versus project-adapted branches and records body hashes.

Independent reconstruction from `4d334ddd8` verified all ten body hashes and
three complete files. Cached x86/x64 product builds and documentation
governance passed. All ten product images matched the previous release exactly
or differed only by four PE linker timestamp bytes. Runtime tests and
publication were not repeated for equivalent images. Unobserved profiles are
not covered by this result.
