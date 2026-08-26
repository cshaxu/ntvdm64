# M0 T276 S23: W8 `fax` package disposition plan

## Objective

Audit all 21 selected `fax` paths as one original owner package. Preserve the
original distinction between the `wowfax` GDI driver and `wowfaxui` UI DLL;
neither is folded into the general WOW32 package simply because WOW32 calls
some of its routines.

## Method

1. Reproduce both original `sources` selections and all build/resource forms.
2. Classify each path as driver, UI, shared declaration, product resource or
   build control.
3. Record source-proven GDI/printer, WOW32 structure/callback, Win32/registry,
   monitor and session mapping boundaries.
4. Keep GDI driver/private printing dependencies explicit; do not substitute a
   public printer API without a source-level contract comparison.

## Mandatory mapping rule

Any WOW16 data referenced by `wowfax` or `wowfaxui` uses a session-owned
guest-memory mapping-manager lease. Native printer/GDI endpoint identity uses
the session host-resource mapping manager; native pointers and handles cannot
enter original 16/32 layout fields.

## Non-goals

No driver/DLL source body, new printer service, build edge, Ninja run or guest
execution is enabled by this audit.

## Exit evidence

Every selected path has an exact provisional owner/disposition and every
reached external family is assigned to `adapter-wow`, `adapter-win32`,
`adapter-softpc`/`adapter-vdm-monitor` or `session` without inventing a fax
provider.
