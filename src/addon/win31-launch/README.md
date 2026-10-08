# Win31 installed-tree launch add-on

This directory owns independently authored native helpers for
`tools/win31-launch`.  It is not guest media, a host runtime dependency, or a
general patcher.  The only admitted binary changes are the two identity-bound,
recoverable Windows 3.1 candidates recorded in the source policy.

The native PIF writer must construct its fixed and extension records from the
published OpenNT PIF ABI definitions; it must not copy a historical PIF or
retain a former installation path as a template.
