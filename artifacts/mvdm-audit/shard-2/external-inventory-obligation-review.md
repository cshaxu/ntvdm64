# External inventory obligation review

## Scope and identity

Read-only cross-reference of the 42,963 frozen external identities against the current external source-coverage snapshot; all frozen IDs match exactly once. OpenNT 21,334 and OpenNT-4.5 21,629 are comparison editions, not unique current-runtime obligations. External current coverage contains 50,153 total records; it is not synonymous with this frozen subset.

SHA-256:
- `artifacts\mvdm-audit\frozen-external-audit-rows.csv`: `949d696e96f088a69ea437a41f15a09d50c8218ba29983a15a7b31e25af95623`
- `O:\repos.hobby\ntvdm64-wow32-audit\management\source-coverage.csv`: `80ee52bb0023b2b9cf45a2e30700c4ba0c345c815bb7ed6e3a0493c642b81f1e`
- `O:\repos.hobby\ntvdm64-wow32-audit\management\seam_registry.py`: `8b81e5b170680bec3887992a5523b4a02207ada2bcf02bf3cffed8978248a30a`
- `O:\repos.hobby\ntvdm64-wow32-audit\management\seam-scope.json`: `b5ac71f12855ea642f977d92139fb2ebb39210d03f8a0fde18b7c453ae415e47`
- `O:\repos.hobby\ntvdm64-wow32-audit\management\seam-frontier.csv`: `d2233d4b5640dba38f8371807c49abbd83f695c97421041886803cab867c636c`
- `O:\repos.hobby\ntvdm64-wow32-audit\management\seam-evidence-map.csv`: `b3ef20590506eb83937cf49473f19e5a0e5ddb3e2dbb7c0da351ed94175cd36d`
- `O:\repos.hobby\ntvdm64-wow32-audit\management\seam-evidence-links.json`: `8f2e5ae42262fe859c8e9933663c765b317b26ec431a3982187acd05cd990fe5`

## Meaning of the 42,870 pending entries

The registry inventory enumerates Git-tracked paths under configured MVDM and provider directory envelopes plus explicit dependency paths, at pinned original commits. It classifies external GDI device subtrees as device context; most other external paths as provider envelope. It explicitly states envelope membership is not proof of relevance or effective code and device expansion follows an actual host-seam dependency. Some content is only Git-blob indexed, not read or parsed; lexical candidates are not a compiler, effective call graph, executable-image or runtime proof. The 42,870 are 36,736 device-context rows and 6,134 host-envelope rows. They are a wide review envelope, not 42,870 verified dependencies or an obligation to import NT4 wholesale. Pending does not mean irrelevant either: promote only a specific proven reachable boundary/contract.

## The 93 named boundary records

These are source-audit classifications, not 93 independently proven direct calls from current MVDM, nor a claim that any whole external file is required in the product. Listed counts and paths preserve editions and recipes as separate records. The strongest explicitly identified direct frontier in this subset is the six original NTDLL loader files classified as callers of original WOW32 DLLENTRY; the physical files represent three files in each edition, not six independent current-product calls. Their modern loader ownership needs separate current-runtime evidence. The KnownDLL CRT/NLS group includes original symbol dependencies, downstream private-state/data, recipes and archive evidence: it is not possible from this classification to claim all 44 are direct MVDM callers. The multimedia boundary groups include exact WinMM callback/provider/task and selected mixer paths; driver, kernel notify and I/O rundown are downstream original chain dependencies and are not all direct MVDM frontiers. Native mixer client, conversion tool, candidate exclusion, and PE-image identity entries are evidence/context, not runtime imports. Each actual edge, owner, lifecycle and supported contract remains to be tied to its admission/graph evidence and current implementation; none of these labels alone proves guest behavior or current product linkage.

Counts and exact identities below enable independent rechecking against `source-coverage.csv`.

### actual_WOW32_DLL_loader_provider_dependency (6)

