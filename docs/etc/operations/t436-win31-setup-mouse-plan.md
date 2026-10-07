# T436 sequence — 386 startup research, mouse driver, setup and patch package

## Owner request and admission condition

Owner approves a Windows 1.01-shaped installation/add-on package and adds:
“还要提供 winstd.cmd, winstd.pif, win386.cmd, win386.pif 供使用。
你把这个加入一个新的S任务，等本次386模式调研启动成功后就可以准入”。

Owner's subsequent governance direction on 2026-10-07 supersedes the earlier
combined-S2 plan: “当前的386调研作为S1准入；S2要把鼠标驱动实现好；
S3要把安装包补丁搞定，都完成以后交给我验收。”

Owner now explicitly accepts unstable enhanced-mode execution and admits S2
mouse work, followed by S3 installer patches. This supersedes the stable
startup prerequisite, not the obligation to report failed replays accurately.
S1 concludes as limited research: actual /3 desktop/Notepad observations,
checked retail-copy SFT adaptation, unresolved startup reliability and no
normal-shutdown/general-compatibility claim.

Use **T436 S1 → S2 → S3**. S2 is concluded; S3 remains a planned successor,
not a concurrent admission. Admit S3 separately before installation work.
S2's enhanced-mode input tests distinguish
startup failure from driver failure; accepted instability is not a passing
mouse test. Standard-mode verification remains required.

## Stage boundaries

| Stage | Scope and completion boundary |
| --- | --- |
| S1 — accepted limited 386 research | Owner accepts actual enhanced desktop/application observations with unresolved independent startup failures. Preserve checked retail-copy adaptation, original standard baseline and explicit limits; no stable/general-compatibility or mouse/installer claim. |
| S2 — mouse driver | Audit the Win3.1 ABI, implement/build the independent NTVDM mouse adaptation, verify real input, movement/clicks/show-hide and restart/cleanup in standard and enhanced modes. Deliver driver source, artifact identity and reproducible tests; packaging waits for S3. |
| S3 — installer/patch package | Consolidate the accepted startup adaptations and completed driver into original Setup plus PATCH, generate both explicit mode entrypoints and isolated installed profiles, validate fresh installation and package independence, publish the hand-test package. |

Each stage follows existing build/test/review/commit/push rules. Stage delivery
does not imply final owner acceptance of T436. After all three stages finish,
give the owner the integrated package, commands, evidence and known limits for
one final acceptance round. Do not close T436 before that acceptance.

## Deliverables and ownership

Owner adds both compiled independent mouse drivers to assets/release with a
separate add-on identity manifest, leaving the ten-host-image manifest intact.
S2 delivers these artifacts and Win1.01's interactive apply-setup entrypoint;
S3 delivers Win3.1's equivalent. Each accepts the actual original-media path,
copies the release driver plus authored installer additions into media/PATCH,
and preserves original media. Installed profiles/runtime add-ons belong to
installed/PATCH and must not depend on the media/repository after installation.
The Win1.01 accepted original-Setup workflow is retained, not reimplemented.

- **S3** `tools/win31-setup`: original Setup orchestration, packaging, checked
  installation of adaptations accepted from S1, PIF/config generation and
  launch templates/readmes. Mirror the owner-approved tools/win101-setup home;
  independently authored mouse-driver sources alone remain under src/addon.
- **S2** `src/addon/win31-mouse-drv`: independently authored Windows 3.1 mouse
  adaptation to NTVDM's existing mouse capability. Audit the actual Win3.1
  driver ABI and standard/enhanced mode boundaries first; do not rename or
  blindly copy the Windows 1.01 driver as a substitute.
- **S3** review and fold the applicable existing `win31-launch` implementation into
  `win31-setup`; remove displaced duplicate paths and update their callers.
  Existing references/evidence are retained, not silently rewritten as passes.
- **S3** `O:/w31setup`: unchanged original installation media at the root; all authored
  driver/script/template/patch additions under `PATCH`. Keep the package
  independent of repository/build directories.
- **S3** installed `<installation-directory>/PATCH`: self-contained launchers, PIFs,
  config/autoexec and necessary add-on runtime files. It must keep working
  after the setup package and its temporary work are removed.

## Required launch interface

