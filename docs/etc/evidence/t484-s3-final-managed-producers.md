# T484 S3 P198: final supplemental managed producers

## Retention decisions

Retire win101_setup_visual_probe and its two exclusive callers,
observe-win101-image and observe-win101-setup. Source inspection finds fixed
T425/T434 build artifacts, private historical input roles, Z: ownership and
subst launch. There are no current callers of those two runners. This is a
sealed investigation chain, not the supported win101-setup tool or current
native control fixtures; those remain unchanged. Remove460 lines rather than
migrate the obsolete visual helper. Historical findings remain in Git/evidence.

Keep the OEM guest and DPMI fatal assertions. No existing test was demonstrated
to replace their real DOS volume/file checks or exact fault-dialog actions.
Move each existing C# body unchanged to test/observation, built through the
shared native csc/CMD rule. Runners load the cached assembly instead of compiling
source through Add-Type. Existing guest assertions and failure handling remain.
Their old runtime staging still requires reconciliation; producer migration is
not proof of complete guest acceptance or modern isolation.

## Verification

Owned background32904 runs build/t484-s3-final-managed-producers/run.ps1.
Both native libraries compile. The extracted LF C# bodies exactly match HEAD's
inline bodies. Actual GetVolumeInformation success and invalid-root failure,
absent-process dialog lookup and invalid-dialog text/click checks pass.
These are bounded host interop checks; they do not exercise a real guest fatal
dialog or newly pass the OEM guest scenario. Cached full build and11 raw/22
published identity checks are required in the same run. Exact product identity
avoids redundant publication. No product, mirror, original guest or asset change.
Final governance/script parsing precede the containing commit and push.

## Tracking

All13 supplemental compiler-review candidates now have retention dispositions.
Two newly retained chains join shared staging follow-up:23 supplemental chains
remain, while the frozen355 census stays316 disposed/39 pending. This is not
S3 closure; shared preparation/staging and final clean acceptance remain open.
