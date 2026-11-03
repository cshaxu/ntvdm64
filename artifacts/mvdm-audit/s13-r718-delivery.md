# S13: selected-tree and excluded BIOS branch audit, batch r718

Ten source files changed: two WOW32 diagnostic mappings, FPU, idle detection,
quick-event and EOI mappings, the BOP and XMS mappings, and two explicitly
excluded BIOS branches. Each original branch keeps its condition, #error and
original diagnostic line mapping. The selected fallback bodies remain intact.
The EMM min fallback and BIOS WDCTRL_BOP reviews are partial; their whole files
remain pending. Their observed worker graphs and the identical same-directory
include traces are recorded in r717 and r718 respectively.

Audited local adaptations remain in place. DIV-770 and DIV-771 register the
existing opt-in WOW and XMS witnesses; enabled tracing performs file I/O and
may change timing. The quick-event original comments were restored. Additional
per-file reviews, existing source hashes and their observed contexts are in the
r692-r713 evidence. All current release-tree Ninja graphs were enumerated in
r699; headers and configurations outside those graphs remain unproven.

Cached x86, x86-extra and x64 builds passed. The [ten-image comparison](s13-r718-build-image-compare.json) shows seven byte-identical images and
three with only PE timestamp differences. Under owner direction no repeat of
Full was performed; the previously delivered Full evidence is r682c. All ten
candidate images were published and hash-verified in assets/release and
O:/winnt/system32. Guest images were neither modified nor rebuilt.

[Counts](s13-r718-counts.json): 1058 existing C/H files in the two roots,
168 reviewed identical, 677 reviewed different, and 213 pending. Relative to
c97d4ee2c, 578 net deleted C/H files are tracked separately. The filesystem
scope is not proof that all entries are mirror files or included in all builds.
