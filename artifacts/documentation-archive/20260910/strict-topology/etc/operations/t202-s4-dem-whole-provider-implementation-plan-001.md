# T202 S4 DEM whole-provider implementation plan

## Purpose

This is the implementation plan for the already-admitted 29-identity DEM
file/handle/FCB/search package.  It turns the S3 source/ABI map into one
provider composition; it is not a trace-derived service backlog and does not
admit an individual endpoint patch.

## Source and boundary decision

The controlling historical sources remain `demfile.c`, `demhndl.c`,
`demfcb.c`, `demsrch.c`, `demdir.c`, `dosdef.h`, and `demerror.c` under
`src/opennt`.  `src/bx-vdm` rehosts only the host-side composition contract:
it owns copied registers, fixed-width tokens, guest-range transactions and
ordinary user-mode host capability calls.  It does not import CCPU/SAS,
`Sim32GetVDMPointer`, `VDMREDIR`, host pointers, host handles or Bochs types.

`bx-core` and `bx-mantle` remain mechanical RAM/CPU owners.  The provider may
request checked reads and writes through the existing typed mechanical-action
seam, but it does not interpret or implement CPU, DOS-kernel, BIOS, PIC, or
device semantics.

## One provider state and transaction model

The replacement is one `dem_local_provider` owned by a DEM package session,
not an extension of `boot_namespace_plane`:

| State | Owner and invariant |
| --- | --- |
| mutation profile and CWD | DEM session; profile resolution is the sole direct/readonly/overlay/virtual selector. |
| host namespace | Borrowed, validated bx-vdm capability; only admitted root handles are reachable. |
| local file tokens | Session-private fixed table; native `HANDLE` never leaves bx-vdm. |
| search continuation | Session-private bounded table keyed by DTA/SRCHBUF checked location plus generation, never by record-resident pointer bytes. |
| pending transaction | One typed gather/write continuation with copied event/CPU and a monotonically checked action id. |

Every request follows this fixed sequence:

1. Verify DEM ingress, four-byte BOP window, real-mode CPU state and the
   package's selected 29-identity membership.
2. Decode only copied register fields and create all required bounded guest
   gather ranges.  No guest address is dereferenced in host code.
3. After the mechanical gather completes, parse the copied pathname, FCB,
   DTA/SRCHBUF or I/O bytes; resolve the common profile; perform the selected
   local-host, readonly, overlay or virtual disposition.
4. Build a fixed-width CPU result plus zero or more preflighted guest writes.
   The mantle executes writes only after all ranges are validated.
5. Clear/invalidate the pending record on any rejected, stale or failed
   continuation.  A token or continuation cannot be reused after release.

The provider does not share old `readonly_namespace`, boot-file, FCB-fence or
profile-search state.  Those units remain fixture evidence until the one
provider has passed its complete regression, after which their product routes
are removed or isolated in the action recorded by S3.

## Complete identity partitions

Implementation is organized by source component, but the resulting build and
regression are one provider delivery.

| Partition | Identities | Common implementation result |
| --- | --- | --- |
| Handle lifecycle | `00,02,08,16,1E,27` | opaque token lookup/release, seek/read/write/time/commit, source-specific byte counts and CF forms. |
| Namespace/metadata | `01,03,04,05,06,12,17,22,44` | one pathname gather/resolver and local-host create/open/delete/rename/attribute/check-path behavior. |
| Standard search | `09,0B` | DTA-address-verified continuation and copied DOS DTA records. |
| FCB search | `0A,0C` | SRCHBUF-address-verified continuation, extended-FCB attributes and copied record layout. |
| FCB file family | `07,20,2C,2D,2E,2F,30,31` | common FCB parse/name/token state, I/O record transactions and distinct FCB partial-write result. |
| Redirector boundary | `47,48` | explicit Redirector-deferred result only; no local-file or pipe substitute. |

No partition is independently called a completed BOP family.  The package
switch occurs only after all six partitions compile in the same source closure
and pass the complete matrix below.

## Capability and failure matrix

| Profile | Reads/searches/queries | Namespace or data mutation | Required result |
| --- | --- | --- | --- |
| direct | selected ordinary host APIs below admitted roots | selected ordinary host APIs after class authorization | source-shaped success or source error mapping |
| readonly | selected host reads and metadata queries | denied before host mutation | source-shaped readonly refusal; never an implicit virtual drive |
| overlay | merged base/overlay view | overlay-owned state only | no host write; all affected search and token behavior sees one overlay view |
| virtual | only a separately admitted virtual-volume backend | virtual backend only | explicit unavailable until that backend is implemented |

For ordinary host errors, `demClientErrorEx` mapping remains the reference.
The hard-error range (`ERROR_WRITE_PROTECT..ERROR_GEN_FAILURE` and
`ERROR_WRONG_DISK`) cannot be silently folded into ordinary `AX/CF` failure:
historically it arms an INT 24 packet.  Until a typed guest hard-error owner is
admitted, the provider must return one explicit source-derived hard-error
unavailable disposition, recorded in the package regression.  It must not
manufacture INT 24 in the adapter.  This limitation is global to the package,
not a reason to add per-service exception logic.

