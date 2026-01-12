decf fibo x, y, depth, maxD int
  decv m int
  inc depth

  add x y m
  set x, y y, m

  out m
  outch space

  // exit if depth 10
  // the second param to decl is the offset from l1 when labelling l1
  decl l1 2
  cmpe depth maxD l1
  call fibo x, y, depth, maxD
endf

decv space int
set space 32

out 1
outch space
out 1
outch space

call fibo 1, 1, 1, 10

exit
