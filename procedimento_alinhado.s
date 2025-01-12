.data
    message: .asciiz "Hello, World!"
    array: .word 1, 2, 3, 4, 5, 6, 7, 8, 9, 10

.text
    main:
        la $a0, message
        li $v0, 4
        syscall
        li $t1, 10
        lw $t2, array
        add $t0, $t1, $zero
        addi $s5, $t0, 2
        li $v0, 10
        syscall