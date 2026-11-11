[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86')]
    [string]$Architecture,
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = ''
)

throw 'Retired T359 SAS-store observation generator: its configuration-command fixture no longer exists. No build or passing result is produced.'
