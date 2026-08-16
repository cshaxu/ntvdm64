# T221 S3 FCB owner-package admission

## Question

How must the remaining FCB identities be recovered as OpenNT owner packages,
rather than as trace-selected individual BOP handlers?

## Inputs

- Original owner: `src/opennt/base/mvdm/dos/dem/demfcb.c`.
- Current partitions:
  `bx_ntvdm_dem_fcb_handle_partition_v1.c`,
  `bx_ntvdm_dem_fcb_wildcard_partition_v1.c`, and
  `bx_ntvdm_dem_fcb_io_route_partition_v1.c`.
- T221 whole-provider disposition and current token/view state.

## Owner families

| Services | Original owner behavior | Current route | Required Overlay recovery |
| --- | --- | --- | --- |
| `50:07`, `50:20` | `demDeleteFCB`, `demRenameFCB`: enumerate FCB wildcard, filter extended-FCB attributes, substitute destination meta characters, then mutate each match. | Direct host enumeration/mutation; Readonly access denied; Overlay/Virtual invalid function. | One merged visible enumeration and wildcard transaction using the namespace Overlay mutation backend; preserve partial-success and attribute-filter rules. |
| `50:2C`, `50:2D`, `50:2E`, `50:2F`, `50:30`, `50:31` | `demCreateFCB`, `demOpenFCB`, `demCloseFCB`, `demFCBIO`, `demDate16`, `demGetFileInfo`: one FCB file/position/info lifecycle. | Direct Win32 handle/session token; `2F` route uses checked DTA read/write actions; Overlay/Virtual invalid before lifecycle. | One typed FCB backend-kind adapter over the existing private Overlay file token, with source-shaped info/date/size records and the existing checked DTA transport. |

## Required recovery order

1. Reuse `demfcb.c` wildcard/attribute/meta-character and register-result
   contracts as the source reference.
2. Extend the existing typed file session and Overlay file backend at their
   package seam; do not create guest JFT/SFT state or host handles.
3. Bind all six handle/I/O services as one lifecycle family, then bind both
   wildcard services as one enumeration/mutation family.
4. Run Direct/Readonly/Overlay/Virtual family regression only after each
   whole family is wired. Native trace remains an integration check afterward.

## Explicit non-solutions

- Do not turn a hit on `50:2F` into a standalone read/write implementation.
- Do not make FCB Overlay use Direct host handles or bypass COW attributes.
- Do not copy FCB/DOS kernel state into the adapter.
- Do not claim the Direct fixture's `HANDLE` result layout proves Overlay
  lifecycle closure.

## Follow-up

The next implementation P begins with a typed FCB Overlay token/info/IO seam;
the wildcard group follows its merged enumeration contract. Virtual remains
explicitly unavailable until its independently admitted view exists.
