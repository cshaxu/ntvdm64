# CCPU vglob source identity and selected C-VID witness

The pending `softpc.new/base/ccpu386/vglob.c` is byte-identical to its two
OpenNT counterparts and to the selected `base/cpu/src/evid/vglob.c`. The
[observer](ccpu-vglob-observe.py) and [receipt](ccpu-vglob-evidence.json)
bind the four source hashes, current graph, object, worker map and prior
fixture record. The selected Ninja object is compiled from the **different**
physical `base/cpu/src/evid/` path; 51 current worker-map accessor symbol
lines name `original-softpc-cvidc:vglob.obj`. Source equality alone does not
prove that the alternate CCPU path was selected or built with this profile.

The retained [r74 fixture record](../cvidc-vector-fixture-r74-evidence.json)
reports passing 38 accessor pairs across four values, plus video vector
selection and guarded scratch checks. It used selected C-VID objects, host
test seams and `/force:multiple`. The current fixture source does **not**
match its recorded historical source hash, so this report does not bind that
historical run to the current fixture or current release. The existence of a
worker map also does not itself prove that its image was executed.

The identity and historical fixture support a bounded partial-implementation
assessment by shared source text if the project records equivalent source
implementations across paths. If classification requires a test or compile of
the exact CCPU path/profile, its disposition remains pending. In either case,
do not mark full: CCPU profile/layout, actual runtime ownership, mode changes,
failure handling and device lifecycle remain unproved; source text retention
is 100%, not a behavioral-completion percentage. No build, guest or source
was changed by this observation.

Coordinator decision: partial denotes the retained implementation selected from the byte-identical C-VID provider, not compilation of this CCPU path or proof of the CCPU profile. The exact profile, deployed image and device lifecycle remain open in S13.
