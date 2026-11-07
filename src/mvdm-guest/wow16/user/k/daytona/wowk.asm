	TITLE	..\wow.c
	.386P
include listing.inc
if 1 ;  @Version gt 510
; .model NOTHING
else
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
_DATA	SEGMENT DWORD USE16 PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT DWORD USE16 PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT DWORD USE16 PUBLIC 'BSS'
_BSS	ENDS
$$SYMBOLS	SEGMENT BYTE USE16 'DEBSYM'
$$SYMBOLS	ENDS
$$TYPES	SEGMENT BYTE USE16 'DEBTYP'
$$TYPES	ENDS
_TLS	SEGMENT DWORD USE16 PUBLIC 'TLS'
_TLS	ENDS
;	COMDAT ??_C@_0DE@PGCI@Assertion?5failed?3?50?5?$CG?$CG?5?$CCPtoHq?5re@
CONST	SEGMENT DWORD USE16 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0BB@FPFL@?4?4?2?4?4?2inc?2user?4h?$AA@
CONST	SEGMENT DWORD USE16 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0BK@ODAC@Global?5lock?5on?5bad?5handle?$AA@
CONST	SEGMENT DWORD USE16 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0GB@JKFJ@Assertion?5failed?3?5?$CCGlobal?5lock?5o@
CONST	SEGMENT DWORD USE16 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0N@LKBL@?4?4?2usercli?4h?$AA@
CONST	SEGMENT DWORD USE16 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0BL@NHGD@GlobalUnlock?5on?5bad?5handle?$AA@
CONST	SEGMENT DWORD USE16 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0FL@KKIG@Assertion?5failed?3?5?$CCGlobalUnlock?5@
CONST	SEGMENT DWORD USE16 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0BJ@CPLJ@GlobalFree?5on?5bad?5handle?$AA@
CONST	SEGMENT DWORD USE16 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0FJ@GEAM@Assertion?5failed?3?5?$CCGlobalFree?5on@
CONST	SEGMENT DWORD USE16 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0CD@CDDL@IsWindowVisible?3?5exception?5handl@
CONST	SEGMENT DWORD USE16 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_08HKAD@?4?4?2wow?4c?$AA@
CONST	SEGMENT DWORD USE16 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT _NtCurrentTeb@0
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _RtlConvertLongToLargeInteger@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _SetCallServerFlag@0
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _WOWRtlCopyMemory@12
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _WOWlstrlenA@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT @ValidateHwnd@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT @ValidateHwndNoRip@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetClassNameA@12
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __GetDesktopWindow@0
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetDesktopWindow@0
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __GetDlgItem@8
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetDlgItem@8
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetKeyboardState@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetKeyState@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetMenu@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetMenuItemCount@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetMenuItemID@8
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetMenuState@12
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _IsWindow@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetWindow@8
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetParent@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetSubMenu@8
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetSysColor@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetSystemMetrics@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetTopWindow@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _IsChild@8
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _IsIconic@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _IsWindowEnabled@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _IsWindowVisible@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _IsZoomed@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _ClientToScreen@8
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetClientRect@8
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetCursorPos@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _GetWindowRect@8
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _ScreenToClient@8
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _EnableMenuItem@12
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT __PhkNext@4
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _CallNextHookEx@16
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _WOW16DefHookProc@16
_TEXT	SEGMENT PARA USE16 PUBLIC 'CODE'
_TEXT	ENDS
; NOTHING	GROUP _DATA, CONST, _BSS
	ASSUME	CS: NOTHING, DS: FLAT, SS: FLAT
endif
_DATA	SEGMENT
COMM	_gcWheelDelta:DWORD
_DATA	ENDS
PUBLIC	@ValidateHwnd@4
EXTRN	@HMValidateHandle@8:FAR
;	COMDAT @ValidateHwnd@4
_TEXT	SEGMENT
; File ..\wow.c
@ValidateHwnd@4 PROC FAR				; COMDAT
; Line 157
	mov	eax, DWORD PTR fs:24
	add	eax, 68					; 00000044H
; Line 162
	test	ecx, ecx
	je	SHORT $L23566
	cmp	DWORD PTR [eax+40], ecx
	jne	SHORT $L23566
; Line 163
	mov	eax, DWORD PTR [eax+44]
	jmp	SHORT $L23564
; Line 169
$L23566:
	mov	edx, 1
	jmp	@HMValidateHandle@8
; Line 170
$L23564:
	ret	0
@ValidateHwnd@4 ENDP
_TEXT	ENDS
PUBLIC	@ValidateHwndNoRip@4
EXTRN	@HMValidateHandleNoRip@8:FAR
;	COMDAT @ValidateHwndNoRip@4
_TEXT	SEGMENT
@ValidateHwndNoRip@4 PROC FAR				; COMDAT
; Line 176
	mov	eax, DWORD PTR fs:24
	add	eax, 68					; 00000044H
; Line 181
	test	ecx, ecx
	je	SHORT $L23570
	cmp	DWORD PTR [eax+40], ecx
	jne	SHORT $L23570
