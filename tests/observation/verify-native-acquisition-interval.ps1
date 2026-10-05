# Source-contract check; runtime presentation/handoff tests remain separate.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$source=Get-Content "$repo/src/ntvwm-exe/main.c" -Raw
if($source -notmatch 'HANDLE waits\[5\]=\{state->shutdown,state->stop_requested,state->quit,state->io_release,state->route_changed\};'){
    throw 'Presentation wait-set shutdown/release priority changed'
}
if($source -notmatch 'timeout=state->presenting && state->users && !state->io_released \? 20 : INFINITE;' -or
   $source -notmatch 'wait=WaitForMultipleObjects\(5,waits,FALSE,timeout\);'){
    throw 'Native acquisition must retain the20ms cancellable wait'
}
if($source -notmatch 'ntvwm_presentation_input' -or $source -notmatch 'ntvwm_presentation_capture'){
    throw 'Native input/capture production calls missing'
}
$loop=$source.Substring($source.IndexOf('static DWORD presentation_loop'),$source.IndexOf('static DWORD WINAPI presentation_pump')-$source.IndexOf('static DWORD presentation_loop'))
if($loop -match 'ntvwm_presentation_input' -or $source -notmatch 'worker_base_input_watch_create'){
    throw 'Input must use the shared event watcher, never the acquisition timeout'
}
'PASS source contract: native20ms output-only acquisition; event input/admission; prioritized wait-set'
