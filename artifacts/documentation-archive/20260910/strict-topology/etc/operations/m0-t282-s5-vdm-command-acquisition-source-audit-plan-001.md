# M0 T282 S5 — VDM command-acquisition source audit

The statically composed original COMMAND package calls the historical public
`GetNextVDMCommand(PVDMINFO)` interface. Its original implementation spans
the client `base/win32/client/vdm.c` and the BaseSrv/CSR server path in
`base/win32/server/srvvdm.c`; it cannot be assumed to be a direct modern Win32
call or a COMMAND-private helper.

Audit every reached client/server branch, message/record layout, blocking and
failure behavior, and each private transport dependency. Reconcile these with
the declared one-session session and adapter-vdm-monitor owners. Deliver a
source-first disposition: direct original composition, smallest same-shaped
binding, explicitly unavailable historical product-shell branch, or a named
later broker owner. Do not implement any callable behavior in this audit.
