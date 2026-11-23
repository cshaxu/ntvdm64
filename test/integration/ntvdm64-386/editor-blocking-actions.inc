%macro blocking_native 0
mov word [which-data],title_a-data
call target_prompt
call production_wait
call target_prompt
mov si,events_a-data
call type_events
mov ax,0x2000
call mouse_input
call target_prompt
mov si,events_exit-data
call type_events
call wait_gone
%endmacro
%macro blocking_dos 0
mov word [which-data],title_b-data
call target_prompt
call production_wait
call target_prompt
mov si,events_b-data
call type_events
call production_wait
call target_prompt
mov ax,0x2001
call mouse_input
call target_prompt
mov si,events_exit-data
call type_events
call wait_gone
%endmacro
%if EDITOR_DOS_FIRST
blocking_dos
blocking_native
%else
blocking_native
blocking_dos
%endif
