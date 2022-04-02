output/bin/calc "with a: a*3" > ./output/bin/calc.expr
cat output/bin/calc.expr
llc ./output/bin/calc.expr

echo ''
echo '===================='
echo ''
# for macos, I compiled llvm source code and installed clang-15
#   system root is set by: -isysroot `xcrun --show-sdk-path`
clang-15 ./output/bin/calc.expr.s rtcalc.c  -isysroot `xcrun --show-sdk-path` -o ./output/bin/calc.out
./output/bin/calc.out

#   not sure if this will work on linux:
#       clang ./output/bin/calc.expr.s rtcalc.c -o ./output/bin/calc.out