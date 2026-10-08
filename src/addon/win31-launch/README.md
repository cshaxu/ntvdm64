# Win31 installed-tree launch add-on

This directory owns the independently authored `PATCH386.EXE` helper used by
`tools/win31-launch`.  It is not guest media, a host runtime dependency, or a
general patcher.  It transforms only the two identity-bound, recoverable
Windows 3.1 candidates recorded in the source policy.  It does not own backup,
mouse, PIF, configuration, or launcher policy; those file operations remain
plain CMD work in `tools/win31-launch/APPLY.CMD`.