- `OpenNT` `base/ntdll/ldrsnap.c` — `SRC:1505c960fc87439b0234`
- `OpenNT` `base/ntdll/ldrapi.c` — `SRC:b96e21503f23c2b066ad`
- `OpenNT` `base/ntdll/ldrinit.c` — `SRC:0acb6cf5bf09e8b2751b`
- `OpenNT-4.5` `nt/private/ntos/dll/ldrsnap.c` — `SRC:6e673405a7ca6162279a`
- `OpenNT-4.5` `nt/private/ntos/dll/ldrapi.c` — `SRC:64fa4d96e1bc42a2b1bf`
- `OpenNT-4.5` `nt/private/ntos/dll/ldrinit.c` — `SRC:a6e13763e905ffd070a2`

### admitted_exact_driver_dependency (6)

- `OpenNT-4.5` `nt/private/windows/media/winmm/regapi.c` — `SRC:c9d73a41f6e99f24c488`
- `OpenNT-4.5` `nt/private/windows/media/winmm/drvrrare.c` — `SRC:697a5cb8532bfcb4ef38`
- `OpenNT-4.5` `nt/private/windows/media/winmm/drvr.c` — `SRC:a7b6058535cc789e2e20`
- `OpenNT-4.5` `nt/private/windows/media/winmm/drvr.h` — `SRC:f71bfc8dcdadad10564a`
- `OpenNT-4.5` `nt/private/windows/media/winmm/playsnd.c` — `SRC:f8019bbdf671713b816e`
- `OpenNT-4.5` `nt/private/windows/media/winmm/sources` — `SRC:770b16367ab76fcb38a8`

### admitted_exact_kernel_notify_dependency (8)

- `OpenNT-4.5` `nt/private/ntos/dd/sound/soundsys/sources` — `SRC:65adb1f516dafc8b4115`
- `OpenNT-4.5` `nt/private/ntos/dd/sound/soundsys/init.c` — `SRC:13efd78c0ab9a4bb3957`
- `OpenNT-4.5` `nt/private/ntos/dd/sound/soundsys/mixer.c` — `SRC:83005ca45a795f8b62ca`
- `OpenNT-4.5` `nt/private/ntos/dd/sound/soundlib/sources` — `SRC:ae51ccaf48d1dfb9f6b2`
- `OpenNT-4.5` `nt/private/ntos/dd/sound/soundlib/mixer.c` — `SRC:924797e0e01bf1c067a7`
- `OpenNT-4.5` `nt/private/ntos/dd/sound/soundlib/inc/mixer.h` — `SRC:5afb030290d77173dce4`
- `OpenNT-4.5` `nt/private/ntos/dd/sound/soundlib/dispatch.c` — `SRC:ebe4f9afc1212962457c`
- `OpenNT-4.5` `nt/private/ntos/dd/sound/soundlib/soundlib.c` — `SRC:ee5bdf58d7f7ea0d0d4f`

### admitted_exact_mixer_provider_profile (9)

- `OpenNT-4.5` `nt/private/windows/media/drivers/sndsys32/dll/oemsetup.inf` — `SRC:9aded9b31c5044abb5f8`
- `OpenNT-4.5` `nt/private/windows/media/drivers/sndsys32/dll/sources` — `SRC:d6547834ff1da2c6758f`
- `OpenNT-4.5` `nt/private/windows/media/drivers/sndsys32/dll/sndsys32.def` — `SRC:bd128b962679f9a476a7`
- `OpenNT-4.5` `nt/private/windows/media/drivers/sndsys32/dll/drvproc.c` — `SRC:c64fb8a345268250f42c`
- `OpenNT-4.5` `nt/private/windows/media/drivers/drvlib/sources` — `SRC:12ce285caa286e9d404a`
- `OpenNT-4.5` `nt/private/windows/media/drivers/drvlib/inc/drvlib.h` — `SRC:778b04f76a6738c6a7cf`
- `OpenNT-4.5` `nt/private/windows/media/drivers/drvlib/mixerdd.c` — `SRC:fb27eadcf5a05c5294b0`
- `OpenNT-4.5` `nt/private/windows/media/drivers/drvlib/drvlib.c` — `SRC:306d8a29b4e55d89a953`
- `OpenNT-4.5` `nt/private/windows/media/drivers/drvlib/drvutil.c` — `SRC:adc53c2a0d7273a93f85`

### admitted_exact_notify_writer_and_io_rundown (5)

