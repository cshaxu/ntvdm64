# INI.EXE

Edits Windows guest configuration files from CMD. Candidate operations read
a source file and write a fresh output file. Callers place the candidate at
the final path only after the rest of their preparation succeeds.

```text
INI.EXE <source> <candidate> set <section> <key> <value>
INI.EXE <source> <candidate> append <section> <key> <value>
INI.EXE <source> <candidate> drop-file <section> <key> <filename>
INI.EXE <source> <candidate> install-files <section> <key> <on|off> <path> [...]
INI.EXE <source> <candidate> select-files <section> <key> <on|off> <path> [...]
INI.EXE <source> <candidate> remove-files <section> <key> <on|off> <path> [...]
```

The file operations match entries by case-insensitive filename in a single
section. Installation inserts the supplied order. Selection requires exactly
one existing entry for each filename and retains its position. Removal deletes
only matching entries. Unrelated bytes and line endings are preserved.

The existing network-profile and isolated test-driver modes remain available
to the test preparation commands. Build the shared binary with
`tools\build-addon.cmd ini`.
