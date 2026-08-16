# T221 S3 FCB wildcard Overlay binding

`50:07` and `50:20` now select the same Overlay wildcard provider after the
shared mutation profile decision. It consumes only the selector-blind visible
enumeration and the existing private mutation backend; Direct, Readonly and
Virtual dispositions remain distinct.

The source-built x64 `/MT` fixture at `build/M0-T221-S3/090` proves Overlay
wildcard delete succeeds while the corresponding host file remains present,
and that the source-shaped same-name rename failure remains a carry failure.
The remaining evidence gap is a standalone successful wildcard rename with a
distinct destination template. Therefore this is binding progress, not
wildcard-package closure.