; Line 182
	mov	eax, DWORD PTR [eax+44]
	jmp	SHORT $L23568
; Line 188
$L23570:
	mov	edx, 1
	jmp	@HMValidateHandleNoRip@8
; Line 189
$L23568:
	ret	0
@ValidateHwndNoRip@4 ENDP
_TEXT	ENDS
PUBLIC	_GetClassNameA@12
;	COMDAT _GetClassNameA@12
_TEXT	SEGMENT
_hwnd$ = 8
_lpClassName$ = 12
_nMaxCount$ = 16
_GetClassNameA@12 PROC FAR				; COMDAT
; Line 197
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
	push	esi
	push	edi
; Line 203
	call	@ValidateHwnd@4
; Line 205
	test	eax, eax
	jne	SHORT $L23579
; Line 206
	xor	eax, eax
	jmp	SHORT $L23574
; Line 208
$L23579:
	mov	edx, DWORD PTR _nMaxCount$[bp]
	test	edx, edx
	je	SHORT $L23580
	mov	esi, DWORD PTR [eax+12]
	mov	ecx, DWORD PTR [eax+76]
; Line 210
	sub	ecx, esi
	mov	ecx, DWORD PTR [ecx+eax+100]
	sub	ecx, esi
	lea	esi, DWORD PTR [ecx+eax]
; Line 211
	push	ds
	pop	es
	mov	edi, esi
	mov	ecx, -1
	sub	eax, eax
	repnz	scas byte ptr [edi]
	not	ecx
	dec	ecx
	dec	edx
; Line 212
	cmp	edx, ecx
	jl	SHORT $L23827
	mov	edx, ecx
$L23827:
; Line 213
	push	ds
	pop	es
	mov	eax, DWORD PTR _lpClassName$[bp]
	mov	ecx, edx
	shr	ecx, 2
	mov	edi, eax
	rep	movs dword ptr [edi], dword ptr [esi]
	mov	ecx, edx
	and	ecx, 3
	rep	movs byte ptr [edi], byte ptr [esi]
; Line 214
	mov	BYTE PTR [edx+eax], 0
; Line 217
$L23580:
	mov	eax, edx
; Line 218
$L23574:
	pop	edi
	pop	esi
	pop	ebp
	ret	12					; 0000000cH
_GetClassNameA@12 ENDP
_TEXT	ENDS
PUBLIC	__GetDesktopWindow@0
;	COMDAT __GetDesktopWindow@0
_TEXT	SEGMENT
__GetDesktopWindow@0 PROC FAR				; COMDAT
; Line 235
	mov	eax, DWORD PTR fs:24
	lea	ecx, DWORD PTR [eax+68]
; Line 237
	mov	eax, DWORD PTR [eax+92]
	mov	eax, DWORD PTR [eax+8]
	sub	eax, DWORD PTR [ecx+28]
; Line 238
	ret	0
__GetDesktopWindow@0 ENDP
_TEXT	ENDS
PUBLIC	_GetDesktopWindow@0
;	COMDAT _GetDesktopWindow@0
_TEXT	SEGMENT
_GetDesktopWindow@0 PROC FAR				; COMDAT
; Line 246
	call	__GetDesktopWindow@0
; Line 247
	test	eax, eax
	jne	SHORT $L23834
	xor	eax, eax
	jmp	SHORT $L23583
$L23834:
	mov	eax, DWORD PTR [eax]
; Line 248
$L23583:
	ret	0
_GetDesktopWindow@0 ENDP
_TEXT	ENDS
PUBLIC	__GetDlgItem@8
;	COMDAT __GetDlgItem@8
_TEXT	SEGMENT
_pwnd$ = 8
_id$ = 12
__GetDlgItem@8 PROC FAR				; COMDAT
; Line 254
	push	ebp
	mov	eax, DWORD PTR _pwnd$[esp]
	mov	ebp, esp
	test	eax, eax
; Line 255
	je	SHORT $L23588
	mov	ecx, DWORD PTR [eax+28]
; Line 256
	cmp	ecx, 2147418111				; 7ffeffffH
	jbe	SHORT $L23838
	sub	eax, DWORD PTR [eax+12]
	add	eax, ecx
	jmp	SHORT $L23842
$L23838:
	mov	eax, ecx
$L23842:
; Line 257
	test	eax, eax
	je	SHORT $L23588
	mov	ecx, DWORD PTR _id$[bp]
$L23590:
; Line 258
	cmp	DWORD PTR [eax+104], ecx
	je	SHORT $L23588
	mov	edx, DWORD PTR [eax+20]
; Line 260
	cmp	edx, 2147418111				; 7ffeffffH
	jbe	SHORT $L23840
	sub	eax, DWORD PTR [eax+12]
	add	eax, edx
	jmp	SHORT $L23843
$L23840:
	mov	eax, edx
$L23843:
; Line 261
	test	eax, eax
	jne	SHORT $L23590
; Line 264
$L23588:
; Line 265
	pop	ebp
	ret	8
