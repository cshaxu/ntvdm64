# Win16 guest library and SOUND import ownership

Three shard-2 pending files match both original editions. The historical `wow16/lib/makefile` uses `implib sound.lib sound.def`, then combines sound.lib with other import libraries into libw.lib. Its sound.def is byte-identical to `drivers/sound/sound.def`. The published ZIP SOUND.DRV is byte-identical to the source-tree and installed System32 copies. Parsing the shipped NE names confirms all 18 explicit sound.def names and ordinals, plus WEP ordinal 19. This verifies the exposed interface metadata and resource identity, not that sound.lib was actually linked into any particular shipped guest or that a call produces audio.

| Entry | Evidence and bounded recommendation |
| --- | --- |
| `wow16/lib/sound.def` | Exact original import definition, original implib/lib16 recipe, and published NE name/ordinal correspondence. Candidate **historical guest import-library build metadata**; retain SOUND behavior, callbacks and lifecycle as separate obligations. |
| `wow16/lib/cdllcew.lib` | Both-original identity; earlier `guest-library-role-review.csv` verifies OMF record framing but no precise selected original consumer or provenance. **Pending**. |
| `wow16/lib/snocrtdw.lib` | Both-original identity and OMF framing, but no precise original consumer or provenance. **Pending**. |

NE export metadata does not prove driver loading, 16-bit thunk execution, host audio availability, event sequencing or cleanup. Nor does a generic modern sound service prove this guest API is restored. Do not infer a full WinMine sound result from these static checks. No guest or product was run or rebuilt; no shared ledger or build tree changed.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/sound-import-family-observer.py` and compare JSON; see earlier guest-library-role-review for the OMF framing boundaries.
