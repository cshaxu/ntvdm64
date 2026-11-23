# T484 S3 P61 obsolete style aggregator

B0067, test/integration/verify-wow-s3-guest-contracts.ps1, is an orphaned
aggregator for the removed project-owned Win16 Style feature. Its twelve
combinations require Off/Win16/All style modes and delegate all execution to
verify-win16-hook-probe.ps1. Commit db91004e5 (T450) deleted that provider and
the style implementation. The retained aggregator cannot execute its contract;
its fixed thirteen-image requirement also predates the current product package.

Repository searches find no current test/tool caller of this aggregator. It
is therefore retired rather than translated into a different twelve-case
acceptance claim. No compiler target, product source, immutable guest or
published image changes. Existing verify-wow-dialog-guest.ps1 retains ordinary
and common-dialog verification; verify-win16-shell-hook-probe.ps1 retains the
original hook boundary without style modes. Their native witness targets and
all underlying probe sources remain intact. This does not claim that these
tests were executed again or that the removed style matrix has a replacement.

The owned verification checks the historical deletion, absence of current
callers, retained native producers/runners, and documentation governance.
The unchanged eleven host inputs and twenty-two published image hashes reuse
P60 delivery evidence; no build input changed, so another host build or runtime
test would exercise identical inputs. Receipts are retained under
artifacts/build-migration/t484/s3-style-retirement. The fixed inventory remains
355: completed dispositions become 134 and remaining dispositions 221.
S3, shared staging migration and the final clean acceptance remain open.
