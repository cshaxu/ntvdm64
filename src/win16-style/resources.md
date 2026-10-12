# Original Win3.1 visual resources

Owner explicitly admits resource embedding for the T445 S3 isolated experiment.
Original artwork is Microsoft's Windows 3.1 material, not project-authored or
relicensed artwork. Public redistribution rights have not been established;
this local experiment is not a production resource-release approval.

Source: owner-supplied installed `O:/w31/SYSTEM/VGA.DRV`, 73,200 bytes,
SHA-256 `53312ED013BDA513C74E33659F4EE7D1B4F2B1CF1A06AED672432A8FBC4A820A`.
The file remains read-only. The NE RT_BITMAP resources 32749, 32748, 32747,
32746, 32745 and 32744 are REDUCE, ZOOM, RESTORE and their depressed forms.
Each is 19 by 18 pixels. `frame_buttons.h` losslessly encodes decoded pixels
as top-down rows of four shade roles, not approximate redraw instructions.
Palette roles map black/white/128-grey/192-grey to the active style palette.
Original default shades reproduce original decoded pixels exactly.

Reproduce and verify using the read-only inspector, in an owned background run:

    tests/observation/inspect-win31-frame-resources.ps1 -Driver <original-VGA.DRV> -OutputRoot <fresh-build-root> -VerifyHeader src/win16-style/frame_buttons.h

It decodes the source NE resources independently and compares all 2,052 pixels
with the embedded six images. Output includes source identity, resource offsets,
dimensions and original previews. Padding/container headers are not artwork and
are not copied. No binary executable code or original module is linked.

USER's normal and tracking paths select distinct normal/depressed OEM images;
the experiment uses that selection contract. Public GDI scales the selected
raster once into the existing caption button slot. Slot dimensions and frame
layout remain the separately reviewed adaptation, not proof of complete USER
behavior equivalence. CLOSE's two-part resource needs further source slicing
analysis; the system-menu bar remains authored. Original font composition is
also pending. These remaining gaps are not masked by the six-button import.
