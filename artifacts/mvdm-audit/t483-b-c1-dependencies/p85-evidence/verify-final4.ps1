$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$results=@()
try {
    foreach($case in @('window-borrow','graph-snapshot','native-creation','process-retirement','task-lifecycle','menu-resource')) {
        $started=[DateTimeOffset]::UtcNow
        & test/observation/verify-wow-task-lifecycle.ps1 -ProviderBuildRoot build/.ninja/msvc-x86-release/wow32 -WorkerBuildRoot build/.ninja/msvc-x86-release/worker -BuildRoot "$PSScriptRoot/final-binding-r2-$case" -Case $case
        $code=$LASTEXITCODE
        $results+=@{case=$case;exit=$code;started=$started.ToString('o');finished=[DateTimeOffset]::UtcNow.ToString('o')}
        $results | ConvertTo-Json | Set-Content "$PSScriptRoot/targeted-final-binding-results.json"
        if($code){throw "$case failed: $code"}
    }
    '0' | Set-Content "$PSScriptRoot/targeted-final2.exit"
} catch {
    Write-Output $_
    '1' | Set-Content "$PSScriptRoot/targeted-final2.exit"
    exit 1
}
