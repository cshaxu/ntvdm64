# Current worker/WOW32 link and bridge identity

After the x86 worker relink, Ninja correctly marked the cached `wow32.dll`
link out of date because its graph consumes the worker's `ntvdm.lib`. Earlier
isolated candidate packages retained the previous WOW32 image; their tests
remain valid for those manifests only. The current WOW32 was relinked, and an
immediate Ninja dry run reported no work. The link retained existing LNK4017
and duplicate-export LNK4197 warnings; they are not newly claimed resolved.

The cached bridge object still hashes
`0FF8EB7E3E78E5CCEFC44D9E994D3B0DB811B4DFC64C0797084467B46E9B6135`,
matching the controlled BOOLEAN/loader fixture. The current WOW32 link map
attributes `_DllMain@12` to that object. The relinked WOW32 input hashes
`C26C7A2B511CF701D7ADC86E3DD810013AE1C19817C9E8DE9CABED31A9ABEEF9`,
which matches the staged package's WOW32 input hash. This establishes the
selected bridge in this input link, not a complete guest DLL lifecycle proof.

The ten-image isolated package manifest hashes
`99FE5760FF1404DA9AEFCE9FA102A0E6326742186EDB8DFE6229346B49CE889D`;
59 immutable guest files passed their manifest check. Three SOL Options
open/close/parent-recovery rounds and launcher exit passed. Opening took
1.907–2.625 s and closing 2.093–2.469 s; latency remains unresolved. The
other component inputs were inherited from an earlier candidate and still
require current-source identity and product-wide validation. This is not a
release or Full pass.
