param([Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$Python)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)) {
    throw 'Fresh build-owned output required'
}
$source=Join-Path $repo 'src/addon/mouse31/mouse31.asm'
$assembler=(Get-Command nasm.exe -ErrorAction Stop).Source
$pythonPath=(Resolve-Path -LiteralPath $Python).Path
New-Item -ItemType Directory -Path $root|Out-Null
& $assembler -f bin $source -o "$root/MOUSE31.DRV" -l "$root/mouse31.lst"
if($LASTEXITCODE){throw 'Guest mouse assembly failed'}
$include=(Split-Path $source).Replace('\','/')+'/'
& $assembler -f bin "-I$include" "$PSScriptRoot/win31_mouse_driver_guest.asm" `
    -o "$root/M31TEST.COM" -l "$root/mock.lst"
if($LASTEXITCODE){throw 'Mock guest assembly failed'}
& $assembler -f bin '-DMOUSE31_TEST_REENTRY' "-I$include" "$PSScriptRoot/win31_mouse_driver_guest.asm" `
    -o "$root/M31REENT.COM" -l "$root/reentry.lst"
if($LASTEXITCODE){throw 'Reentrant mock guest assembly failed'}
& $assembler -f bin '-DMOUSE31_TEST_PRIOR_PM' "-I$include" "$PSScriptRoot/win31_mouse_driver_guest.asm" `
    -o "$root/M31PM.COM" -l "$root/prior-pm.lst"
if($LASTEXITCODE){throw 'Prior protected callback guest assembly failed'}
& $assembler -f bin "-I$include" "$PSScriptRoot/win31_mouse_driver_real.asm" `
    -o "$root/M31REAL.COM" -l "$root/real.lst"
if($LASTEXITCODE){throw 'Real protected-mode guest assembly failed'}
& $assembler -f bin "-I$include" "$PSScriptRoot/win31_mouse_callback_context.asm" `
    -o "$root/M31CTX.COM" -l "$root/context.lst"
if($LASTEXITCODE){throw 'Callback context guest assembly failed'}
& $assembler -f bin "$PSScriptRoot/win31_mouse_windows_probe.asm" `
    -o "$root/M31WIN.EXE" -l "$root/windows-probe.lst"
if($LASTEXITCODE){throw 'Win16 mouse runtime probe assembly failed'}
& $assembler -f bin "$PSScriptRoot/win31_mouse_swap_probe.asm" `
    -o "$root/M31SWAP.EXE" -l "$root/swap-probe.lst"
if($LASTEXITCODE){throw 'Win16 mouse exchange probe assembly failed'}
& $assembler -f bin "$PSScriptRoot/win31_mouse_dosalloc_probe.asm" `
    -o "$root/M31ALOC.EXE" -l "$root/dosalloc-probe.lst"
if($LASTEXITCODE){throw 'Win16 DOS allocation probe assembly failed'}
& $assembler -f bin '-DMOUSE31_VM_CHILD' "$PSScriptRoot/win31_mouse_dosalloc_probe.asm" `
    -o "$root/M31VM.EXE" -l "$root/vm-visibility-probe.lst"
if($LASTEXITCODE){throw 'Win16 VM-visibility parent assembly failed'}
& $assembler -f bin '-DMOUSE31_VM_CHILD' '-DMOUSE31_VM_LAUNCH_CONTROL' "$PSScriptRoot/win31_mouse_dosalloc_probe.asm" `
    -o "$root/M31VMCTL.EXE" -l "$root/vm-launch-control.lst"
if($LASTEXITCODE){throw 'Win16 VM-launch control assembly failed'}
& $assembler -f bin "$PSScriptRoot/win31_mouse_vm_child.asm" `
    -o "$root/M31CHLD.COM" -l "$root/vm-visibility-child.lst"
if($LASTEXITCODE){throw 'DOS VM-visibility child assembly failed'}
& $assembler -f bin "$PSScriptRoot/win31_vxd_trace_probe.asm" `
    -o "$root/M31VXD.COM" -l "$root/vxd-trace-probe.lst"
if($LASTEXITCODE){throw 'VxD trace transport probe assembly failed'}
& $assembler -f bin "$PSScriptRoot/win31_mouse_prior_owner_probe.asm" `
    -o "$root/M31OWNR.EXE" -l "$root/prior-owner.lst"
if($LASTEXITCODE){throw 'Win16 prior mouse owner probe assembly failed'}
& $assembler -f bin '-DMOUSE31_OWNER_CONTROL' "$PSScriptRoot/win31_mouse_prior_owner_probe.asm" `
    -o "$root/M31CTRL.EXE" -l "$root/prior-owner-control.lst"
if($LASTEXITCODE){throw 'Win16 prior mouse owner control assembly failed'}
& $pythonPath "$PSScriptRoot/audit-win31-mouse.py" --driver "$root/MOUSE31.DRV" --output "$root/contract.json" | Out-Null
if($LASTEXITCODE){throw 'NE audit failed'}
$audit=Get-Content "$root/contract.json" -Raw|ConvertFrom-Json
$expected=@(1,2,3,4,8)
if($audit.neVersion -ne '0x30a' -or $audit.automaticDataSegment -ne 2 -or
   $audit.flags -ne '0x8309' -or $audit.imports.Count -ne 0 -or $audit.entries.Count -ne 5) {
    throw 'Wrong protected driver/module ABI'
}
for($i=0;$i -lt $expected.Count;$i++) {
    if($audit.entries[$i].ordinal -ne $expected[$i] -or $audit.entries[$i].segment -ne 1) {
        throw 'Wrong Windows entrypoint'
    }
}
if(!@($audit.residentNames|Where-Object {$_.name -eq 'WEP' -and $_.ordinal -eq 8}).Count) {
    throw 'Missing named Windows cleanup export'
}
if($audit.segments[0].flags -ne '0x40' -or $audit.segments[1].flags -ne '0x41') {
    throw 'Callbacks/data must be fixed and resident'
}
[ordered]@{role='independent-guest-driver-build-not-runtime-pass';source=$source;
    sourceSha256=(Get-FileHash $source).Hash;assembler=$assembler;
    assemblerVersion=(& $assembler -v);driverSha256=(Get-FileHash "$root/MOUSE31.DRV").Hash;
    contract=$audit} |ConvertTo-Json -Depth 8|Set-Content "$root/build.json"
'PASS guest assembly and fixed/resident Win3.1 NE ABI; callback/provider/Windows tests remain required'
