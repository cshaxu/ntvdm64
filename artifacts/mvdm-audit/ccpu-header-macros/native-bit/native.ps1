$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$root=Join-Path $repo 'build/t473-s10/ccpu-bit-memory'
$driver=Join-Path $repo 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
$command='@echo off'+"`r`n"+'call "'+$driver+'" cl.exe /nologo /MT /W4 /WX /O2 /Fo"'+$root+'/oracle.obj" /Fe"'+$root+'/oracle.exe" "'+$repo+'/tests/mvdm-host/ccpu_bit_memory_oracle.c"'+"`r`n"+'exit /b %errorlevel%'+"`r`n"
[IO.File]::WriteAllText("$root/build.cmd",$command,[Text.Encoding]::ASCII)
foreach($case in @('build','oracle')){
 $image=if($case -eq 'build'){$env:ComSpec}else{"$root/oracle.exe"}
 $start=@{FilePath=$image;WindowStyle='Hidden';PassThru=$true;WorkingDirectory=$root;RedirectStandardOutput="$root/$case.stdout.txt";RedirectStandardError="$root/$case.stderr.txt"}
 if($case -eq 'build'){$start.ArgumentList='/d /s /c ""'+$root+'/build.cmd""'}
 $p=Start-Process @start
 try{
  @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$image;arguments=$start.ArgumentList;diagnostic_budget_seconds=60}|ConvertTo-Json|Set-Content "$root/$case.process.json"
  if(!$p.WaitForExit(60000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned hardware observation budget exceeded'}
  $p.WaitForExit();$code=$p.ExitCode
  @{exit=$code;finished=$p.ExitTime.ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/$case.completion.json"
  if($code){throw "$case did not meet the independent expectation"}
 }finally{$p.Dispose()}
}
Get-Content "$root/oracle.stdout.txt"