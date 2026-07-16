extrn ExitProcess: proc
.code
main proc
    mov rax, 0      ;код возврата
    call ExitProcess
main endp
end