__GetDlgItem@8 ENDP
_TEXT	ENDS
PUBLIC	_GetDlgItem@8
;	COMDAT _GetDlgItem@8
_TEXT	SEGMENT
_hwnd$ = 8
_id$ = 12
_GetDlgItem@8 PROC FAR					; COMDAT
; Line 271
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
; Line 275
	call	@ValidateHwnd@4
; Line 276
	test	eax, eax
	jne	SHORT $L23598
; Line 277
	xor	eax, eax
	jmp	SHORT $L23847
; Line 279
$L23598:
	push	DWORD PTR _id$[bp]
	push	eax
	call	__GetDlgItem@8
; Line 281
	test	eax, eax
	jne	SHORT $L23846
	xor	eax, eax
	jmp	SHORT $L23847
$L23846:
	mov	eax, DWORD PTR [eax]
$L23847:
; Line 287
	pop	ebp
	ret	8
_GetDlgItem@8 ENDP
_TEXT	ENDS
PUBLIC	_GetKeyboardState@4
; EXTRN	_wow16CsrFlag:DWORD
;	COMDAT _GetKeyboardState@4
_TEXT	SEGMENT
_GetKeyboardState@4 PROC FAR				; COMDAT
; Line 305
	mov	eax, DWORD PTR _wow16CsrFlag
	mov	BYTE PTR [eax], 1
; Line 306
	xor	eax, eax
; Line 310
	ret	4
_GetKeyboardState@4 ENDP
_TEXT	ENDS
PUBLIC	_GetKeyState@4
;	COMDAT _GetKeyState@4
_TEXT	SEGMENT
_GetKeyState@4 PROC FAR				; COMDAT
; Line 319
	mov	eax, DWORD PTR _wow16CsrFlag
	mov	BYTE PTR [eax], 1
; Line 320
	xor	ax, ax
; Line 321
	ret	4
_GetKeyState@4 ENDP
_TEXT	ENDS
PUBLIC	_GetMenu@4
;	COMDAT _GetMenu@4
_TEXT	SEGMENT
_hwnd$ = 8
_GetMenu@4 PROC FAR					; COMDAT
; Line 326
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
; Line 330
	call	@ValidateHwnd@4
; Line 332
	test	eax, eax
	jne	SHORT $L23608
; Line 333
	xor	eax, eax
	jmp	SHORT $L23605
; Line 339
$L23608:
	mov	cl, BYTE PTR [eax+171]
	and	cl, 192					; 000000c0H
	cmp	cl, 64					; 00000040H
	je	SHORT $L23609
	mov	ecx, DWORD PTR [eax+104]
; Line 340
	cmp	ecx, 2147418111				; 7ffeffffH
	jbe	SHORT $L23856
	sub	ecx, DWORD PTR [eax+12]
	add	ecx, eax
$L23856:
; Line 341
	test	ecx, ecx
	jne	SHORT $L23858
	xor	eax, eax
	jmp	SHORT $L23605
$L23858:
	mov	eax, DWORD PTR [ecx]
	jmp	SHORT $L23605
; Line 342
$L23609:
; Line 343
	mov	eax, DWORD PTR [eax+104]
; Line 345
$L23605:
	pop	ebp
	ret	4
_GetMenu@4 ENDP
_TEXT	ENDS
PUBLIC	_GetMenuItemCount@4
;	COMDAT _GetMenuItemCount@4
_TEXT	SEGMENT
_hMenu$ = 8
_GetMenuItemCount@4 PROC FAR				; COMDAT
; Line 359
	push	ebp
	mov	edx, 2
	mov	ebp, esp
; Line 362
	mov	ecx, DWORD PTR _hMenu$[esp]
	call	@HMValidateHandle@8
; Line 364
	test	eax, eax
	jne	SHORT $L23614
; Line 365
	mov	eax, -1
	jmp	SHORT $L23612
; Line 367
$L23614:
	mov	eax, DWORD PTR [eax+40]
; Line 368
$L23612:
	pop	ebp
	ret	4
_GetMenuItemCount@4 ENDP
_TEXT	ENDS
PUBLIC	_GetMenuItemID@8
;	COMDAT _GetMenuItemID@8
_TEXT	SEGMENT
_hMenu$ = 8
_nPos$ = 12
_GetMenuItemID@8 PROC FAR				; COMDAT
; Line 381
	push	ebp
	mov	edx, 2
	mov	ebp, esp
; Line 385
	mov	ecx, DWORD PTR _hMenu$[esp]
	call	@HMValidateHandle@8
; Line 387
	test	eax, eax
	je	SHORT $L23864
; Line 394
	mov	ecx, DWORD PTR _nPos$[bp]
	cmp	DWORD PTR [eax+40], ecx
	jle	SHORT $L23864
	test	ecx, ecx
	jl	SHORT $L23864
; Line 395
	shl	ecx, 6
	mov	edx, DWORD PTR [eax+56]
	sub	edx, DWORD PTR [eax+12]
	add	edx, ecx
	add	eax, edx
; Line 396
	cmp	DWORD PTR [eax+12], 0
	jne	SHORT $L23864
