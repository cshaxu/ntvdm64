%ifndef FIRST_DOS_BEFORE_SECOND
%define FIRST_DOS_BEFORE_SECOND 0
%endif
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
%if FIRST_DOS_BEFORE_SECOND
%if DOSONLY
%error FIRST_DOS_BEFORE_SECOND requires DOSONLY=0
%endif
mov word [which-data],title_b-data
call target_prompt
mov dword [wanted_count-data],1
call wait_reader_count
call target_prompt               ; waiting can return focus to the system VM
mov si,events_b-data
call type_events
mov dword [wanted_count-data],1
call wait_editor_count
call alt_file                    ; exercise first editor before second VM exists
%if MOUSE_MENU
mov dword [wanted_count-data],1
call wait_menu_stage
%endif
mov si,events_esc-data
call type_events
%if MOUSE_MENU
mov dword [wanted_count-data],2
call wait_menu_stage
%endif
push ds
push word launch_a-data
push word 1
api 1,166                       ; start second prompt after first editor is live
cmp ax,32
jb fatal
%endif
mov word [which-data],title_a-data
call target_prompt
mov word [which-data],title_b-data
call target_prompt
%if FIRST_DOS_BEFORE_SECOND && MOUSE_MENU
call alt_file                    ; exercise the same editor after VM switch
mov dword [wanted_count-data],3
call wait_menu_stage
mov si,events_esc-data
call type_events
%endif
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
%if !FIRST_DOS_BEFORE_SECOND
mov word [which-data],title_b-data
call target_prompt
mov si,events_b-data
call type_events
%endif
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
