# M0 T310 S8 P4 — CCPU EFLAGS contract closure

\`c_bsic.c\` owns the original \`c_getEFLAGS\` function and returns an \`IU32\`
Intel flag word. Its own selected \`c_bsic.h\` had no declaration, so
\`c_intr.c\` and \`pushf.c\` relied on a modern compiler's implicit host \`int\`.

\`MVDM-HOST-DIV-071\` restores only the exact fixed-width declaration in the
same CCPU header. It does not alter flag storage, instruction order, CCPU
control flow, or introduce any adapter or mapping-manager path.

Clean formal Ninja rebuilds of the defining object and both call sites
(\`c_bsic\`, \`c_intr\`, \`pushf\`) succeed for x64 and x86 without either historic
implicit-call warning.