; Line 397
	mov	eax, DWORD PTR [eax+8]
	jmp	SHORT $L23617
$L23864:
; Line 400
	mov	eax, -1
; Line 401
$L23617:
	pop	ebp
	ret	8
_GetMenuItemID@8 ENDP
_TEXT	ENDS
PUBLIC	_GetMenuState@12
EXTRN	__GetMenuState@12:FAR
;	COMDAT _GetMenuState@12
_TEXT	SEGMENT
_hMenu$ = 8
_uId$ = 12
_uFlags$ = 16
_GetMenuState@12 PROC FAR				; COMDAT
; Line 408
	push	ebp
	mov	edx, 2
	mov	ebp, esp
; Line 411
	mov	ecx, DWORD PTR _hMenu$[esp]
	call	@HMValidateHandle@8
; Line 413
	test	eax, eax
	je	SHORT $L23629
	test	DWORD PTR _uFlags$[bp], -65536		; ffff0000H
	jne	SHORT $L23629
; Line 417
	push	DWORD PTR _uFlags$[bp]
	push	DWORD PTR _uId$[bp]
	push	eax
	call	__GetMenuState@12
	jmp	SHORT $L23626
; Line 413
$L23629:
; Line 414
	mov	eax, -1
; Line 418
$L23626:
	pop	ebp
	ret	12					; 0000000cH
_GetMenuState@12 ENDP
_TEXT	ENDS
PUBLIC	_IsWindow@4
; EXTRN	_wow16gpsi:DWORD
;	COMDAT _IsWindow@4
_TEXT	SEGMENT
_hwnd$ = 8
_IsWindow@4 PROC FAR					; COMDAT
; Line 423
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
; Line 429
	call	@ValidateHwndNoRip@4
; Line 434
	test	eax, eax
	je	SHORT $L23633
; Line 439
	mov	edx, DWORD PTR _hwnd$[bp]
	mov	ecx, DWORD PTR _wow16gpsi
	and	edx, 65535				; 0000ffffH
	mov	ecx, DWORD PTR [ecx+4]
	shl	edx, 4
	test	BYTE PTR [edx+ecx+9], 1
	je	SHORT $L23633
; Line 440
	xor	eax, eax
; Line 462
$L23633:
	pop	ebp
	cmp	eax, 1
	sbb	eax, eax
	inc	eax
; Line 463
	ret	4
_IsWindow@4 ENDP
_TEXT	ENDS
PUBLIC	_GetWindow@8
EXTRN	__GetWindow@8:FAR
;	COMDAT _GetWindow@8
_TEXT	SEGMENT
_hwnd$ = 8
_wCmd$ = 12
_GetWindow@8 PROC FAR					; COMDAT
; Line 469
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
; Line 472
	call	@ValidateHwnd@4
; Line 473
	test	eax, eax
	jne	SHORT $L23640
; Line 474
	xor	eax, eax
	jmp	SHORT $L23638
; Line 476
$L23640:
	push	DWORD PTR _wCmd$[bp]
	push	eax
	call	__GetWindow@8
; Line 477
	test	eax, eax
	jne	SHORT $L23871
	xor	eax, eax
	jmp	SHORT $L23638
$L23871:
	mov	eax, DWORD PTR [eax]
; Line 478
$L23638:
	pop	ebp
	ret	8
_GetWindow@8 ENDP
_TEXT	ENDS
PUBLIC	_GetParent@4
EXTRN	__GetParent@4:FAR
;	COMDAT _GetParent@4
_TEXT	SEGMENT
_hwnd$ = 8
_GetParent@4 PROC FAR					; COMDAT
; Line 482
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
; Line 485
	call	@ValidateHwnd@4
; Line 486
	test	eax, eax
	jne	SHORT $L23644
; Line 487
	xor	eax, eax
	jmp	SHORT $L23642
; Line 489
$L23644:
	push	eax
	call	__GetParent@4
; Line 490
	test	eax, eax
	jne	SHORT $L23875
	xor	eax, eax
	jmp	SHORT $L23642
$L23875:
	mov	eax, DWORD PTR [eax]
; Line 491
$L23642:
	pop	ebp
	ret	4
_GetParent@4 ENDP
_TEXT	ENDS
PUBLIC	_GetSubMenu@8
EXTRN	__GetSubMenu@8:FAR
;	COMDAT _GetSubMenu@8
_TEXT	SEGMENT
_hMenu$ = 8
_nPos$ = 12
_GetSubMenu@8 PROC FAR					; COMDAT
; Line 496
	push	ebp
	mov	edx, 2
	mov	ebp, esp
; Line 499
	mov	ecx, DWORD PTR _hMenu$[esp]
	call	@HMValidateHandle@8
; Line 501
	test	eax, eax
	jne	SHORT $L23649
; Line 502
	xor	eax, eax
	jmp	SHORT $L23647
; Line 504
$L23649:
	push	DWORD PTR _nPos$[bp]
	push	eax
	call	__GetSubMenu@8
; Line 505
	test	eax, eax
	jne	SHORT $L23879
	xor	eax, eax
	jmp	SHORT $L23647
