# M0 T408 S6 — Redirector OEM-copy disposition

## Result

No replacement is required: the selected x86 graph already compiles the
complete original `opennt-host/netapi/netlib/copystr.c` together with its
original `allocstr.c` and `initoem.c` support.  `vrnetapi.c` continues to call
the original `NetpCopyWStrToStr` for host-local bounded result buffers.

The only related local body, `mvdm_redirector_copy_wide_to_guest`, is not a
second provider.  Current public `NetWkstaUserGetInfo` returns Unicode, while
the original non-UNICODE source expected a directly dereferenceable VDM
pointer.  The standalone `ES:DI` address has no durable flat alias; it must
be OEM-converted to a bounded temporary and committed by a guest-memory lease.
The three reached call sites are the CD-name composite write, computer name,
and user name publication.  Deleting this seam would restore an unsafe raw
guest pointer, not original NetLib behavior.

## Disposition

- Original NetLib conversion owner: retained unchanged and selected.
- Adapter OEM conversion: retained only for Unicode-to-guest publication.
- No duplicate original algorithm was found; no source change is justified.

