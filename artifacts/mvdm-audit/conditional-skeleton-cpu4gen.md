# CPU4GEN conditional skeleton restoration

The OpenNT and current `cpu4gen.h` each contain 215 `#ifdef CCPU` groups. Group order and each selected CCPU branch's normalized `#define` lines were checked equal. The original non-CCPU alternative contained a nested PROD conditional in 22 groups where the current alternative held only the excluded-branch error. Those 22 original PROD/else/endif shells have been restored with errors for deleted bodies; retained pre-existing `#line` directives preserve the current active path. Original identity and group ordinals are in `conditional-skeleton-cpu4gen.json`. This does not enable non-CCPU or CPU30.

The x86 cached NTVDM build passed. Against the published text-aperture package, `.text` and `.data` are byte-identical; `.rdata` differs only at two bytes in the PE debug-directory timestamp field. Whole-image identity is not asserted. No guest changes, runtime retest, or republication occurred. Other missing conditional candidates remain open.
