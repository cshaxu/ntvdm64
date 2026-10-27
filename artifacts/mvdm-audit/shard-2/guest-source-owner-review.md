# Shard 2 guest source owner candidates

Scope: all frozen pending immutable-guest-or-guest-build-input entries with source, declaration and resource extensions listed in companion CSV. Original guest files were only read; no guest build or run, product change or ledger mutation. CSV names each input, independently checks SHA256 against frozen current hash, and provides nearby actual recipe/consumer line evidence and guest target line candidates.

Rows: 340; candidate guest build inputs: 163; unresolved: 177; hashes mismatching or missing: 0.

This is a conservative candidate inventory, not an accepted final per-file classification: exact same-directory or parent recipe name and nearby target line are not necessarily the same conditional rule, and same-stem object references do not prove source mapping. Local include/resource references likewise do not prove selected build. Historical test-only targets and target-specific resource preprocessing must be identified individually before excluding the host-input role. No inference that guest capability is excluded or present.

Known installed archive matches in guest-remaining-target-groups.csv are byte-identity evidence only; source-to-installed-binary lineage and actual loading require further proof. Inputs lacking a recipe name and target relationship remain unresolved, even if their directories suggest guest ownership. No source/resource was classified as an actual immutable runtime binary in this pass.

Follow-through: for each candidate, connect exact rule condition and target link/resource dependency; disambiguate generated build inputs and test fixtures; compare current immutable guest package identity and loader path. Only then propose excluding the file as a host build input, never guest behavior.
