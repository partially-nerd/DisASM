decv msg str
set msg "Hello World"
outch msg
outch 10

decv lower str
set lower msg
low lower
outch lower
outch 10

decv upper str
set upper msg
upr upper
outch upper
outch 10

decv betnTerminals str
set betnTerminals msg
mid 1 betnTerminals -1
outch betnTerminals
outch 10

decv lenOrig int
decv lenBetn int
len msg, betnTerminals lenOrig, lenBetn

outch "Original Length = "
out lenOrig

outch 10, "BetnTerminals Length = "
out lenBetn
outch 10

exit
