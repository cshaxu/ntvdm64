# T221 S2 P1: file/handle/FCB/search provider current-state map

## Question

Does the existing 29-service DEM implementation already own the complete
provider state and all four shared mutation modes, or does it contain only a
direct/readonly partial seam that must be completed as one package?

## Current state owners

| State | Current owner | Boundary fact |
| --- | --- | --- |
| Opaque local file token | `bx_ntvdm_dem_file_session_v1` | Holds private `HANDLE`, generation and copied PDB owner. The guest sees only the 32-bit token; it is neither a Win32 handle nor a JFT/SFT. |
| Local path-to-token bridge | `bx_ntvdm_dem_local_file_backend_v1` | Owns root-relative host open and adopts the private handle into the session. |
| Package state | `bx_ntvdm_dem_whole_provider_v1` | Owns the file session, local backend, pathname/FCB search transaction, temporary copied direct namespace PDB owner, and optional declared-image/search inputs. |
| Search continuation | `bx_ntvdm_search_transaction_v1` | Keeps a provider-owned plan keyed by copied boundary/CPU/PDB/DTA or SRCHBUF inputs; host pointers cannot become guest continuation data. |
| Profile authority | `bx_ntvdm_mutation_profile_v1` plus `bx_ntvdm_dem_profile_consumer_v1` | Authorizes DEM by mutation class; provider code may resolve the class but cannot make a selector-local mode choice. |

The owner layout is directionally correct: `dem_whole_provider` is not a BOP
ingress and the token/session APIs do not take a BOP selector.  It is still
only a partial package because several original contracts and profile backends
are incomplete.

## Four-mode reality check

`bx_ntvdm_mutation_profile_v1` defines Direct, Readonly, Overlay and Virtual
modes. The current namespace partition resolves this shared policy, but its
actual behavior is deliberately incomplete:

| Mode | Current namespace/local-backend behavior | S2 disposition |
| --- | --- | --- |
| Direct | Opens/mutates only selected root-relative host paths through private tokens. | Retain as one provider backend; audit all 29 service forms and source failures. |
| Readonly | Rejects mutation with a typed refusal and can use declared startup images. | Retain as a backend, but merge file/search/FCB coverage under the same provider rather than retain endpoint-local leaves. |
| Overlay | `local_file_backend` and namespace partition explicitly report overlay/unavailable; no overlay state exists. | Add a package-owned overlay view only in the later implementation S; do not fall through to direct host writes. |
| Virtual | Explicitly unavailable; no implicit boot C: is created. | Add only as an explicit profile backend, including the user-approved excluded-host-C boot-volume case; it must remain opt-in and independently lifecycle-tested. |

This confirms the shared ABI is the correct authority but not proof that all
four backends already exist.

## Required whole-provider design

The implementation plan must retain a single state owner with these backend
rules:

1. One root/view resolver selects direct host roots, readonly projection,
   overlay view, or explicit virtual boot volume before a file, directory,
   FCB or search operation is executed.
2. One opaque token table has a backend kind and generation; all close, seek,
   read, write, time and commit operations validate that token kind rather
   than interpreting a host handle or JFT/SFT.
3. One search continuation table owns pathname and FCB first/next state;
   overlay enumeration merges the selected backend view without serializing a
   host pointer into DTA/SRCHBUF.
4. `47/48` are not local backend kinds. They stay unavailable until the
   Redirector package owns pipe token state.
5. Copied PDB ownership remains diagnostic/compatibility state around direct
   opens only. It is not released by `50:3C` and does not create an adapter
   DOS process table.

## Follow-up

P2 must turn these rules into the 29-service subfamily-to-backend matrix and
one positive/negative regression pair per subfamily. No runtime source or
profile behavior changes in this P1 record.
