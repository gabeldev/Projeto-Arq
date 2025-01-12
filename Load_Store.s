.data
	V: .word 10, 20, 30, 40, 50, 60, 70, 80, 90, 100

.text
	main: 
	
		la $t1, V
		lw $t2, 8($t1)
		lw $t3, 16($t1)
		
		addi $t1, $t1, 4
		add $t4, $t2, $t3
		sw $t4, 4($t1)
		
		
		sair:
			li $v0, 1
			lw $a0, 4($t1)
			syscall
			
			li $v0, 10
			syscall
		
		# lw $s1, 0($t1)
		


