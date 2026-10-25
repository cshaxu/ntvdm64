# M0 T456 closure

T456 closes Queue package W08, WOW32 fonts, text and metafiles.

| Stage | Durable record |
| --- | --- |
| S1 | [Font/text/metafile owner matrix](../etc/evidence/m0-t456-s1-font-text-metafile-owner-matrix.md) |
| S2 | [Glyph-outline rotation seam](../etc/evidence/m0-t456-s2-glyph-outline-rotation.md) |
| S3 | [Classic metafile owner/host audit](../etc/evidence/m0-t456-s3-metafile-owner-host-audit.md) |
| S4 | [Package acceptance](../etc/evidence/m0-t456-s4-package-acceptance.md) |

The sole production correction is the bounded `GetGlyphOutlineWow`
ignore-rotation adaptation in `48ab4e32a`; it uses temporary public GDI font
selection and preserves the original caller's selected object. T456 introduces
no guest media/configuration change, renderer, font/metafile registry or
private GDI import. `fcb2bce34` adds the S3 source/capability witness only.

All ten release/installed images remain hash-identical to the verified S2
manifest. Repeated focused tests pass. The existing private-desktop early
application exits are explicitly retained as non-passes rather than silently
reclassified. W06 dialogs, input, hooks and timers is next in Queue.