$L23879:
	mov	eax, DWORD PTR [eax]
; Line 506
$L23647:
	pop	ebp
	ret	8
_GetSubMenu@8 ENDP
_TEXT	ENDS
PUBLIC	_GetSysColor@4
;	COMDAT _GetSysColor@4
_TEXT	SEGMENT
_nIndex$ = 8
_GetSysColor@4 PROC FAR				; COMDAT
; Line 511
	push	ebp
	mov	ecx, DWORD PTR _nIndex$[esp]
	mov	ebp, esp
	test	ecx, ecx
; Line 531
	jl	SHORT $L23653
	cmp	ecx, 25					; 00000019H
	jge	SHORT $L23653
; Line 540
	mov	eax, DWORD PTR _wow16gpsi
	mov	edx, DWORD PTR [eax]
	mov	eax, DWORD PTR [edx+ecx*4+672]
	jmp	SHORT $L23651
; Line 531
$L23653:
; Line 537
	xor	eax, eax
; Line 541
$L23651:
	pop	ebp
	ret	4
_GetSysColor@4 ENDP
_TEXT	ENDS
PUBLIC	_GetSystemMetrics@4
;	COMDAT _GetSystemMetrics@4
_TEXT	SEGMENT
_index$ = 8
_GetSystemMetrics@4 PROC FAR				; COMDAT
; Line 546
	push	ebp
	mov	ecx, DWORD PTR _index$[esp]
	mov	ebp, esp
	test	ecx, ecx
; Line 549
	jl	SHORT $L23657
	cmp	ecx, 76					; 0000004cH
	jge	SHORT $L23657
; Line 551
	mov	eax, DWORD PTR fs:24
	cmp	DWORD PTR [eax+80], 1024		; 00000400H
	jae	SHORT $L23658
; Line 572
	cmp	ecx, 2
	jl	SHORT $L23658
	cmp	ecx, 4
	jle	SHORT $L23664
	cmp	ecx, 7
	jl	SHORT $L23658
	cmp	ecx, 8
	jle	SHORT $L23663
	cmp	ecx, 15					; 0000000fH
	je	SHORT $L23663
	cmp	ecx, 17					; 00000011H
	je	SHORT $L23663
$L23658:
	mov	eax, DWORD PTR _wow16gpsi
	mov	edx, DWORD PTR [eax]
	mov	eax, DWORD PTR [edx+ecx*4+368]
	jmp	SHORT $L23655
; Line 579
$L23664:
; Line 582
	mov	eax, DWORD PTR _wow16gpsi
	mov	edx, DWORD PTR [eax]
	mov	eax, DWORD PTR [edx+ecx*4+368]
	inc	eax
	jmp	SHORT $L23655
; Line 573
$L23663:
; Line 577
	mov	eax, DWORD PTR _wow16gpsi
	mov	edx, DWORD PTR [eax]
	mov	eax, DWORD PTR [edx+ecx*4+368]
	dec	eax
	jmp	SHORT $L23655
; Line 549
$L23657:
	xor	eax, eax
; Line 587
$L23655:
	pop	ebp
	ret	4
_GetSystemMetrics@4 ENDP
_TEXT	ENDS
PUBLIC	_GetTopWindow@4
;	COMDAT _GetTopWindow@4
_TEXT	SEGMENT
_hwnd$ = 8
_GetTopWindow@4 PROC FAR				; COMDAT
; Line 603
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
	test	ecx, ecx
; Line 609
	jne	SHORT $L23668
; Line 610
	call	__GetDesktopWindow@0
; Line 611
	jmp	SHORT $L23669
$L23668:
; Line 612
	call	@ValidateHwnd@4
; Line 613
$L23669:
; Line 614
	test	eax, eax
	jne	SHORT $L23670
; Line 615
	xor	eax, eax
	jmp	SHORT $L23666
; Line 617
$L23670:
	mov	ecx, DWORD PTR [eax+28]
	cmp	ecx, 2147418111				; 7ffeffffH
	jbe	SHORT $L23889
	sub	eax, DWORD PTR [eax+12]
	add	eax, ecx
	jmp	SHORT $L23890
$L23889:
	mov	eax, ecx
$L23890:
; Line 618
	test	eax, eax
	jne	SHORT $L23891
	xor	eax, eax
	jmp	SHORT $L23666
$L23891:
	mov	eax, DWORD PTR [eax]
; Line 619
$L23666:
	pop	ebp
	ret	4
_GetTopWindow@4 ENDP
_TEXT	ENDS
PUBLIC	_IsChild@8
EXTRN	__IsChild@8:FAR
;	COMDAT _IsChild@8
_TEXT	SEGMENT
_hwndParent$ = 8
_hwnd$ = 12
_IsChild@8 PROC FAR					; COMDAT
; Line 625
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
	push	esi
; Line 628
	call	@ValidateHwnd@4
	mov	esi, eax
; Line 629
	test	esi, esi
	jne	SHORT $L23676
; Line 630
	xor	eax, eax
	jmp	SHORT $L23673
