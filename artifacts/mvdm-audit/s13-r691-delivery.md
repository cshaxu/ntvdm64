# S13: witnessed source condition mappings, batch r691

Ten source files received original diagnostic and continuation line mappings. Nine complete witnessed conditional-tree reviews are recorded in [batch review](s13-r691-batch-review.json); DPMI data.c retains its local CPU40 adapter and only its DBG arm was reviewed in [the scoped review](s13-r690-dpmi-data-review.json), so the file remains pending for the whole-file audit. Supporting per-file anchor records are r683-r688 and r691. Preliminary held results are r686 and r688; they are not completed reviews.

Instrumented copies initially changed include resolution for DPMI data.c and WOW32 wdib.c. Those results were rejected and replaced with temporary sibling probes using the unmodified compiler search order. Their ordered include traces matched the originals in each observed context. Other selected-arm evidence is limited to the recorded worker graphs; wdib has the observed WOW32 x86 graph. Other supported contexts remain to be proved.

Cached x86, x86-extra, and x64 builds passed. The [ten-image comparison](s13-r691-build-image-compare.json) showed six byte-identical images and four with only PE timestamp changes. Per owner instruction, full tests were not repeated; the previously delivered Full evidence is [r682c](s13-r682c-full-evidence.json). The exact candidate was published and hash-verified in assets/release and O:/winnt/system32. Guest images were not modified or rebuilt.

[Counts](s13-r691-counts.json): 1058 currently present C/H files in the two roots, 165 reviewed identical, 659 reviewed different and 234 pending. The 578 net deleted C/H files relative to c97d4ee2c are separate. These filesystem counts do not prove all files are mirror files or included in every supported build.
