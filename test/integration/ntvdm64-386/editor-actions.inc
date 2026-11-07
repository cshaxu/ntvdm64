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
mov word [which-data],title_a-data
call target_prompt
call alt_file
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
mov dword [wanted_count-data],1
call wait_complete
mov si,events_exit-data
call type_events
call wait_gone
mov word [which-data],title_b-data
call target_prompt
mov si,events_esc-data
call type_events
call alt_file
mov si,events_x-data
call type_events
mov dword [wanted_count-data],2
call wait_complete
mov si,events_exit-data
call type_events
call wait_gone
