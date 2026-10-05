# T430 S5 DEM directory reset

Sequential admission after S4 fb5843dc6. Initial source review identifies
FileFindReset's explicit unsupported-service fallback and an existing T230
actual FCB import fixture. Production NtVdmControl has a query binding API,
but no production binder is established. This is a missing fast path, not
proof of an ordinary FCB correctness failure. Actual filesystem proof and
source comparison precede any correction or original-defect disposition.

S4's published package remains the baseline. No production change, new query
algorithm, guest patch or runtime capability claim follows from this note.

## Initial provenance gate

Current demsrch.c::FileFindReset calls NtVdmControl only for a remembered
non-dot name and SupportReset, then takes its original NtQueryDirectoryFile
loop after an unsupported result. The loop requires both remembered index
and name, and explicitly warns it will fail if the remembered file was
deleted. That warning/algorithm is present in pinned OpenNT, not a new
project failure policy. The current monitor facade returns
STATUS_NOT_IMPLEMENTED when no query capability is bound; no production
bind_query_dir caller is established by the source sweep.

Pinned ntos/vdm/vdm.c::VdmQueryDirectoryFile is a kernel IRP implementation
with object-manager, driver and completion ownership. It is not a public
user-mode function body that can simply be called from a new adapter.
The sibling SoftPC source sweep finds no demsrch/VdmQueryDir counterpart;
this does not authorize a novel repair of the original deletion limitation.

The T230 fixture is historical comparison only: it references removed
opennt-bop-overlay/adapter-mvdm-host-out composition and cannot serve as the
current production caller. Its idle-reopen and FCB cleanup cases inform the
new selected-source test, not a dependency on that old runtime architecture.
Runtime proof, mutation disposition and S5 closure remain pending.

## Selected-source proof and disposition

FileFindReset is exactly equal to pinned OpenNT/base/mvdm/dos/dem/demsrch.c
after newline normalization (4,438 characters). Whole-unit hashes differ due
to already registered surrounding bindings/source selection. Selected SHA256:
7832188C12A40752965FE51FFB14E8ADE6DA8913A31E87925D9B125F882B3737;
cited original SHA256:
2FB56AAD4D35A02DEC38E9202D41B94A25076428C3D9EB08211D89F890C896EE.
No mirror changes or format-only rewrite are introduced in this stage.

The kernel fast path sets IRP_MJ_DIRECTORY_CONTROL / IRP_MN_QUERY_DIRECTORY,
the remembered FileIndex and SL_INDEX_SPECIFIED, with file-object/IRP/MDL
ownership. User-mode NtQueryDirectoryFile has no corresponding FileIndex
argument. A finite carrier remains eligible under source policy, but a dummy
callback or another slow scan would not restore that indexed operation.
The existing fallback works normally; no ordinary correctness need for a new
carrier is demonstrated here. Missing indexed capability/performance and the
original deleted-name failure remain debt, not completed indexed restart.
The sibling source selection has no matching correction to adopt.

## Actual tests

tests/observation/verify-dem-directory-reset.ps1 compiles the selected original
demsrch.c through tests/mvdm-host/dem_directory_reset_test.c using production
Ninja DEM flags, x86 /MT CCPU40. No private types/algorithm are copied.
Existing fixture seams supply unrelated CPU/host closure; actual filesystem
and monitor functions remain selected. The map pins FileFindOpen,
FileFindReset, FileFindNext and FillFCBSrchBuf to the fresh test unit and
NtVdmControl to monitor-bindings/vdm_control.obj.

build/M0-T430/S5/r003-provider and r010-provider-final pass47/0 each:
unchanged reopen, unrelated rename/
create, deleted remembered name, first/next/end/FCB cells, undersized buffer,
original handle/buffer counts and actual closed-handle checks. Normal and
unrelated mutation return0; deleted remembered name returns80000006,
STATUS_NO_MORE_FILES. That reproduces a retained limitation, not mutation
tolerance. The original SupportReset assignment is not changed incidentally.

tests/observation/verify-dem-fcb-guest.ps1 and dem_fcb_probe.asm exercise actual
DOS INT21/11h/12h through S4. r009-guest passes: three distinct owned 8.3 names
in a guest-written40-byte witness, FF at end and for missing pattern, outer
COMMAND completion1. Elapsed7227ms; test COM SHA256:
8B9EB55DA33E626E23532BB36E6FFD9DD4B7FB7A746CC06B14F1E670FFD2EEA8.
No immutable guest image is patched/replaced. Z: and exact owned test files/
package processes are cleaned on exit.

The provider deliberately closes/reopens to exercise reset without waiting
eight minutes. Guest evidence proves ordinary FCB ingress/continuation, not
mutation plus eight-minute expiry in a running guest. Indexed driver
behavior, all filesystems, concurrent mutation and unbounded-directory
performance are not proved.

## Failed harness attempts

r001 failed on a wrong fixture SRCHBUF field spelling. r002 ran but produced
no stdout because original PROD printf is deliberately quiet; r003 uses
fprintf and requires all47 assertions. No production changes followed.
r004–r007 used different launch/container settings and failed before guest
execution87; r008's physical long package path failed1067. These reports do
not establish a unique product-versus-harness cause. r009 uses the
preceding matrix's combined Z: path, package PATH, private desktop and
milestone settings. This corrects test setup, not arbitrary non-isolated/
long-path support or a causal production repair. Earlier reports remain
failures. The known long-path
limitation is outside this packet and unchanged.

## Reproduction and gate

```powershell
./tests/observation/verify-dem-directory-reset.ps1 -OutputRoot build/M0-T430/S5/<fresh-provider-run>
./tests/observation/verify-dem-fcb-guest.ps1 -RuntimeRoot build/M0-T430/S4/r006-runtime -Observer build/M0-T427/S4/r049/console-startup-observer.exe -OutputRoot build/M0-T430/S5/<fresh-guest-run>
```

Runners fail closed on assertions, timeout and absent/wrong witness. Only
test inputs/evidence change; production sources/build inputs and published
eight-file set remain S4. No rebuild/republication or full product matrix
is required for this test/disposition-only delivery. S6/S7 and owner
acceptance remain before final T closure.

Review confirms all eight O:/winnt/system32 hashes match S4's verified
r006-runtime set; Z: is absent. Documentation governance, relative links and
diff whitespace checks pass. No original source or guest media is changed.
Only this stage's new TODO row is included in delivery; existing other-session
proposal/TODO edits remain preserved and excluded.
