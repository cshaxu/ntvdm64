# S12: unselected DBG branches

The eight observed product build graphs do not define `DBG` on compiler command
lines. The observed dependency closure defines it only as zero. This audit
therefore covers numeric `#if DBG` branches, not `#ifdef DBG` branches. Thirty-one
stale dependency paths were excluded from the evidence. Guest images were not
changed.

Against baseline `2338f3ced`, the JSONL inventory records 345 branches whose
removed bodies match OpenNT. The companion JSON records two previously adapted
debug-print branches reviewed separately. Their inactive adaptations are no
longer needed; active adaptations remain intact. The two project mirror
registers record the corresponding exceptions.

An independent reconstruction checked all 347 removed body hashes and reproduced
the complete contents of all 64 changed source files. Cached x86 and x64 product
builds completed; VDMREDIR was rebuilt after the final two edits. All ten release
components matched the prior release exactly or differed only in four PE linker
timestamp bytes. The authored SoftPC profile check passed 428 checks, and the
DOS/Video7 source-contract check passed. No runtime re-release was warranted for
the resulting equivalent images. These observations cover the recorded product
graphs, not unobserved build profiles.
