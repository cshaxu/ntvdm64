# GRP add-on utility

`GRP.EXE` is the project-authored AMD64 parser/editor for supported Windows 3.x
`PMCC` Program Manager `.GRP` files. It owns only structural parsing,
root-prefix replacement, pointer relocation, and the documented 16-bit group
checksum. It never selects files, creates backups, or owns recovery; callers
must do those operations in CMD before invoking it.
