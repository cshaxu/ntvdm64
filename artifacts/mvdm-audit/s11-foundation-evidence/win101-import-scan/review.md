# Windows 1.01 WIN87EM witness census

The read-only NE module-reference scan of `O:/W101` found 46 NE EXE/DLL/DRV
images, no parser errors, and no static module name `WIN87EM`. The same parser
positively identified WIN87EM in the independently hash-identified Win3.1
`O:/w31/CALC.EXE`. The paths, hashes, import names and control are in the JSON;
the script is retained for inspection. No executable or guest file was changed.

This installation therefore provides no static-import witness for the
pre-Windows-3.0 special loader branch under investigation. The scan does not
cover dynamically constructed names, execution, other media, or whether
Windows 1.x programs encode the later NE expected-version field in the same
way; it does not claim such compatibility. The actual pre-3.0 WIN87EM loader
path remains unverified.
