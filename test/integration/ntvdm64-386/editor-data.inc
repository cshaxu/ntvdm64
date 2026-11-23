%if EDITOR_FRONTEND
frontend_request: times 4 dw 0
frontend_ack: dw 0
frontend_request_name: db W31_EDITOR_DEST,'\PATCH\MREQ.BIN',0
frontend_ack_name: db W31_EDITOR_DEST,'\PATCH\MACK.BIN',0
%endif
mouse_half_row: dw 0
mouse_rect: times 4 dw 0
mouse_point: times 2 dw 0
mouse_actual: times 2 dw 0
vmd_api: dd 0
vmd_version: dw 0
vmd_reply: times 50 db 0
vmd_query_error: dw 0
mouse_scroll: times 2 dw 0
mouse_delta: times 2 dw 0
mouse_extent: times 2 dw 0
mouse_receipt_name: db W31_EDITOR_DEST,'\PATCH\MOUSE'
mouse_receipt_digit: db '0'
db '.BIN',0
timer_id: dw 0
timer_msg: times 18 db 0
completed_count: dd 0
wanted_count: dd 0
read_count: dw 0
editors_name: db W31_EDITOR_DEST,'\PATCH\EDITORS.BIN',0
readers_name: db W31_EDITOR_DEST,'\PATCH\READERS.BIN',0
%if DOSONLY
completion_name: db W31_EDITOR_DEST,'\PATCH\TOTAL.BIN',0
%else
completion_name: db W31_EDITOR_DEST,'\PATCH\DONE.BIN',0
%endif
input_api: dd 0
stamp: dw 0
active: dw 0
which: dw 0
title_a: db 'Prompt probe A',0
title_b: db 'Prompt probe B',0
%if !EDITOR_WINDOWS95
launch_a: db W31_EDITOR_DEST,'\PATCH\A.PIF',0
%endif
%include W31_EDITOR_EVENTS
%if !EDITOR_WINDOWS95
menu_stage_name: db W31_EDITOR_DEST,'\PATCH\MENUSTG.BIN',0
%endif
%if EDITOR_GUEST_INPUT
production_buttons: dw 0
production_pending_button: dw 0
%endif
%if EDITOR_WINDOWS95 && MOUSE_MENU
mouse_hwnd: dw 0
mouse_class: times 64 db 0
%endif
%if EDITOR_DISMISS
welcome_title: db EDITOR_WELCOME_BYTES,0
%endif
