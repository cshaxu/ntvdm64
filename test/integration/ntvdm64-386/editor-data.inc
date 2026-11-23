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
launch_a: db W31_EDITOR_DEST,'\PATCH\A.PIF',0
%include W31_EDITOR_EVENTS
menu_stage_name: db W31_EDITOR_DEST,'\PATCH\MENUSTG.BIN',0
