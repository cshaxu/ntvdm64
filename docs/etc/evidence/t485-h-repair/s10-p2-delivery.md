# S10 P2 delivery verification

Nine complete file reviews: evidgen.h, sevid000.c, sevid001.c, sevid004.c,
sevid016.c, sevid017.c, sevid018.c, sevid019.c and gvi.h. Seven changed files;
sevid018/019 unchanged. Original LF recovered, four trailing-space differences
restored and gvi.h XP491 line mapping corrected. No executable expression,
#error exclusion, original guest or winnt.zip modification.

Batch non-error census185/101302 ->267/62146: +82 exposed blocks/-39156
lines. This is original-format recovery, not82 new functional changes.
S10 baseline201/102863 ->267/62146: +66/-40717. All30 census hashes checked.
Source-reviewed10/30 files,131/1060 points;929 remain. Four whole frozen
points restored and final-verified;127 evidenced residuals remain pending
owner decisions. Retention is not newly approved by verification.

Owned build/publication PID15628 completed. Cached full build passed;
runtime2 staged13 images and published to assets/release and O:/WINNT/SYSTEM32.
All26 destination SHA256 identities were rechecked after tests. Manifest is
build/t485-s10/runtime2/product-system32-manifest.json.

Owned targeted runner PID2192 completed:
- cvidc.exit=0; actual production-linked C-VID setup/vector/selection, read/write,
  CHAIN2 planes, strings, SAS moves/fills, dirty spans and scratch cases passed.
- debug-live/result.txt confirms live MEM exit0 and NTVDM VDD exports,
  allocation failure preservation, debugger writeback/register round trip and
  matching task-termination witness.
- dos4gw-result.txt and rendered before/hidden/restored PNGs confirm actual
  NESticle menu and Escape interaction. No ROM gameplay claim.

These results are under build/t485-s10/p2. Full is reserved for S13.
Historical generated block-copy limitation remains explicitly distinguished
in the sevid019 review; current SAS tests do not certify that old loop.
Documentation governance and relative links passed before final receipt;
they are rerun after this receipt. Commit/push pending.
