# DOS core and device historical source roles — shard 0

The [55-path table](dos-core-guest-owner-review.csv) combines 54 byte-exact
primary-original source/include roles and one independently verified
supplemental ANSI message input. The [input receipt](dos-core-guest-owner-inputs.json)
retains source paths/hashes for the original recipe and literal include
traversal. Newline-only differences encountered during traversal are not
promoted as byte-exact exclusions.

Original link inputs are specific: NTDOS uses `msdos.lnk`; NTIO uses
`msbio_4.lnk`, not the adjacent `msbio.lnk`; ANSI and keyboard use their own
response files. COUNTRY directly links `country.obj`; HIMEM explicitly links
its six named objects; REDIR's named OBJS use the original MASM inference
rule and inline link16 response. Object/source rules, link membership and
direct includes are retained per row. `dos/v86/make.inc` supplies the common
MASM/CL16 and `.exs.sys` relocator rules; these tools were not executed.

The reader ignores MASM COMMENT blocks and records only literal include
edges; conditional includes remain conditional. Shared `sysmsg.inc:389`
includes `MSGSERV.ASM` inside `MSG_UTILNAME`. ANSI's parser actually invokes
that macro at line 59, so the path is not justified merely by an unused
macro definition. Its `MSG_SERVICES <ANSI.CL1>` at line 88 invokes the
original `$M_INC` include machinery. The existing CL1 artifact matches the
approved OpenNT-src-2 original and historical T301 record; it is not assigned
an invented primary-edition identity.

Only pending paths under `dos/v86` are proposed. Reached shared
`inc/DBGSVC.INC` and `inc/RDRSVC.INC` remain outside this batch for their own
host/guest ABI-owner review. Unsupported or commented keyboard variants,
unproved C headers, existing binaries/libraries, source derivation and guest
runtime behavior remain independently open. No guest or product is changed.
