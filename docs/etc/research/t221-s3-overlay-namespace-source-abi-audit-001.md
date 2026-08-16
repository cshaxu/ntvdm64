# T221 S3: Overlay Namespace Open/Create Source and ABI Audit

## Question

What must be recovered before `50:03` (create), `50:12` (open), and `50:22`
(create-new) can jointly admit the Overlay view without turning the resolver
into three endpoint-specific implementations?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demdisp.c` service ownership table.
- `src/opennt/base/mvdm/dos/dem/demfile.c`: `demOpen` and
  `demCreateCommon`/`demCreateNew`.
- Current `bx_ntvdm_dem_namespace_partition_v1`, path router, Overlay
  resolver/file object/session token and T221 S2 disposition.

## Source contract map

| Concern | Original `demfile.c` rule | Current adapter state | Required shared Overlay seam |
| --- | --- | --- | --- |
| Path | `DS:SI`; slash normalization before `CreateFileOem` | checked copied OEM pathname then canonical drive-relative resolver | retain copied path/range contract; normalize once before view selection |
| `50:12` access | `BL & ACCESS_MASK`: read, write, or read/write; `AL != 0` rejects EAs | Direct maps the three access forms and rejects `AL != 0` | use the same access result for private handle admission |
| `50:12` share | compatible/deny modes map to Win32 share flags and conflict at open time | Direct maps share flags; private Overlay object has no share state | bounded per-private-file share gate; reject with `ERROR_SHARING_VIOLATION` |
| `50:03/22` create | read/write access, full delete-capable sharing; `CREATE_ALWAYS` / `CREATE_NEW` | Direct uses these dispositions | carry disposition, source attributes and private share state through one resolver/open request |
| Attributes | create's `CX` becomes `FILE_ATTRIBUTE_NORMAL` when zero; volume-label is a separate historical route | Direct does not yet reproduce the volume-label exception | preserve ordinary attribute value in Overlay entry; leave volume-label route explicitly unavailable until `demlabel.c` owner is admitted |
| Result | opaque host handle is returned in `AX:BP`; open returns size in `BX:CX`, `DX=0` except named pipe | current token façade returns private opaque token and size fields | return the same copied token/result layout, never a backend token or host handle |
| PDB ownership | guest DOS owns JFT/SFT; host handle is associated to the caller | current package reads copied `CurrentPDB` before these calls | adopt Overlay token with the same copied PDB owner; no adapter JFT/SFT mirror |
| Named pipe | `IsNamedPipeName` loads VDMREDIR and uses a pipe path | Redirector is deferred and `47/48` unavailable | reject/defer named pipe before ordinary file-view resolution; never materialize it as a COW file |
| Errors | `demClientError` selects normal or hard-error result | existing `fail()` preserves current normal/FFFF boundary | preserve that mapper; resolver/backend expose ordinary Win32 error only |

## Findings

The P6 resolver is intentionally only a host-base copier.  It has no
per-file share table, no create-attribute input, no volume-label owner and no
named-pipe classification.  Calling it individually from `03`, `12`, or `22`
would therefore violate both the original contract and T221's one-provider
rule.

The existing `namespace_partition` also has the correct common BOP gather,
path and copied-PDB transaction seam.  The missing work belongs below that
partition as one selector-blind Overlay namespace backend request:

```text
checked OEM path + CPU open/create fields + copied PDB
    -> Overlay namespace request (access, share, disposition, attributes)
    -> path/view resolver + private share gate + COW file object
    -> typed session adoption + original AX:BP/BX:CX/DX result form
```

## Disposition

`03/12/22` remain explicitly unavailable in Overlay until the complete request
exists.  This is not a regression: P7's handle subfamily consumes only a
pre-adopted private token and makes no claim about ordinary namespace opens.

The next implementation packet must add the shared request/backend, cover all
three identities in Direct/Readonly/Overlay/Virtual disposition tests, and
prove no Overlay operation opens a host handle with write access.  It must not
add selector-specific resolver calls or enable named pipes/Redirector.
