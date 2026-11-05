[CmdletBinding()]
param(
    [ValidateSet('x86', 'x64')][string]$Architecture,
    [string]$RepositoryRoot = ''
)
# T290's retired Bochs adapter and fixture no longer exist in this tree.
# Keep the historical matrix visibly incomplete instead of producing a graph
# that selects an absent C++ input. The original script is indexed in S5.
throw 'Retired T290 disabled-ingress graph: historical adapter and fixture are absent.'