## Regression replacing the legacy family fixture

The new fixture has a real temporary admitted host root and one provider
session; it is not the old boot-image-only assertion that every `50:00..48`
produces some typed outcome.  It must prove:

1. every listed identity has one provider or the explicit `47/48` defer;
2. direct, readonly and overlay selection for each relevant mutation class;
3. token opacity, stale-token rejection, teardown closure and no raw handle
   or pointer bytes in CPU/guest state;
4. pathname/CWD drive admission and `..`/long-name/reparse rejection;
5. DTA/SRCHBUF stale, altered and cross-PDB continuation rejection;
6. `demWrite` zero-length, seek, truncate and disk-full form separately from
   FCB I/O partial-write form;
7. source-specific rename/path/pipe and hard-error dispositions; and
8. a source-built MSVC x64 `/MT` closure, documentation gate and diff check.

A bounded native trace is expressly after this regression and verifies the
whole package switch only.  It may not create an additional endpoint task.

## Delivery order

1. Introduce the provider session/pending-transaction ABI and its isolated
   direct/readonly/overlay regression without binding legacy BOP routes.
2. Add the six partitions above behind that same provider and expand the one
   package regression until all 29 identities are covered.

   The remaining FCB wildcard delete/rename source contract and provider
   regression are specified by
   [the FCB wildcard provider plan](t202-s4-dem-fcb-wildcard-provider-plan-001.md).
3. Switch DEM package dispatch once, displacing all S3-listed workaround
   routes as retain/migrate/replace/defer requires.  A partition may have a
   narrow transition route only when it can prove that its input token was
   issued by the new provider; an unqualified pathname entry (notably
   `50:01/03/04/05/06/12/17/22/44`) must not be partially switched merely
   because a temporary host fixture can exercise it.  It remains provider
   internal until the atomic package switch.
4. Run the package regression, then one bounded native trace; record the
   hard-error and virtual-volume limitations rather than claiming DEM or CLI
   completion.

### Atomic switch result

The switch is now installed in the DEM package session. When a whole provider
is present, all 27 local identities are selected by that provider: local-handle
operations; the nine pathname operations; four search operations; the five
FCB pathname operations; FCB close/date/I/O; and the source-owned FCB
wildcard operations. A legacy boot token is no longer a fallback capability:
the new owner rejects it as an invalid token. `47/48` remain outside this
local-filesystem set and retain their Redirector-deferred disposition. The
former boot snapshot path/fast-I/O assertions are fixture-only evidence and
are not a product fallback.

## FCB I/O transition rule

The `50:2F demFCBIO` transition may be installed before the final FCB/pathname
switch only when both conditions are independently true: the AX:BP value is a
live opaque token issued by this provider, and the package has a checked DTA
registration.  The route copies bytes to or from that registered
guest-physical range through a mechanical action; it does not dereference a
DOS pointer, discover an FCB, or expose a native handle. All other FCB
pathname/record requests remain behind the final atomic package switch.

The copied CPU-result slots retain `demfcb.c` exactly: `2C/2D` return the
opaque token in `AX:BP`, time/date in `BX:CX`, and size in `DX:SI`; `2F`
returns size in `AX:BX` and transferred bytes in `CX`; `30` returns date/time
in `AX:DX`; and `31` returns attributes/time/date/size in `AX/CX/DX/BX:DI`.
These are guest-register layout facts, not host ABI fields.

The same rule applies to the `demfile.c` pathname group: `01` reports queried
attributes in `CX`; local `12` reports size as `BX` high / `CX` low and clears
`DX` as its non-pipe marker; and `44` clears `DX` on the admitted check-path
success. The provider must never repurpose these guest slots for host state.

## Shared pathname transaction rule

The complete `01/03/04/05/06/12/17/22/44` group uses one provider-internal
transaction: DS:DX supplies the first path for `01/04/05/06/17`; DS:SI
supplies it for `03/12/22/44`; and `17` obtains its destination from ES:DI.
Every input is an independently range-checked, NUL-required OEM `MAX_PATH`
copy. The gathered bytes are the only inputs to the namespace partition.
`demRename` therefore needs two full 260-byte records; the generic gather ABI
v2 admits a maximum 520-byte transaction for precisely this bounded case.
No pathname service is routed through the package session until all nine
services and their direct/readonly/overlay/virtual matrix meet the final
atomic-switch gate.

The analogous FCB pathname record group (`07/20/2C/2D/31`) uses the same
bounded rule with its original address choices: `07` reads ES:DI, `20` reads
DS:SI and ES:DI, and `2C/2D/31` read DS:SI. Its transaction only forwards
copied records to the existing FCB wildcard/local-handle provider; it cannot
interpret an FCB or use a host handle. It remains un-routed until the complete
FCB family passes the atomic package gate.
