param([Parameter(Mandatory)][string]$Image,[Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory -Path $root|Out-Null
$assembly=Join-Path $root 'Classification.dll'
Add-Type -OutputAssembly $assembly -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class ImageClassification {
 [DllImport("kernel32.dll",CharSet=CharSet.Unicode,SetLastError=true)]
 public static extern bool GetBinaryType(string name,out uint type);
 [DllImport("kernel32.dll",CharSet=CharSet.Unicode,SetLastError=true)]
 public static extern IntPtr CreateFile(string name,uint access,uint share,IntPtr security,uint creation,uint flags,IntPtr template);
 [DllImport("ntdll.dll")]
 public static extern int NtCreateSection(out IntPtr section,uint access,IntPtr attrs,IntPtr size,uint protect,uint flags,IntPtr file);
 [DllImport("kernel32.dll")]
 public static extern bool CloseHandle(IntPtr handle);
}
'@
Add-Type -Path $assembly
$type=0;$ok=[ImageClassification]::GetBinaryType($Image,[ref]$type)
$errorCode=if($ok){$null}else{[Runtime.InteropServices.Marshal]::GetLastWin32Error()}
$file=[ImageClassification]::CreateFile($Image,2684354560,7,[IntPtr]::Zero,3,0,[IntPtr]::Zero)
if($file -eq [IntPtr](-1)){throw 'Image open failed'}
$section=[IntPtr]::Zero
try {$status=[ImageClassification]::NtCreateSection([ref]$section,1,[IntPtr]::Zero,[IntPtr]::Zero,0x10,0x1000000,$file)}
finally {if($section -ne [IntPtr]::Zero){[void][ImageClassification]::CloseHandle($section)};[void][ImageClassification]::CloseHandle($file)}
[ordered]@{image=$Image;getBinaryTypeSuccess=$ok;type=$type;error=$errorCode;sectionStatus=('0x{0:X8}' -f $status)}|ConvertTo-Json