- `OpenNT-4.5` `nt/private/ntos/dd/sound/soundlib/wave.c` — `SRC:40d465f760d41713133c`
- `OpenNT-4.5` `nt/private/ntos/dd/sound/soundlib/synth.c` — `SRC:519bc99780e746e080e8`
- `OpenNT-4.5` `nt/private/ntos/io/devctrl.c` — `SRC:04a074d392407360791b`
- `OpenNT-4.5` `nt/private/ntos/io/internal.c` — `SRC:8d0c0d6f2866de56c7f9`
- `OpenNT-4.5` `nt/private/ntos/io/iosubs.c` — `SRC:16e600460335f8c61184`

### bounded_startup_image_identity_evidence (2)

- `OpenNT-4.5` `nt/public/sdk/lib/i386/wow32.dll` — `SRC:7046e5a6b33e42c54a8c`
- `OpenNT-4.5` `nt/public/sdk/lib/i386/ntvdm.exe` — `SRC:ebfa6b28703f97420e8d`

### exact_callback_provider_dependency (1)

- `OpenNT-4.5` `nt/private/windows/media/winmm/mmwowcb.h` — `SRC:829a694d21ac1a82871c`

### exact_mixer_client_context (4)

- `OpenNT-4.5` `nt/private/windows/media/mixerapp/sources` — `SRC:17ff75876eb42e0d2150`
- `OpenNT-4.5` `nt/private/windows/media/mixerapp/mixer.c` — `SRC:955958fdec6203f2a278`
- `OpenNT-4.5` `nt/private/windows/media/mixerapp/volume.c` — `SRC:c2ffee79c892f5e5d54b`
- `OpenNT-4.5` `nt/private/windows/media/mixerapp/volumei.h` — `SRC:793986557d6b639719c5`

### exact_mixer_provider_dependency (2)

- `OpenNT-4.5` `nt/private/windows/media/winmm/mixer.h` — `SRC:10741ae5226fcad12a9c`
- `OpenNT-4.5` `nt/private/windows/media/winmm/util.c` — `SRC:20f31eafdee1762683e1`

### exact_original_dependency_only (3)

- `OpenNT-4.5` `nt/private/windows/media/winmm/mixer.c` — `SRC:b88fcdee5df5c11329d1`
- `OpenNT-4.5` `nt/private/sdktools/vctools/link/coff/cvtomf.h` — `SRC:c853622e042a3ca6cdaf`
- `OpenNT-4.5` `nt/private/sdktools/vctools/link/coff/cvtomf.cpp` — `SRC:18f9c08d04ecc3ead033`

### exact_task_notify_provider_dependency (1)

- `OpenNT-4.5` `nt/private/windows/media/winmm/task.c` — `SRC:b8162cc5e98da96c1853`

### selected_exact_dependency (44)