| Installed file | Contract |
| --- | --- |
| `winstd.cmd` | Resolve plain run16 through PATH, invoke the adjacent winstd.pif, preserve the actual result. |
| `winstd.pif` | WIN.COM standard mode `/S`, with installed CWD and installed dedicated config/autoexec paths. |
| `win386.cmd` | Resolve plain run16 through PATH, invoke the adjacent win386.pif, preserve the actual result. |
| `win386.pif` | WIN.COM forced 386-enhanced mode `/3`, using the configuration and accepted adaptations proven by S1. |

Retain mode arguments in both relevant original PIF fields and validate the
extension chain/checksum. Do not silently fall back from /3 to standard mode.
Share config/profile generation where semantics match. If the proven modes
require different settings, use explicitly selected installed mode profiles,
not edits to default product config. No ambiguous win.cmd/start.cmd alias is
required; the requested two explicit entrypoints are the product interface.

## Installation and safety contract

- Run setup.cmd without a mandatory preselected destination, choose it in
  original Setup, then confirm the actual installed directory for postconfiguration.
  Keep the final output visible with pause and preserve the real exit status.
- A nonzero Setup/run16 return remains a failure result. Owner-confirmed
  completed installation may receive validated postconfiguration without
  pretending that the original return was success; blank confirmation skips it.
- Derive the package path from its own script directory; require explicit
  packaging inputs and the actual user-selected destination. No hardcoded
  executable/install/media paths, drive substitution or temporary mappings.
- Use installation-local CONFIG.NT/AUTOEXEC.NT and mode-specific equivalents
  only where necessary. Never modify default O:/winnt/system32 profiles.
  Preserve the proven retail DOSX choice; do not mask DOSX generically.
- Remove temporary work on normal success/failure; do not ship WORK, build
  outputs or transient logs as installation media. Preserve unexpected preexisting
  work/user files rather than deleting them without ownership proof.
- Repair or remove copied temporary PIF/config references through owned
  postconfiguration; no installed file may refer back to the setup WORK/package.
- Only carry forward already approved and validated retail adaptations with
  exact input hashes, before-byte/range checks, provenance and recovery.
  Original OpenNT/NT DOS and other original media remain under source policy.
- No helper/host component, production-protocol change or raw-volume permission
  workaround. Direct-disk warnings are a separate compatibility issue, not
  something this mouse driver or installer can silently claim repaired.

## Stage-specific verification and final handoff

1. **S1** freeze the actual mode/config/retail adaptation identities and prove
   current enhanced startup by runtime evidence, not only source or a build.
   Record independent failures/limits without retries-to-success acceptance.
2. **S2** audit and implement the Win3.1 mouse ABI against real NTVDM input, preserving
   original device semantics and avoiding competing/double input consumers.
   Validate standard and enhanced modes separately, including movement,
   clicks, show/hide and restart/cleanup; input submission alone is not success.
3. **S2** build the driver in its guest toolchain, verify its ABI/artifact
   identity and real provider path. Deliver reproducible driver tests before S3.
4. **S3** prepare original media plus PATCH using only accepted S1 adaptations
   and the completed S2 driver. Run original Setup; validate all four installed
   launch files and profiles without setup-directory dependencies.
5. **S3** exercise /S and /3 through the production run16/NTVDM/frontend chain with
   actual mode evidence and usable desktop/application/mouse interaction.
   Record exit failures and unsupported boundaries honestly; do not use Ignore,
   reduced assertions or retries to manufacture acceptance.
6. **S3** prove no default-profile/immutable-media changes, no package-directory
   dependency and preservation of the accepted standard/enhanced frontiers.
   Include applicable existing regression/publication gates, reviewed evidence,
   commit/push and owner side-test package. If host product code changes,
   synchronize the latest coherent ten images in assets/release under EXECUTION.

7. **Final owner acceptance** after S1/S2/S3 all conclude: hand off O:/w31setup
   and the installed standard/enhanced entrypoints, exact verification results
   and unresolved boundaries. T436 remains open until the owner's verdict.

The exact S2 and S3 packets/tests are frozen at their sequential admissions,
using the then-verified predecessor inputs. This record claims no future
implementation or successful Windows/mouse/installation capability.
