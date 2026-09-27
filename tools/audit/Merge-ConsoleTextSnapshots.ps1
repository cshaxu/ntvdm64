function Merge-ConsoleTextSnapshots {
    param([Parameter(Mandatory)][string[]]$Snapshots)
    $lines = [Collections.Generic.List[string]]::new()
    foreach ($snapshot in $Snapshots) {
        if ($snapshot -match '(?m)^# capture-error=') { throw 'Console snapshot capture failed' }
        $current = @([regex]::Matches($snapshot, '(?m)^\[\d+\] ?(.*?)\r?$') |
            ForEach-Object { $_.Groups[1].Value })
        if (!$current.Count) { continue }
        $overlap = [Math]::Min($lines.Count, $current.Count)
        while ($overlap -gt 0) {
            $matches = $true
            for ($index = 0; $index -lt $overlap; ++$index) {
                $prior=$lines[$lines.Count - $overlap + $index]
                # Only the last live row may grow as the next command is
                # typed. Never rewrite an earlier completed output row.
                if ($prior -cne $current[$index] -and !($index -eq $overlap-1 -and
                    $current[$index].StartsWith($prior,[StringComparison]::Ordinal))) {
                    $matches = $false; break
                }
            }
            if ($matches) { break }
            --$overlap
        }
        # Do not turn unrelated/repainted frames into fabricated output history.
        if ($lines.Count -and !$overlap) { throw 'Console snapshots have no contiguous text overlap' }
        if ($overlap) { $lines[$lines.Count-1]=$current[$overlap-1] }
        for ($index = $overlap; $index -lt $current.Count; ++$index) { $lines.Add($current[$index]) }
    }
    $rendered = @(for ($index = 0; $index -lt $lines.Count; ++$index) {
        '[{0}] {1}' -f $index, $lines[$index]
    })
    return ($rendered -join "`r`n")
}
