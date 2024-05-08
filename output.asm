extern _write: PROC
.data
.code
buffer$ EQU 32
x$ EQU 80
dump    PROC		; COMDAT
$LN10:
    sub     rsp, 72		; 00000048H
    mov     r8d, 1
    mov     BYTE PTR buffer$[rsp + 31], 10
    lea     r10, QWORD PTR buffer$[rsp + 30]
    mov     r11, -3689348814741910323	; cccccccccccccccdH
    align   2
$LL4@dump:
    mov     rax, r11
    lea     r10, QWORD PTR[r10 - 1]
    mul     rcx
    inc     r8
    shr     rdx, 3
    movzx   eax, dl
    shl     al, 2
    lea     r9d, DWORD PTR[rax + rdx]
    add     r9b, r9b
    sub     cl, r9b
    add     cl, 48		; 00000030H
    mov     BYTE PTR[r10 + 1], cl
    mov     rcx, rdx
    test    rdx, rdx
    jne     SHORT $LL4@dump
    lea     rdx, QWORD PTR buffer$[rsp + 32]
    mov     ecx, 1
    sub     rdx, r8
    call    _write
    add     rsp, 72		; 00000048H
    ret     0
dump    ENDP
public _start
_start:
    ; -- push -- 40043
    push 40043
    ; -- push -- 40042
    push 40042
    ; -- plus -- 
    pop rax
    pop rbx
    add rax, rbx
    push rax
    ; -- dump -- 
    pop rcx
    call dump
    ; -- push -- 420
    push 420
    ; -- push -- 413
    push 413
    ; -- minus -- 
    pop rax
    pop rbx
    sub rbx, rax
    push rbx
    ; -- dump -- 
    pop rcx
    call dump
    ; -- push -- 10
    push 10
    ; -- push -- 20
    push 20
    ; -- plus -- 
    pop rax
    pop rbx
    add rax, rbx
    push rax
    ; -- dump -- 
    pop rcx
    call dump
    ret
end
