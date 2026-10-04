param(
    [string]$Cache='build/M0-T427/S2/r001',
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$Environment='build/M0-T427/S4/r049/msvc-x86.cmd'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$cachePath=(Resolve-Path $Cache).Path
$environmentPath=(Resolve-Path $Environment).Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
    throw 'Fresh repository build root required'
}
$null=New-Item -ItemType Directory -Path $root
$graph=Get-Content (Join-Path $cachePath 'build.ninja')
$flags=@($graph | Where-Object {$_ -match '^cflags = '})
if($flags.Count -ne 1 -or $flags[0] -match 'MVDM_CCPU_DECODE_DIAGNOSTICS'){
    throw 'Normal CCPU flags missing, ambiguous or diagnostic-enabled'
}
$arguments=@([regex]::Matches(($flags[0] -replace '^cflags = ','').Replace('$:',':'),'"[^"]*"|\S+') |
    ForEach-Object {$_.Value.Trim('"')})
$diagnostic=Join-Path $root 'c_main-diagnostic.obj'
$source=Join-Path $repo 'src/mvdm/softpc.new/base/ccpu386/c_main.c'
$current=Get-Content $source -Raw
$prior=(& git -C $repo show '68e860553:src/mvdm/softpc.new/base/ccpu386/c_main.c') -join "`n"
if($LASTEXITCODE){throw 'Accepted source comparison unavailable'}
$oldBlock=[regex]::Match($prior,'(?s)DECODE:\s*#ifdef NTVDM(?<body>.*?)#endif')
$newBlock=[regex]::Match($current,'(?s)DECODE:\s*/\* DIVERGENCE: MVDM-HOST-DIV-325\..*?\*/\s*#if defined\(NTVDM\) && defined\(MVDM_CCPU_DECODE_DIAGNOSTICS\)(?<body>.*?)#endif')
if(!$oldBlock.Success -or !$newBlock.Success -or
    $oldBlock.Groups['body'].Value.Replace("`r",'') -ne $newBlock.Groups['body'].Value.Replace("`r",'')){
    throw 'Diagnostic attribution body changed or compile-selection shape wrong'
}
& $environmentPath cl.exe @arguments /DMVDM_CCPU_DECODE_DIAGNOSTICS "/Fo$diagnostic" $source *> (Join-Path $root 'diagnostic-build.txt')
if($LASTEXITCODE){throw 'Diagnostic object compile failed'}
$normal=Join-Path $cachePath 'obj/ccpu/c_main.obj'
$symbols=@('mvdm_softpc_report_nt_transition','mvdm_softpc_report_wow_allocsel_instruction',
    'mvdm_softpc_report_wow_setdescriptor_instruction','mvdm_softpc_report_wow_getsel_instruction',
    'mvdm_softpc_report_wow_longptradd_instruction')
foreach($selection in @(@{Name='normal';Path=$normal},@{Name='diagnostic';Path=$diagnostic})){
    $output=& $environmentPath dumpbin.exe /symbols $selection.Path
    if($LASTEXITCODE){throw 'Object symbol inspection failed'}
    $output | Set-Content (Join-Path $root ($selection.Name+'-symbols.txt'))
    $text=$output -join "`n"
    foreach($symbol in $symbols){
        $present=$text -match ('UNDEF[^\r\n]*\b_'+[regex]::Escape($symbol)+'\b')
        if($present -ne ($selection.Name -eq 'diagnostic')){
            throw "Unexpected $($selection.Name) object reference: $symbol"
        }
    }
}
[ordered]@{
    Profile='Selected normal MSVC x86 /MT CCPU40 flags; diagnostic differs only by explicit macro'
    Source=(Get-FileHash $source).Hash;Graph=(Get-FileHash (Join-Path $cachePath 'build.ninja')).Hash
    Normal=(Get-FileHash $normal).Hash;Diagnostic=(Get-FileHash $diagnostic).Hash
    Flags=$arguments;Symbols=$symbols;NormalReferences=0;DiagnosticReferences=5
    DiagnosticBody='Identical after newline normalization to accepted68e860553'
    CompilerEnvironment=@{CL=$env:CL;_CL_=$env:_CL_}
}|ConvertTo-Json -Depth 5 | Set-Content (Join-Path $root 'manifest.json')
Write-Output 'PASS normal object excludes all five DECODE observers; explicit diagnostic object retains all five'