; Line 632
$L23676:
	mov	ecx, DWORD PTR _hwndParent$[bp]
	call	@ValidateHwnd@4
; Line 633
	test	eax, eax
	jne	SHORT $L23677
; Line 634
	xor	eax, eax
	jmp	SHORT $L23673
; Line 636
$L23677:
	push	esi
	push	eax
	call	__IsChild@8
; Line 637
$L23673:
	pop	esi
	pop	ebp
	ret	8
_IsChild@8 ENDP
_TEXT	ENDS
PUBLIC	_IsIconic@4
;	COMDAT _IsIconic@4
_TEXT	SEGMENT
_hwnd$ = 8
_IsIconic@4 PROC FAR					; COMDAT
; Line 641
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
; Line 644
	call	@ValidateHwnd@4
; Line 646
	test	eax, eax
	jne	SHORT $L23681
; Line 647
	xor	eax, eax
	jmp	SHORT $L23679
; Line 649
$L23681:
	mov	al, BYTE PTR [eax+171]
	and	al, 32					; 00000020H
	cmp	al, 1
	sbb	eax, eax
	inc	eax
; Line 650
$L23679:
	pop	ebp
	ret	4
_IsIconic@4 ENDP
_TEXT	ENDS
PUBLIC	_IsWindowEnabled@4
;	COMDAT _IsWindowEnabled@4
_TEXT	SEGMENT
_hwnd$ = 8
_IsWindowEnabled@4 PROC FAR				; COMDAT
; Line 654
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
; Line 657
	call	@ValidateHwnd@4
; Line 659
	test	eax, eax
	jne	SHORT $L23685
; Line 660
	xor	eax, eax
	jmp	SHORT $L23683
; Line 662
$L23685:
	mov	al, BYTE PTR [eax+171]
	and	al, 8
	cmp	al, 1
	sbb	eax, eax
	neg	eax
; Line 663
$L23683:
	pop	ebp
	ret	4
_IsWindowEnabled@4 ENDP
_TEXT	ENDS
PUBLIC	_IsWindowVisible@4
EXTRN	__IsWindowVisible@4:FAR
;	COMDAT _IsWindowVisible@4
_TEXT	SEGMENT
_hwnd$ = 8
_IsWindowVisible@4 PROC FAR				; COMDAT
; Line 667
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
; Line 671
	call	@ValidateHwnd@4
; Line 682
	test	eax, eax
	jne	SHORT $L23690
; Line 683
	xor	eax, eax
; Line 684
	jmp	SHORT $L23692
$L23690:
; Line 685
	push	eax
	call	__IsWindowVisible@4
; Line 692
$L23692:
; Line 693
	pop	ebp
	ret	4
_IsWindowVisible@4 ENDP
_TEXT	ENDS
PUBLIC	_IsZoomed@4
;	COMDAT _IsZoomed@4
_TEXT	SEGMENT
_hwnd$ = 8
_IsZoomed@4 PROC FAR					; COMDAT
; Line 697
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
; Line 700
	call	@ValidateHwnd@4
; Line 702
	test	eax, eax
	jne	SHORT $L23702
; Line 703
	xor	eax, eax
	jmp	SHORT $L23700
; Line 705
$L23702:
	xor	ecx, ecx
	mov	cl, BYTE PTR [eax+171]
	and	ecx, 1
	mov	eax, ecx
; Line 706
$L23700:
	pop	ebp
	ret	4
_IsZoomed@4 ENDP
_TEXT	ENDS
PUBLIC	_ClientToScreen@8
;	COMDAT _ClientToScreen@8
_TEXT	SEGMENT
_hwnd$ = 8
_ppoint$ = 12
_ClientToScreen@8 PROC FAR				; COMDAT
; Line 711
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
	push	esi
; Line 714
	call	@ValidateHwnd@4
; Line 716
	test	eax, eax
	jne	SHORT $L23707
; Line 717
	xor	eax, eax
	jmp	SHORT $L23705
; Line 720
$L23707:
	mov	ecx, DWORD PTR [eax+56]
	mov	edx, DWORD PTR _ppoint$[bp]
	movsx	esi, WORD PTR [edx]
	add	ecx, esi
; Line 722
	cmp	ecx, -32768				; ffff8000H
	jl	SHORT $L23905
	mov	esi, 32767				; 00007fffH
	cmp	ecx, esi
	jl	SHORT $L23909
	mov	ecx, esi
$L23909:
	mov	WORD PTR [edx], cx
	jmp	SHORT $L23906
$L23905:
	mov	WORD PTR [edx], -32768			; ffff8000H
$L23906:
	add	edx, 2
	mov	eax, DWORD PTR [eax+60]
	movsx	ecx, WORD PTR [edx]
	add	eax, ecx
; Line 723
	cmp	eax, -32768				; ffff8000H
	jl	SHORT $L23907
	mov	ecx, 32767				; 00007fffH
	cmp	eax, ecx
	jl	SHORT $L23910
	mov	eax, ecx
$L23910:
	mov	WORD PTR [edx], ax
	jmp	SHORT $L23908
