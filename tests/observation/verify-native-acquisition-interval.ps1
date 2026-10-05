# Source-contract check; runtime presentation/handoff tests remain separate.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$source=Get-Content "$repo/src/ntvwm-exe/main.c" -Raw
if($source -notmatch 'HANDLE waits\[4\]=\{state->shutdown,state->stop_requested,state->quit,state->io_release\};'){
    throw 'Presentation wait-set shutdown/release priority changed'
}
if($source -notmatch 'while\(\(wait=WaitForMultipleObjects\(4,waits,FALSE,20\)\)!=WAIT_OBJECT_0\+2\)'){
    throw 'Native acquisition must retain the20ms cancellable wait'
}
if($source -notmatch 'ntvwm_presentation_input' -or $source -notmatch 'ntvwm_presentation_capture'){
    throw 'Native input/capture production calls missing'
}
'PASS source contract: native20ms acquisition and existing prioritized wait-set'
