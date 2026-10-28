# Win95 offline path utility

path95.c produces the AMD64 path95.exe. It discovers SystemRoot from a
supported CREG hive or prepares a fresh candidate with bounded path
substitutions and updated metadata. It uses public format documentation;
no libcreg implementation is linked or copied.

Build with BUILD.CMD output-exe in a repository build directory and publish
with tools/build/Stage-AddonUtilityRelease.ps1 -Path95Build. The
tools/win95-path scripts consume assets/release/path95.exe and own backup,
installation and recovery. This helper never joins a Run16 or worker runtime.