- `OpenNT-4.5` `nt/private/windows/winnls/mbcs.c` — `SRC:592446af0cffa0e1393a`
- `OpenNT-4.5` `nt/private/windows/winnls/init.c` — `SRC:4880a0472b4a9b009e70`
- `OpenNT-4.5` `nt/private/windows/winnls/nlstrans/alloc.c` — `SRC:5049816b7186806af0ba`
- `OpenNT-4.5` `nt/private/windows/winnls/nlstrans/codepage.c` — `SRC:c24cce8d439d70e3f842`
- `OpenNT-4.5` `nt/private/windows/winnls/data/1252.txt` — `SRC:dd8a27b05ccee33faf53`
- `OpenNT-4.5` `nt/private/windows/winnls/data/437.txt` — `SRC:3ca01880ce1c974481ce`
- `OpenNT` `base/crts/crtw32/string/strlwr.c` — `SRC:e12bc0bc25f81c4489f1`
- `OpenNT` `base/crts/crtw32/string/stricmp.c` — `SRC:7c530d14f7ee1f39690c`
- `OpenNT` `base/crts/crtw32/string/x86/stricmp.asm` — `SRC:35a7dcc91a6c6fb483bd`
- `OpenNT` `base/crts/crtw32/string/nt/sources` — `SRC:73c6249a84d1c7e9780e`
- `OpenNT` `base/crts/crtw32/string/sources.nt` — `SRC:e3e3274406a6bb054e63`
- `OpenNT` `base/crts/crtw32/crt32.nt` — `SRC:65a279830a861efc5345`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/string/strlwr.c` — `SRC:c9aff22d8e987752276f`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/string/stricmp.c` — `SRC:6db1c2a6700b36b89f42`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/string/i386/stricmp.asm` — `SRC:0f59317a459ab99d5e4d`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/string/nt/sources` — `SRC:9f67bc5271c69d4182a5`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/string/sources.nt` — `SRC:5e7ad6efd6bf4257d538`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/crt32.nt` — `SRC:297bb03d483905c327fc`
- `OpenNT` `base/crts/libw32/lib/sources.nt` — `SRC:adf68fcfaf72064602cb`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/libw32/lib/sources.nt` — `SRC:a8a0843575fe915811a4`
- `OpenNT` `public/sdk/lib/x86/libcntpr.lib` — `SRC:39c8197aff567fee2170`
- `OpenNT` `base/crts/crtw32/convert/tolower.c` — `SRC:ac541f46bee931e82556`
- `OpenNT` `base/crts/crtw32/h/ctype.h` — `SRC:815cd09a4f6d242624af`
- `OpenNT` `base/crts/crtw32/convert/isctype.c` — `SRC:85c76aa4fb2dbac0b334`
- `OpenNT` `base/crts/crtw32/misc/setlocal.c` — `SRC:6d7f30604745ce231f8f`
- `OpenNT` `base/crts/crtw32/misc/nlsdata2.c` — `SRC:5a6fa631570c6e2eeeaa`
- `OpenNT` `base/crts/crtw32/misc/nlsdata1.c` — `SRC:039d3e15660408e569ea`
- `OpenNT` `base/crts/crtw32/misc/ctype.c` — `SRC:dba8d3f890800fec2d7b`
- `OpenNT` `base/crts/crtw32/misc/nt/sources` — `SRC:41ec298ffe0efc3d8652`
- `OpenNT` `base/crts/crtw32/convert/nt/sources` — `SRC:9ed337c4d14e38f138c7`
- `OpenNT` `base/crts/crtw32/convert/sources.nt` — `SRC:1b54d71736d114679ba7`
- `OpenNT` `base/crts/crtw32/misc/initctyp.c` — `SRC:c563787fe717156b8a23`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/convert/tolower.c` — `SRC:6ed951dfb26e2d662b57`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/h/ctype.h` — `SRC:ac398655b226593a226c`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/convert/isctype.c` — `SRC:6e6676f861804f0c340e`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/misc/setlocal.c` — `SRC:b8d153e06705d4c309b3`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/misc/nlsdata2.c` — `SRC:e0cfefe90b3883347c49`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/misc/nlsdata1.c` — `SRC:61cfdb5186130d5fa487`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/misc/ctype.c` — `SRC:fc7c97f28ca7cf7ec94a`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/misc/nt/sources` — `SRC:df00b118e61987fb9544`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/convert/nt/sources` — `SRC:27c8cd3f62fcb61c427b`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/convert/sources.nt` — `SRC:15ff44dde639ffa22edd`
- `OpenNT-4.5` `nt/private/sdktools/vctools/crt/crtw32/misc/initctyp.c` — `SRC:4ff9c21b4ee04c9125a7`
- `OpenNT-4.5` `nt/public/sdk/lib/i386/libcntpr.lib` — `SRC:6799072c4b0727cc4fef`

### selected_provider_candidate_exclusion_only (2)

- `OpenNT-4.5` `nt/private/windows/media/mmdrv/sources` — `SRC:4d2c40fec90085c92137`
- `OpenNT-4.5` `nt/private/windows/media/mmdrv/mmdrv.def` — `SRC:909cf8cb90f7b852f069`

## Finite follow-through and limits

1. Start from identified original MVDM entry/call sites and exact function-level admission evidence; classify each edge as direct, transitive, recipe, negative candidate, or context. Resolve actual profile and ownership in the modern product before proposing source recovery.
2. For direct or necessary transitive contracts, compare existing product/host service, identity, lifecycle, error behavior and relevant tests; preserve missing evidence as pending. For native OS/driver internals, original use does not imply importing that subsystem; select a bounded host service or adaptation only with equivalent required observable semantics.
3. Do not infer whole-file functionality, active build membership or guest support from a source/recipe/image identity. No guest execution, product code changes, or ledger classification were performed in this review.
