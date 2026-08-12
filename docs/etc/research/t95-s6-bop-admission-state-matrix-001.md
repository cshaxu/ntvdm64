# T95 S6 BOP admission-state matrix 001

## Question

After the complete OpenNT BOP inventory and generic listener were introduced,
which BOP identities have only catalogue visibility, which have adapter source
code, and which have real source-built guest evidence?  In particular, can a
source file or a historical dispatch fragment be treated as proof that a
service is connected to the current minimal Bochs route?

## Inputs and procedure

This is a read-only correlation of:

- the pinned complete inventory and its 256-selector listener test, recorded
  in `opennt-bop-definition-inventory-001.md`;
- `src/bx-ntvdm-adapter/bx_ntvdm_bop_catalog_v1.[ch]` and the registered
  default-off Bochs listener;
- `bx_ntvdm_adapter_runtime.c`, `bx_ntvdm_cpu_result_bridge_v1.c`,
  `bx_ntvdm_startup_transaction_bridge.c`, and every existing
  `*_service.c` adapter unit; and
- the named r12/r20/r26/r27/r29/r30 and T95 S6 source-control-flow evidence
  records cited below.

The audit does not build or run Bochs, alter a BOP dispatch, read guest
memory, or infer that an unobserved catalogue member is required.

## Terms

| State | Meaning |
| --- | --- |
| `observed` | The generic default-off listener recognizes a copied `C4 C4` window and logs identity.  It never resumes or mutates state by itself. |
| `adapter unit` | A source unit recognizes the exact copied identity and can form a typed result/registration.  This is not runtime evidence. |
| `minimal-route evidence` | A source-built guest trace reaches the identity and records the intended narrow transaction/result. |
| `not admitted` | No current product claim permits it to resume.  It remains listener/pass-through even if an historical OpenNT owner exists. |

`adapter unit` and `minimal-route evidence` are deliberately independent:
the old/general runtime and the narrow source-built fixture have different
link and lifecycle closures.  Neither may be promoted on the strength of the
other.

## Complete observation boundary

All `C4 C4 <selector>` values are observable, including undefined selectors;
the catalogued OpenNT top-level selectors and SoftPC BIOS constants merely add
source class and family metadata.  For the six service families, a copied
fourth byte is logged as present or truncated without attempting dispatch.
The pinned inventory has 16 top-level selectors, six service families, 41
SoftPC BIOS constants and 349 annotated historical BIOS table rows.  This is
the complete *definition inventory*, not a promise to rehost 349 BIOS rows.

## Existing exact identity units

| Identity | OpenNT name / source role | Current evidence state | Admission conclusion |
| --- | --- | --- | --- |
| `50:11` | `SVC_DEMLOADDOS` | Host-service plane has the one NTDOS bulk write; r12/T95 trace proves its first controlled source-built transaction. | Narrow startup exception only; no general DEM dispatcher. |
| `12` | `BOP_MEM_SIZE` / conventional memory | Source-shaped adapter result; post-`50:11` audit ties the caller to the firmware BDA value. | Admitted only for the selected bare profile. |
| `15` with `AH=88` | `BOP_15` extended memory | Source-shaped profile result and r26 trace evidence. | Admitted only for this BIOS subfunction/profile. |
| `50:0D` | `SVC_DEMGETBOOTDRIVE` | Exact adapter unit; r27 records the reached call. | Uses the documented contained fallback only; no registry owner. |
| `50:0F` | `SVC_DEMGETDRIVES` | Exact adapter unit using the frozen CLI drive snapshot; r29 records the reached call. | Capability is drive inventory, not a filesystem projection. |
| `50:1B` | `SVC_DEMSETDTALOCATION` | Two-phase guest-read/registration unit and narrow trace evidence. | Registration only; it does not grant later file access. |
| `50:32` | `SVC_DEMSETHARDERRORINFO` | Exact registration unit reached after the DTA sequence. | Registration only; `50:33` retry/error delivery remains unadmitted. |
| `50:3B` | `SVC_DEMDEBUG` | Exact false-branch unit and startup trace. | Contained CLI has no host-debugger relationship. |
| `50:46` | `SVC_DEMGETDPBLIST` | Multi-write unit exists; source contract is in `r20-dem-getdpblist-boundary.md`. | No independent runtime acceptance claim without its matching generic multi-write consumer. |
| `54:04` | `SVC_CMDGETCURDIR` | Multi-write unit exists; r30 records the reached call and current-directory closure is separately documented. | Root-string result is not a host filesystem or placement capability. |
| `54:05` | `SVC_CMDSETINFO` | Exact registration unit; r20 contract and reached trace exist. | Registration only; it does not instantiate historical SCS/console behavior. |
| `54:0C`, `54:0D` | CONFIG/AUTOEXEC pathname requests | Reached `54:0C` is listener-only; source audit identifies a paired materialization/read-handle dependency. | Explicitly not admitted pending S7 placement plus a bounded read-only file projection. |
| `54:02` | `SVC_CMDCOMSPEC` | Historical owner/source traced. | Listener-only: reports an already existing guest COMSPEC and cannot load COMMAND. |
| `50:12`, `50:00`, `50:16`, `50:02` | DEM Open/Seek/Read/Close | Historical caller/ABI closure recorded. | Listener-only pending the one coherent S7 token/projection capability. |

All other DOS, COMMAND, WOW, XMS, DPMI, redirector, debugger and SoftPC BIOS
identities are `observed` and `not admitted` unless a later evidence record
names an exact exception.  The inventory's source definition is sufficient to
classify them, but not sufficient to assign a modern host owner or a CPU
result.

## Interpretation

The project has met the requested *BOP interception baseline*: every BOP
encoding is observable at the adapter/Bochs seam, with source-defined entries
classified and unknown encodings still visible.  It has not, and must not,
turn that baseline into an all-selector dispatcher.

The next real stop (`54:0C`) does not select the next numeric identity.  It
selects the already identified S7 file/placement capability, because its
subsequent ordinary DOS open consumes a guest pathname.  The closest adjacent
identities (`50:12/00/16/02`, `54:0D`, `54:02`) share that same closure and
must be admitted, implemented and tested as one bounded capability or remain
listeners.

## Follow-up

Retain this matrix as the test oracle for future BOP work.  A new handler must
add: (1) its exact matrix row; (2) OpenNT caller, historical-owner and
post-call ABI/failure evidence; (3) an owner-side, typed transaction test;
and (4) minimal-route runtime evidence before the row can claim `admitted`.
No BOP may be connected merely because its selector was observed or because a
legacy adapter source unit exists.
