.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
	li $v0, 4
	la $a0, _prompt
	syscall
	li $v0, 5
	syscall
	jr $ra

write:
	li $v0, 1
	syscall
	li $v0, 4
	la $a0, _ret
	syscall
	move $v0, $0
	jr $ra

main:
	li $t0, 0
	move $t1, $t0
	li $t2, 1
	move $t3, $t2
	li $t4, 0
	move $t5, $t4
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	addi $sp, $sp, 4
	move $t6, $v0
	move $t7, $t6
label1:
	blt $t5, $t7, label2
	j label3
label2:
	move $t8, $t1
	move $t9, $t3
	add $s0, $t8, $t9
	move $s1, $s0
	move $s2, $t3
	move $a0, $s2
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	addi $sp, $sp, 4
	move $s3, $t3
	move $t1, $s3
	move $s4, $s1
	move $t3, $s4
	move $s5, $t5
	li $s6, 1
	add $s7, $s5, $s6
	move $t5, $s7
	j label1
label3:
	li $t0, 0
	move $v0, $t0
	jr $ra
