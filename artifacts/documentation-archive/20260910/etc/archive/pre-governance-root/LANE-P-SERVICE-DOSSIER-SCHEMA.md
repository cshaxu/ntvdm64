# Lane P Service Dossier Schema

Status: M40 governance schema, 2026-08-08. This is a prerequisite evidence
format for a contained-profile adapter overlay. It is not source code, a
selector implementation, or authorization to modify historical material.

## Required Header

Every dossier states:

```text
identifier:
historical source path and function:
dispatcher owner and entry fact:
lane: P only
fixed profile version:
required generic backend capabilities:
historical-body region replaced:
reason H1/H2 bridge is insufficient:
```

The reason must name the excluded historical product policy and cite M38-style
source lines. “Modern Windows incompatibility” is not sufficient.

## Required Contract Table

| Field | Required content |
| --- | --- |
| Entry | copied register inputs, mode assumption, caller order, and record layout facts |
| Profile input | immutable, pre-admitted bytes/metadata/capability; no ambient host query |
| Guest reads | every logical range, semantic size, translation route, and validation rule |
| Guest writes | each staged range, bytes/encoding, ordinary-RAM requirement, overlap relationship, and commit grouping |
| Register result | exact permitted GPR/FLAGS modifications and retry result, if any |
| Session result | metadata, stop, fault, or completion facts; no hidden global mutation |
| Success | observable original-route outcome and trace label `lane_p` |
| Failure | validation/provider/capacity/cancellation outcomes, original return/termination/fault owner, and no-mutation rule |
| Deferred branches | exact conditions that stop or fail rather than gaining fallback behavior |
| Historical difference | concise statement of the original product-policy region not preserved |

All address operations must use the M33 transaction vocabulary. A dossier may
not say “pointer,” “map guest RAM,” “write directly,” or “best effort.”

## Required Evidence

1. Source excerpt and exact original caller/function ownership.
2. One fixed-profile input specimen described only by roles, lengths, hashes,
   and redacted values; never stored protected guest content.
3. Positive fixture with expected copied transaction and register disposition.
4. Invalid-address, range/provider, capacity/retry, and capability-failure
   fixtures proving guest memory remains unchanged where required.
5. Trace assertion proving original dispatcher/function entry was reached and
   the resulting event carries `lane_p`, not a historical-restoration label.
6. Link/isolation assertion proving the overlay is outside default runner and
   Lane H reference targets.
7. Differential explanation showing why its observed behavior is not used as
   evidence for original-host restoration.

## Implementation Admission Checklist

All items must be true before code is admitted:

- M36 backend adoption gate has passed for every required capability.
- M37's second-consumer gate is satisfied or separately resolved by core owner.
- The dossier identifies no raw host handle, process-global current directory,
  environment, registry, system-directory fallback, temporary file, or device
  path.
- A service failure does not become a synthetic DOS success.
- The overlay preserves original dispatcher ownership and never invokes its
  function through a parallel selector decoder.
- A named source-modification admission exists if the approach touches an
  imported historical source rather than an adapter-local overlay.

## Initial Dossier Order

The first dossier set is ordered by bootstrap dependency, not convenience:

1. set-info binding;
2. COMSPEC announcement;
3. initial environment;
4. configuration/AUTOEXEC result;
5. first command record;
6. contained current directory;
7. terminal acknowledgement and fixed keyboard result;
8. one-shot completion and stop.

No dossier grants implementation for a later item merely because an earlier
one is described. Native execution, redirection, PIF, save-world, TSR, WOW,
and generic keyboard installation remain outside this first set.

## Result

M40 converts Lane P overlay work into explicit, individually reviewable
evidence packets. M41 will instantiate the first two dossiers, set-info and
COMSPEC, because they have the smallest profile policy surface and expose the
transaction boundary without configuration or process policy.
