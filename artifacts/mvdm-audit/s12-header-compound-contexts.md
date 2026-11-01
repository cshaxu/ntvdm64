# S12: compound conditions in observed header consumers

The previous per-flag inventories had different file and consumer sets. This
check instead joins the current Ninja compiler commands to the valid dependency
entries by object for the x86 worker and WOW32 default product graphs. It
records each observed header's distinct six-flag profiles and edge counts in
the companion CSV. The join found 434 existing MVDM headers, 5,030 worker and
8,571 WOW32 header edges. No project header edge in those joined commands was
missing on disk. The two worker objects and two WOW32 resources without valid
dependency records are assembly or resource inputs, respectively, rather than
C compiler units; they are outside this C/header scan.

The worker dependency log also contained 201 objects outside the matching
default command closure. Some remain listed as available targets, so they are
not described as deleted or stale; they were not used as evidence of current
product consumers. Other build profiles and explicitly invoked non-default
targets are not covered by this inventory.

The conservative compound-expression scan accepts only `defined`, the six
recorded flags, constants 0/1 and boolean operators. It requires a fixed
outcome for every joined consumer, an original-source body match, and absence
of an existing divergence marker. It found no new deletable body: its sole
candidate had already been registered in MVDM-HOST-DIV-518. Unsupported
expressions and other flags require their own evidence. No source or runtime
image changed in this check.
