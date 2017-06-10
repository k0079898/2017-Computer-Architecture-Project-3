  lw    $1, 0($0)
  addi  $2, $0, 1
  addi  $3, $0, 1
A:add   $2, $2, $3
  bne   $1, $2, A
  lw    $5, 11($1)
  add   $1, $1, $5
B:addi  $2, $2, 2
  bne   $1, $2, B
  sub   $6, $1, $2
  sub   $6, $1, $5
  halt
  halt
  halt
  halt
