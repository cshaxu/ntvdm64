# DOS kernel image and linker intermediate roles

The original NTIO.SYS and NTDOS.SYS match their source-tree, archive and
installed copies. Both are partial image roles: byte identity is not proof of
their source-to-image lineage or of complete DOS/BIOS runtime behavior.
NTDOS.BIN is the distinct original intermediate named by the DOS makefile's
relocation and STRIPZ steps; it is not packaged under that name. Its file role
can be excluded without excluding DOS kernel behavior.

The separate `bin86/country.sys` differs from the actual packaged country
image; it is not included in this checkpoint. The
[country review](../shard-2/dos-kernel-country-review.md) retains that identity
boundary. All DOS memory, file, localization, error and lifecycle obligations
remain in the relevant recovery stages. No guest was built or modified.
