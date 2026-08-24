# M0 T262 S3 — OpenNT guest zero-difference closure

## Result

The complete guest mirror is byte-identical to its OpenNT source roots:

- `dos-v86`: 578 files
- `wow16`: 2 files
- total: 580 files

The sole untracked in-root input is
`dos-v86/cmd/append/sysmac.lib`. Its SHA-256 is
`58C68BBC15C35273F18D5BCCF3DDA919BD66F0B023341BE17E59804082E8A452`,
which matches `refs/opennt/base/mvdm/dos/v86/cmd/append/sysmac.lib` exactly.
It is original guest build input, not a generated file or a divergence, and is
therefore added to the mirror index unchanged.

No `DIVERGENCE` marker occurs beneath `opennt-guest`; the README's zero-entry
intrusion register remains correct.
