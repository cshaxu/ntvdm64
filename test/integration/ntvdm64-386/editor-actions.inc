%macro close_native 1
mov word [which-data],title_a-data
call target_prompt
call alt_file
%if MOUSE_MENU
mov word [mouse_half_row-data],15 ; native EDIT Exit: row 7
call mouse_cell
%else
mov si,events_esc-data
call type_events
mov ax,0x111
mov bx,0x1d
call inject_key
mov si,events_q-data
call type_events
mov ax,0x211
mov bx,0x1d
call inject_key
%endif
mov dword [wanted_count-data],%1
call wait_complete
mov si,events_exit-data
call type_events
call wait_gone
%endmacro
%macro close_dos 1
mov word [which-data],title_b-data
call target_prompt
mov si,events_esc-data
call type_events
call alt_file
%if MOUSE_MENU
call mouse_exit
%else
mov si,events_x-data
call type_events
%endif
mov dword [wanted_count-data],%1
call wait_complete
mov si,events_exit-data
call type_events
call wait_gone
%endmacro
; WIN.INI load starts these applications minimized. Open both before waiting
; for DOS input readiness, rather than depending on background scheduling.
mov word [which-data],title_a-data
call target_prompt
mov word [which-data],title_b-data
call target_prompt
%if DOSONLY
call wait_readers
mov word [which-data],title_a-data
call target_prompt
mov si,events_a-data
call type_events
mov dword [wanted_count-data],3
call wait_complete
mov word [which-data],title_b-data
call target_prompt
mov si,events_b-data
call type_events
call wait_editors
mov si,events_esc-data
call type_events
call alt_file
%if MOUSE_MENU
call mouse_exit
%else
mov si,events_x-data
call type_events
%endif
mov dword [wanted_count-data],4
call wait_complete
mov si,events_exit-data
call type_events
call wait_gone
mov word [which-data],title_a-data
call target_prompt
mov si,events_exit-data
call type_events
call wait_gone
%else
call wait_readers
mov word [which-data],title_a-data
call target_prompt
mov si,events_a-data
call type_events
mov word [which-data],title_b-data
call target_prompt
mov si,events_b-data
call type_events
call wait_editors
%if SAVE_WITNESS
mov word [which-data],title_a-data
call target_prompt
mov si,events_text_a-data
call type_events
mov ax,0x111
mov bx,0x1d
call inject_key
mov si,events_s-data
call type_events
mov ax,0x211
mov bx,0x1d
call inject_key
mov word [which-data],title_b-data
call target_prompt
mov si,events_esc-data
call type_events
mov si,events_text_b-data
call type_events
call alt_file
mov si,events_s-data
call type_events
%endif
%if FIRST_DOS
close_dos 1
close_native 2
%else
close_native 1
close_dos 2
%endif
%endif
