Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public static class UserAtomInventory {
 [DllImport("user32.dll",CharSet=CharSet.Unicode,SetLastError=true)] public static extern int GetClipboardFormatNameW(uint id,StringBuilder value,int size);
}
"@
$count=0; $dll=0; $pathlike=0; $probe=0; $user32=0; $kernel=0; $b=[Text.StringBuilder]::new(260)
for($i=0xc000;$i -le 0xffff;$i++) { [void]$b.Clear(); if([UserAtomInventory]::GetClipboardFormatNameW([uint32]$i,$b,260)) {$count++; $v=$b.ToString(); if($v.EndsWith('.dll',[StringComparison]::OrdinalIgnoreCase)){$dll++}; if($v.Contains('\') -or $v.Contains('/')){$pathlike++}; if($v.EndsWith('mouse-hook.dll',[StringComparison]::OrdinalIgnoreCase)){$probe++}; if($v.EndsWith('\user32.dll',[StringComparison]::OrdinalIgnoreCase)){$user32++}; if($v.EndsWith('\kernel32.dll',[StringComparison]::OrdinalIgnoreCase)){$kernel++}} }
"user_lookup_count=$count dll_suffix_count=$dll pathlike_count=$pathlike probe_name_count=$probe user32_name_count=$user32 kernel32_name_count=$kernel slots=16384 scope=user_name_lookup_not_hook_table"
