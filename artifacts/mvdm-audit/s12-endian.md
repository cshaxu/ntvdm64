# S12: unselected endian and bit-order branches

The observed MVDM product commands do not define `BIGEND` or `BIT_ORDER1`;
no corresponding definitions or undefinitions were found in project or
generated C/header sources. The host configuration selects `LITTLEND` and
`BIT_ORDER2`. Fifty-nine outermost positive branches matching the original
source were replaced by markers while retaining their conditionals; one
nested candidate was subsumed. Locations and removed-body hashes are recorded
in the companion JSONL. Other unobserved configurations are outside this
evidence.

Independent reconstruction from `af8c4b529` verified all 59 body hashes and
all 27 complete files. Cached x86/x64 product builds and documentation
governance passed. All ten product images matched the release exactly or
differed only by four PE linker timestamp bytes; runtime tests and publication
were not repeated for equivalent images.
