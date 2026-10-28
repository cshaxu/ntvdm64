# Original WOW internal-table declaration carrier

`wow32/witbl.h` declares `aw32Internal` and, under `DEBUG_OR_WOWPROFILE`, `iInternalMax`; it contains no implementation or table entries. Its current bytes match both frozen OpenNT editions. Case-insensitive searches of current MVDM/tools and both available original MVDM trees find the header name and both identifiers only in the header itself. The previously captured selected dependency census has no exact edge to it.

The checked original and current selected `wow32/wowtbl.c` instead define `aw32WOW` and bind `pawThunkTable` to that combined table. This identifies the selected table owner, but does not prove every internal, USER, GDI or other guest API entry or any debug/profile behavior.

Exclude this exact unused historical declaration carrier from the observed NTVDM host source scope. Preserve the original file; S14 retains table ordering, dispatch, applicable internal APIs, failures and task lifecycle. External, generated or unbuilt consumers are not disproved, and a supported profile finding one must reopen this disposition. No runtime or guest test was performed.
