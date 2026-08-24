# M0 T263 S9 three-file original-body recovery ledger

## Question

Can the three previously source-derived `opennt-host` files be replaced by
reviewable, cropped OpenNT original bodies without exposing an incomplete NT4
product success path?

## Inputs

- `O:\repos.external\OpenNT\base\mvdm\softpc.new\host\src\nt_error.c`
- `O:\repos.external\OpenNT\base\mvdm\vdmredir\vrinit.c`
- `O:\repos.external\OpenNT\base\mvdm\vdmredir\vrnmpipe.c`
- current direct callers under `src/opennt-bop/dem/` and the top-level BOP 59
  provider.

## Retained original blocks

- `nt_error.c`: original `TlsDirectError` and `host_direct_access_error`,
  source lines 207--253.  Its reached callers are DEM direct-access reports and
  BOP 59.  The rest of the module is unrelated general SoftPC, WOW and CSR
  dialog product code and is not admitted.
- `vrinit.c`: original `IsVrInitialized` and `VrInitialized`, source lines
  86 and 127--155.  DEM's original DLL-import call shape requires only this
  query provider.
- `vrnmpipe.c`: source-order synchronous interceptors and their original list
  helpers: lines 2343--3231, plus the required original local type/prototype
  fragments at lines 141--162 and 214--231.  Reached callers are the DEM
  open/close/read/write and local named-pipe-name paths.

## Deleted unadmitted blocks

- `nt_error.c` general host-error, editable dialog, WOW, CSRSS hard-error and
  debug-only bodies require their own unavailable product dependencies.
- `vrinit.c` `VrInitialize`, `VrUninitialize`, VDD hooks, NetBIOS, DLC, ICA
  interrupt queue and suspend/resume blocks are deleted together.  In
  particular there is no replacement `VrInitialize` success gate.
- `vrnmpipe.c` selector-facing pipe-information calls, network calls,
  asynchronous named-pipe thread/queue/cancellation and diagnostics are
  deleted with their dedicated data and prototypes.  Their service admission
  remains Redirector-owner work.

## Local divergences and owners

- `HOST-DIV-012..014` map the reached historical TLS/resource/dialog calls in
  `host_direct_access_error` to the existing same-shaped `adapter-win32`
  facade.
- `HOST-DIV-015..016` omit the original VDD cancellation-list and raw-handle
  retirement operations from the otherwise original synchronous pipe ordering.
  Cancellation remains unadmitted; native handle retirement remains session
  ownership.
- `HOST-DIV-017` changes only the static provider's emitted `VrInitialized`
  symbol, preserving the original BOP-side function-pointer import shape.
- `HOST-DIV-020`, `023..026` are statement-local x64/warning-clean or static
  provider-header differences.  No `adapter-bop` route logic is added to any
  host mirror.  Guest register/span and DOS-handle translation stay at the
  existing `opennt-bop` to `adapter-softpc` boundary.
- `BOP-DIV-088` changes the former `57:00/57:01` composition consumer to the
  existing invalid-function terminal. It prevents the removed host functions
  from being replaced by a false loaded-session state.

## Observation

The prior files did contain equivalent but reordered/reduced local algorithms.
The current replacement begins from the cited original source spans and retains
their function order, data structures, comments and failure ordering for each
admitted block.  The `vrinit.c` fabricated initialization success has been
removed.

## Verification state

Static caller and unavailable-product scans pass: no retained occurrence of
VDD hook installation, NetBIOS/DLC initialization, ICA delivery, async named
pipe queue, or NT hard-error product call remains in the three mirrors.

The fresh `build/M0-T263-S9/r001` graph generated and completed. A final
incremental invocation reported `ninja: no work to do`. These focused formal
fixture binaries all exited `0`:

- `t263-s8-opennt-host-mirror-fixture.exe` — original utility and synchronous
  pipe paths;
- `t237-s5-bop-nosupport-fixture.exe` — the retained BOP 59 direct-access
  route;
- `t230-s3-demfile-direct-import-fixture.exe` — DEM file paths and explicit
  `LoadVdmRedir` non-admission; and
- `t251-s3-redir-ingress-fixture.exe` — explicit `57:00/57:01` failure plus
  the independent synchronous pipe helper paths.

The earlier source-derived 57:0B..0F/23..26 test branch is no longer run under
the fabricated `57:00` loaded state. It remains Redirector-owner work, not
evidence for this host mirror closure.

## Follow-up

After the build root is available, compile the focused host mirror fixture and
the affected DEM/BOP 59 fixtures; then review every changed line against these
source spans and update the document inventory before S9 closure.
