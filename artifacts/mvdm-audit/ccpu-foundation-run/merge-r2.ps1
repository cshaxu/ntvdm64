$ErrorActionPreference='Stop'
Set-Location 'O:\repos.hobby\ntvdm64'
& python -B -X utf8 artifacts/mvdm-audit/ccpu-foundation-observer.py
if($LASTEXITCODE){exit $LASTEXITCODE}
& python -B -X utf8 artifacts/mvdm-audit/ccpu-foundation-role-merge.py
exit $LASTEXITCODE