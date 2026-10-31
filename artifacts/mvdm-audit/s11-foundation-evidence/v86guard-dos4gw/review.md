# V86guard NTVDM: DOS4GW graphical progress control

The previously used isolated NESticle/DOS4GW observer was rerun with the
current V86guard NTVDM input and the same earlier diagnostic NTCON used by
the successful prior control. The NTVDM input hash matches the current
release package's pre-sealing input; the two unmodified application hashes
match the previously recorded originals. The instrumented frontend recorded
78 successful 640x400 graphical commits during the 15-second observation.
The observer reported timeout and the runner returned failure because the
interactive program did not exit; this is graphical progress, not normal
exit or gameplay acceptance. The earlier comparable diagnostic observation
recorded 77 commits, but frame counts alone do not establish performance
equivalence.

This is an isolated diagnostic-front-end package, not a test of the released
NTCON or a full release-package regression. The owned Z: mapping was removed;
no guest binary was modified or rebuilt. `result.json`, the frame trace, the
observer report and the test package input hashes retain the evidence.
