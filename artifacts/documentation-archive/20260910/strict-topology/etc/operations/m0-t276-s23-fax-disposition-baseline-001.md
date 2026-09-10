# M0 T276 S23: `fax` package disposition baseline

## Observations

The 21 selected paths contain two distinct original products, not a single
general-purpose fax library:

- `wowfax` has one C body and one resource selected as `TARGETTYPE=GDI_DRIVER`
  with `NTGDIK`/`NTGDIKM` definitions. Its source says it runs in the graphics
  engine (CSRSS) context.
- `wowfaxui` is a DLL with four selected C bodies and a resource. It links
  USER, GDI, Kernel, Spooler, Shell and Advapi libraries, coordinates printer
  UI/installation, and exchanges FAXDEV data with WOW through a shared section
  and window notification.

## Boundary conclusion

The GDI driver is a private historical graphics-engine product dependency and
cannot be silently reimplemented as a user-mode printing helper. The UI DLL
uses public printing/registry APIs in places, but its original cross-process
shared mapping embeds historical 32-bit pointer/offset assumptions. A future
recovery must use session-owned resource identity, copied broker records and
fixed-width offsets; direct native `PVOID` to `DWORD` conversion is forbidden
on both x86 and x64.

The two ledgers account for every selected path and each reached interface
family. They establish a whole-package future boundary only: no driver, UI
body, printer installation or WOW callback is enabled by this audit.
