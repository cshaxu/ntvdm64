# Exercise the runner's actual version-negative block without launching any
# product, compiler, helper, or global RPC endpoint.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$tokens=$null;$errors=$null
$ast=[Management.Automation.Language.Parser]::ParseFile(
    (Join-Path $repo 'tools/audit/Invoke-ProductVerification.ps1'),[ref]$tokens,[ref]$errors)
if($errors.Count){throw 'Runner parse failed'}
$calls=@($ast.FindAll({param($node)
    $node -is [Management.Automation.Language.CommandAst] -and
    $node.GetCommandName() -eq 'Invoke-Gate' -and
    $node.CommandElements.Count -eq 3 -and
    $node.CommandElements[1].Value -eq 'version-negatives'
},$true))
if($calls.Count -ne 1){throw 'Require exactly one version-negative gate'}
$body=$calls[0].CommandElements[2].ScriptBlock.GetScriptBlock()
$names=@('OPENNT_BROKER_PRODUCT_BUILD','OPENNT_VERSION_TEST_BUILD',
    'OPENNT_VERSION_TEST_LOGS','OPENNT_VERSION_TEST_RUNTIME')
$saved=@{};foreach($name in $names){$saved[$name]=[Environment]::GetEnvironmentVariable($name,'Process')}
$savedExit=$global:LASTEXITCODE
$cache='fixture-cache';$log='fixture-log';$Node='Test-VersionNode'
$script:checked=0
function Test-VersionNode([string]$path) {
    if($path -ne "$repo/tools/audit/Verify-ProductVersions.mjs" -or
       $env:OPENNT_BROKER_PRODUCT_BUILD -ne $cache -or
       $env:OPENNT_VERSION_TEST_BUILD -ne "$log/version-negative" -or
       $env:OPENNT_VERSION_TEST_LOGS -ne "$log/version-negative/logs" -or
       $env:OPENNT_VERSION_TEST_RUNTIME -ne 'Z:\'){throw 'Negative gate configuration missing'}
    ++$script:checked
    if($script:mode -eq 'exception'){throw 'fixture-node-exception'}
    $global:LASTEXITCODE=if($script:mode -eq 'nonzero'){17}else{0}
}
try {
    foreach($initial in 'absent','present') {
        foreach($script:mode in 'success','nonzero','exception') {
            foreach($name in $names){
                if($initial -eq 'absent'){Remove-Item ('Env:'+$name) -ErrorAction SilentlyContinue}
                else{[Environment]::SetEnvironmentVariable($name,"incoming-$name",'Process')}
            }
            $caught=$null
            try {& $body}catch{$caught=$_.Exception.Message}
            if($script:mode -eq 'success' -and $caught){throw $caught}
            if($script:mode -eq 'nonzero' -and $caught -ne 'Version negative gate failed'){throw 'Nonzero result was not propagated'}
            if($script:mode -eq 'exception' -and $caught -ne 'fixture-node-exception'){throw 'Exception was not propagated'}
            foreach($name in $names){
                $value=[Environment]::GetEnvironmentVariable($name,'Process')
                if(($initial -eq 'absent' -and $null -ne $value) -or
                   ($initial -eq 'present' -and $value -ne "incoming-$name")){throw "Environment restoration failed: $initial/$script:mode/$name"}
            }
        }
    }
    if($script:checked -ne 6){throw 'Not every branch reached the negative-node boundary'}
    'PASS actual runner version-negative environment: absent/present x success/nonzero/exception; failure propagation retained'
}finally {
    foreach($name in $names){
        if($null -eq $saved[$name]){Remove-Item ('Env:'+$name) -ErrorAction SilentlyContinue}
        else{[Environment]::SetEnvironmentVariable($name,$saved[$name],'Process')}
    }
    $global:LASTEXITCODE=$savedExit
}
