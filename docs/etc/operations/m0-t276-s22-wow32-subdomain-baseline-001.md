# M0 T276 S22: `wow32` active-subdomain baseline

## Question

What original provider subdomains and historical boundary families occur in the
selected WOW32 DLL, without treating a build selection as permission to enable
one of its bodies?

## Inputs and method

`wow32/sources` was parsed condition-aware. Its 77 active C bodies are then
classified from their original source headers, includes and declared provider
roles. The reproducible generator rejects an omitted, duplicated or inactive
source. The corresponding ledger has one row for every active C body; the
separate file-disposition ledger continues to account for every one of the 201
selected paths.

## Observations

- The active DLL splits into eight original subdomains: thunk/frame/structure;
  message/window procedure; GDI/window object; kernel/task/DOS session; USER;
  shell/OLE/printing; multimedia; and network.
- `wcall16.c`, `wcall32.c`, `wstruc.c`, the message thunks, `wkmem.c` and the
  resource/structure conversion paths show that historical 16/32 crossing is
  a package-wide concern, not a helper to be reauthored inside one provider.
- `walias.c`, `wmmalias.c` and the handle conversion macros prove that native
  resource identities cannot be preserved by a 32-bit cast on x64.
- `gdi32p`, `user32p`, VDD headers and `ntvdm.lib` in the original build are
  historical product/private boundaries. Their appearance is evidence for the
  named adapter owners, not evidence that a public modern symbol is a drop-in
  replacement.

## Required recovery boundaries

- `adapter-wow` owns the package-facing WOW contract and any eventual
  same-shaped composition boundary.
- `adapter-softpc` and `adapter-vdm-monitor` own historical machine/frame
  operations, then delegate mechanically to the Bochs side.
- `adapter-win32` owns only unavailable historical Windows/NT interface
  facades; public APIs can remain direct when their original contract matches.
- `session` owns the guest-memory lease and host-resource mapping-manager
  instances. A historical guest address remains numeric guest data; native
  pointer or HANDLE identity never crosses an imported WOW interface.

## Interpretation

WOW32 cannot be recovered safely by admitting isolated thunk files in trace
order. The unit of future recovery is the original DLL package with its eight
subdomains and explicit historical interfaces. This record establishes only
the package boundary; it neither enables a source body nor asserts a modern
implementation exists for a private USER/GDI or product-shell dependency.
