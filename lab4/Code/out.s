.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
	li $v0, 4
	la $a0, _prompt
	syscall
	li $s0, _prompt
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
	move t0, t1
	li t1 , 0
	move t0, t1
	li t1 , 1
	move t0, t1
	li t1 , 0
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	addi $sp, $sp, 4
	move t1, $v0
	move t0, t1
label1:
	blt t0, t0, label2
	j label3
label1:
	move t0, t1
	move t1, t0
	move t1, t0
	add t1, t1, t1
	move t1, t0
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	addi $sp, $sp, 4
	move t1, t0
	move t0, t1
	move t1, t0
	move t0, t1
	move t1, t0
	li t1 , 1
	add t1, t1, t1
	move t0, t1
	j label1
label3:
	li t1 , 0
	move $v0, t1
	jr $ra
