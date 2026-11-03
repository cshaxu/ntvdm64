# S13: C-VID / CCPU selected context and registered differences

The original/current conditional tree in `ev_glue.c` was checked. Actual x86
worker and x64 reference-x86 Ninja commands preprocessed temporary source
copies with markers inside each of the four retained CCPU/C-VID groups; all
markers appeared in both outputs. The nested non-PROD arm remains error-only.
The original diff has five hunks; retained adaptations are recorded under
MVDM-HOST-DIV-076, 137 and 156, and the excluded arm under DIV-500.
[Evidence](s13-r675-evglue-partial.json) records group locations, diff hash
and preprocessing witnesses. The DIV-137 register was corrected to include
the explicitly initialized Gdp definition in this file.

No product source or runtime image changed; prior published build and Full
evidence remain unchanged. The real-command preprocessing observations do not
prove unenumerated contexts. [Counts](s13-r675-counts.json) keep deletions
separate. Remaining file and context audit continues.
