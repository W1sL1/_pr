.MODEL SMALL
.STACK 100h
.DATA
; ваши данные
.CODE
main:
    ; ваш код
    mov ax, 4c00h
    int 21h
END main
