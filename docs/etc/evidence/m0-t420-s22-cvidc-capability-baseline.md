# T420 S22 C-VID capability baseline

## Question

Does the selected x86 C-VID package publish one original-profile vector/state
surface, and can its original video accessors and known consumers be checked
against the current formal build rather than a stale historical build graph?

## Procedure and observations

`AuditCvidAccessorProfile.mjs` now requires an explicit current
`--build-graph`; it no longer silently reads the retired T412 graph. With
`build/M0-T420/S21/formal-x86-004/build.ninja`, the generated original
accessor fixture compiled and passed all 38 accessor pairs: generated offsets,
field values, isolated writes and scratch preservation.

The current formal C-VID binding fixture separately passed `38 pairs x 4
values, 81 slots, latches/selectors, republish, old-provider negative control,
concurrent scratch preservation`. The current product map attributes `Gdp` to
original `cvidc/ev_glue.obj`; metadata has 154 public and 55 private slots.
The CPU40 quick-event and direct effective-address providers are selected.

Nine historical short-vector consumer translation units were preprocessed with
the same current formal flags. None retained a direct `C_Video`/`Video` field
access after preprocessing. This is a consumer census, not proof that every
C-VID vector has guest execution coverage.

## Current disposition

The 26 already-recorded SPC386 non-PIG/non-SPC486 null slots remain a
source-profile question, not missing provider candidates. S22 next needs to
map every remaining selected vector/timing consumer to real guest evidence or
a source-proven profile-null/no-caller disposition. This baseline does not
close S22.
