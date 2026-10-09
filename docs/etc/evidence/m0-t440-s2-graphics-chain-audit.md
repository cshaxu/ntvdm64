# M0 T440 S2 — graphics-chain audit

## Result

The shared Window renderer already implements pixel comparison and damaged
rectangle presentation.  There is no missing dirty-rectangle feature in
`src/ntcon-exe/lib/kvm-window/render.c`.

The project-owned NTVDM graphics adapter was the earlier cost boundary:
`src/ntvdm-exe/win32/console_graphics.c` built a complete DIB payload at every
graphics invalidation, before `worker-base/publication.c` could retain only the
latest payload.  The SoftPC comparison display path instead rejects unchanged
source generation before capture.

The S2 renderer fixture measured 640x480 rendering at roughly 0.55 ms median.
That isolated result does not attribute guest execution, capture, transport,
NTCON or RDP latency.  It ruled out a renderer rewrite and admitted S3 only
for the project-owned source-capture boundary.