$L23907:
	mov	WORD PTR [edx], -32768			; ffff8000H
$L23908:
; Line 724
	mov	eax, 1
; Line 729
$L23705:
	pop	esi
	pop	ebp
	ret	8
_ClientToScreen@8 ENDP
_TEXT	ENDS
PUBLIC	_GetClientRect@8
EXTRN	__GetClientRect@8:FAR
;	COMDAT _GetClientRect@8
_TEXT	SEGMENT
_hwnd$ = 8
_prect$ = 12
_GetClientRect@8 PROC FAR				; COMDAT
; Line 734
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
; Line 737
	call	@ValidateHwnd@4
; Line 739
	test	eax, eax
	jne	SHORT $L23713
; Line 740
	xor	eax, eax
	jmp	SHORT $L23711
; Line 742
$L23713:
	push	DWORD PTR _prect$[bp]
	push	eax
	call	__GetClientRect@8
; Line 743
$L23711:
	pop	ebp
	ret	8
_GetClientRect@8 ENDP
_TEXT	ENDS
PUBLIC	_GetCursorPos@4
;	COMDAT _GetCursorPos@4
_TEXT	SEGMENT
_lpPoint$ = 8
_GetCursorPos@4 PROC FAR				; COMDAT
; Line 748
	push	ebp
	mov	eax, DWORD PTR _wow16gpsi
	mov	ebp, esp
	mov	ecx, DWORD PTR [eax]
	mov	eax, DWORD PTR [ecx+772]
; Line 760
	cmp	eax, -32768				; ffff8000H
	jl	SHORT $L23915
	mov	ecx, 32767				; 00007fffH
	mov	edx, DWORD PTR _lpPoint$[bp]
	cmp	eax, ecx
	jl	SHORT $L23919
	mov	eax, ecx
$L23919:
	mov	WORD PTR [edx], ax
	jmp	SHORT $L23916
$L23915:
	mov	edx, DWORD PTR _lpPoint$[bp]
	mov	WORD PTR [edx], -32768			; ffff8000H
$L23916:
	mov	eax, DWORD PTR _wow16gpsi
	mov	ecx, DWORD PTR [eax]
	mov	eax, DWORD PTR [ecx+776]
; Line 761
	cmp	eax, -32768				; ffff8000H
	jl	SHORT $L23917
	mov	ecx, 32767				; 00007fffH
	cmp	eax, ecx
	jl	SHORT $L23920
	mov	eax, ecx
$L23920:
	mov	WORD PTR [edx+2], ax
	jmp	SHORT $L23918
$L23917:
	mov	WORD PTR [edx+2], -32768		; ffff8000H
$L23918:
; Line 762
	mov	eax, 1
	pop	ebp
; Line 763
	ret	4
_GetCursorPos@4 ENDP
_TEXT	ENDS
PUBLIC	_GetWindowRect@8
;	COMDAT _GetWindowRect@8
_TEXT	SEGMENT
_hwnd$ = 8
_GetWindowRect@8 PROC FAR				; COMDAT
; Line 768
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
; Line 771
	call	@ValidateHwnd@4
; Line 773
	test	eax, eax
	jne	SHORT $L23720
; Line 774
	xor	eax, eax
	jmp	SHORT $L23718
; Line 777
$L23720:
; Line 778
	add	eax, 40					; 00000028H
; Line 782
$L23718:
	pop	ebp
	ret	8
_GetWindowRect@8 ENDP
_TEXT	ENDS
PUBLIC	_ScreenToClient@8
;	COMDAT _ScreenToClient@8
_TEXT	SEGMENT
_hwnd$ = 8
_ppoint$ = 12
_ScreenToClient@8 PROC FAR				; COMDAT
; Line 787
	push	ebp
	mov	ecx, DWORD PTR _hwnd$[esp]
	mov	ebp, esp
	push	esi
; Line 790
	call	@ValidateHwnd@4
; Line 792
	test	eax, eax
	jne	SHORT $L23725
; Line 793
	xor	eax, eax
	jmp	SHORT $L23723
; Line 796
$L23725:
	mov	esi, DWORD PTR _ppoint$[bp]
	movsx	ecx, WORD PTR [esi]
	sub	ecx, DWORD PTR [eax+56]
; Line 798
	cmp	ecx, -32768				; ffff8000H
	jl	SHORT $L23925
	mov	edx, 32767				; 00007fffH
	cmp	ecx, edx
	jl	SHORT $L23929
	mov	ecx, edx
$L23929:
	mov	WORD PTR [esi], cx
	jmp	SHORT $L23926
$L23925:
	mov	WORD PTR [esi], -32768			; ffff8000H
$L23926:
	add	esi, 2
	movsx	ecx, WORD PTR [esi]
	sub	ecx, DWORD PTR [eax+60]
; Line 799
	cmp	ecx, -32768				; ffff8000H
	jl	SHORT $L23927
	mov	eax, 32767				; 00007fffH
	cmp	ecx, eax
	jl	SHORT $L23930
	mov	ecx, eax
