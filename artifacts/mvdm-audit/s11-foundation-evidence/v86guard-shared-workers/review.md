# V86guard release: shared WOW task retirement and idle callback

The existing shared-task `verify-wow-idle-workers.ps1` used the V86guard
package and two hash-checked prebuilt Win16 guests. Two distinct tasks were
observed in worker 12124. The registered task's idle vector ran four times
on the peer task; the peer request then exited successfully while the
registrar remained active, and the registrar subsequently exited normally.
The test checked that each task retired exactly once, no retired task received
further idle dispatch, and the surviving registrar continued afterward.
`native.txt`, `identities.json`, `observer.txt`, inputs and trace hashes retain
the evidence. The ten package images were matched to the published manifest
in the companion export recheck.

This verifies the scripted same-worker normal path, not arbitrary callbacks,
forced termination, every dialog/object lifetime, or the unrelated historical
SOL 1400. Only the native host observer was built for this run; guest files
were not modified or rebuilt.
