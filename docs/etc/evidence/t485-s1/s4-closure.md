# T485 S4 closure

H023 through H036 are delivered. Cumulative migration progress is 36/159 batches.

H035 and H036 each contained one old MIPS/PPC sources selector. The fixed XP tree has neither directory. Their declarations already exist in the imported sources.inc, and no product build reads the old files. Both redundant files were removed. Cached x64/x86 build passed in build/t485-s4-h034/selector60-build.log with no product work. All seven x86 host inputs matched the published manifest's input hashes; both release locations matched all thirteen published image hashes.

The final package remains 02D4C396-7B6E-4E71-8284-47F4D8DEB69A. Its Full checkpoint passed 23/23, including all three guest desktop gates. The real serial warning Ignore and Terminate actions, DPMI ownership tests, WOW output and XMS checks are recorded in the [H034 receipt](h034.md) and [startup repair evidence](h033-startup-repair.md). Win95 exit one remains the previously accepted boundary.

The selector-only cleanup changes no executable or guest input; the identical package retains that verification. Rollback for the selector deletion is 14aed3a07. The next admitted S5 covers N001 through N012 in inventory order. Independent new XP components remain outside this T.