$L23930:
	mov	WORD PTR [esi], cx
	jmp	SHORT $L23928
$L23927:
	mov	WORD PTR [esi], -32768			; ffff8000H
$L23928:
; Line 800
	mov	eax, 1
; Line 805
$L23723:
	pop	esi
	pop	ebp
	ret	8
_ScreenToClient@8 ENDP
_TEXT	ENDS
PUBLIC	_EnableMenuItem@12
EXTRN	_MNLookUpItem@16:FAR
;	COMDAT _EnableMenuItem@12
_TEXT	SEGMENT
_hMenu$ = 8
_uIDEnableItem$ = 12
_uEnable$ = 16
_EnableMenuItem@12 PROC FAR				; COMDAT
; Line 811
	push	ebp
	mov	edx, 2
	mov	ebp, esp
; Line 815
	mov	ecx, DWORD PTR _hMenu$[esp]
	call	@HMValidateHandle@8
; Line 816
	test	eax, eax
	jne	SHORT $L23733
; Line 817
	mov	eax, -1
	jmp	SHORT $L23730
; Line 823
$L23733:
	push	dword ptr 0
	mov	ecx, DWORD PTR _uEnable$[bp]
	and	ecx, 1024				; 00000400H
	push	ecx
	push	DWORD PTR _uIDEnableItem$[bp]
	push	eax
	call	_MNLookUpItem@16
	test	eax, eax
	jne	SHORT $L23734
; Line 824
	mov	eax, -1
	jmp	SHORT $L23730
; Line 830
$L23734:
	mov	eax, DWORD PTR [eax+4]
; Line 831
	mov	ecx, eax
	xor	ecx, DWORD PTR _uEnable$[bp]
	test	cl, 3
	jne	SHORT $L23735
; Line 832
	and	eax, 3
	jmp	SHORT $L23730
; Line 836
$L23735:
	mov	eax, DWORD PTR _wow16CsrFlag
	mov	BYTE PTR [eax], 1
	xor	eax, eax
; Line 840
$L23730:
	pop	ebp
	ret	12					; 0000000cH
_EnableMenuItem@12 ENDP
_TEXT	ENDS
PUBLIC	__PhkNext@4
;	COMDAT __PhkNext@4
_TEXT	SEGMENT
_phk$ = 8
__PhkNext@4 PROC FAR					; COMDAT
; Line 855
	push	ebp
	mov	ecx, DWORD PTR _phk$[esp]
	mov	ebp, esp
	mov	eax, DWORD PTR [ecx+20]
; Line 861
	test	eax, eax
	je	SHORT $L23738
; Line 862
	sub	eax, DWORD PTR [ecx+12]
	add	eax, ecx
	jmp	SHORT $L23740
; Line 863
$L23738:
	test	BYTE PTR [ecx+32], 1
	mov	eax, 0
	jne	SHORT $L23740
; Line 864
	mov	eax, DWORD PTR fs:24
	lea	edx, DWORD PTR [eax+68]
; Line 866
	mov	eax, DWORD PTR [ecx+24]
	mov	ecx, DWORD PTR [edx+24]
	mov	eax, DWORD PTR [ecx+eax*4+20]
	mov	ecx, DWORD PTR [edx+28]
	imul	ecx, 44					; 0000002cH
	sub	eax, ecx
; Line 867
$L23740:
; Line 869
	pop	ebp
	ret	4
__PhkNext@4 ENDP
_TEXT	ENDS
PUBLIC	_CallNextHookEx@16
;	COMDAT _CallNextHookEx@16
_TEXT	SEGMENT
_CallNextHookEx@16 PROC FAR				; COMDAT
; Line 899
	mov	eax, DWORD PTR fs:24
	mov	ecx, DWORD PTR [eax+100]
; Line 907
	sub	ecx, DWORD PTR [eax+96]
	add	eax, 68					; 00000044H
	push	ecx
	call	__PhkNext@4
	test	eax, eax
	mov	eax, 0
	je	SHORT $L23747
; Line 912
	mov	eax, DWORD PTR _wow16CsrFlag
	mov	BYTE PTR [eax], 1
	xor	eax, eax
; Line 1065
$L23747:
	ret	16					; 00000010H
_CallNextHookEx@16 ENDP
_TEXT	ENDS
PUBLIC	_WOW16DefHookProc@16
;	COMDAT _WOW16DefHookProc@16
_TEXT	SEGMENT
_nCode$ = 8
_wParam$ = 12
_lParam$ = 16
_hhk$ = 20
_WOW16DefHookProc@16 PROC FAR				; COMDAT
; Line 1073
	push	ebp
	mov	ebp, esp
; Line 1074
	push	DWORD PTR _lParam$[esp]
	push	DWORD PTR _wParam$[bp]
	push	DWORD PTR _nCode$[bp]
	push	DWORD PTR _hhk$[bp]
	call	_CallNextHookEx@16
; Line 1075
	pop	ebp
	ret	16					; 00000010H
_WOW16DefHookProc@16 ENDP
_TEXT	ENDS
END
