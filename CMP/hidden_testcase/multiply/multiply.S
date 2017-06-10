PC init at 00000068
following 27 instructions

lh     $4 0($0)
lh     $5 2($0)
or     $3 $5 $0
and    $2 $0 $0
addi   $8 $0 C=32
slt    $9 $0 $8
beq    $0 $9 L=13
andi   $9 $3 C=1
beq    $0 $9 L=4
bne    $8 $9 L=2
sub    $2 $2 $4
beq    $0 $0 L=1
add    $2 $2 $4
srl    $3 $3 C=1
andi   $9 $2 C=1
sll    $9 $9 C=31
or     $3 $3 $9
sra    $2 $2 C=1
addi   $8 $8 C=-1
beq    $0 $0 L=-15
sw     $3 4($0)
lw     $1 4($0)
halt
halt
halt
halt
halt
