# M0 T484 closure

The owner accepted S7 and directed closure of S7 and T484. Foreground
activation was discussed and left outside the accepted scope. The existing
queue retains its order. The final product delivery is commit `f7e3b2e09`,
package `0b82f7de-640f-43e1-962e-73f904c0bf60`.

The [stage-by-stage record](../etc/evidence/t484-delivery-reconciliation.md)
covers S1 through S7. CMD/BAT owns the production and addon builds, code
generation, test artifact compilation, staging and publication. The runtime
acceptance harness and historical diagnostic records still include Python
and PowerShell scripts. The original guest source and `winnt.zip` are
unchanged from the T483 closure.

[S6](../etc/evidence/t484-s6-clean-cmd-acceptance.md) records clean, cached,
changed-input and failure-path build verification. The final S7 package
passed the cached full build, Full 20/20 and five private-desktop DOS dialog
trials. All eleven host images and both manifests were checked at the two
publication destinations. All fifteen addon hashes match their manifest.
[S7](../etc/evidence/t484-s7-dos-error-dialog-mouse.md) records the runtime
tests, publication identities and limits.
