addi	$1, $0, 0x7FFF
sll		$1, $1, 16
ori		$1, $1, 0xFFFF 		#$1 = 7FFFFFFF
addi	$2, $0, 6      		#$2 = 6
add		$3, $1, $2			#overflow
slt		$4, $0, $3			#$3 > 0, then branch
bgtz	$4, 4
sw		$3, 0($5)
addi	$5, $5, 4
addi	$3, $3, -1			#let $3 become positive number
beq 	$0, $0, -6
add		$0, $0, $2     		#write to register 0 error
lw		$6, 0($5)
lw		$7, -4($5)
lw		$8, -8($5)
lw		$9, -12($5)
lw		$10, -16($5)
lw		$11, -25($5)
halt
halt
halt
halt
halt