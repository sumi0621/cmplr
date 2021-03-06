#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./cmplr "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4  '(3+5)/2;'
assert 10 '-10+20;'
assert 1  "10+20 == 15+15;"
assert 1  "10+20 != 1;"
assert 0  "1 == 0;"
assert 1  "2 > 1;"
assert 1  "1*2 < 3 * 2 + 1;"
assert 1  "1 * 2 >= 1 * 2;"
assert 1  "1 * 3 >= 2;"
assert 1  "1 + 2 <= 3;"
assert 1  "1 + 2 <= 4;"
assert 14 "a = 3; b = 5 * 6 - 8;a + b / 2;"
assert 11 "abc = 1; foooo_11133 = 10; abc + foooo_11133;"
assert 1  "return 1;"
assert 9 "return 1 + 2 * 4;"
assert 11 "abc = 1; foooo_11133 = 10; aaa = abc + foooo_11133;return aaa;"
assert 2  "if(1 == 1)2;"
assert 4  "if (2 == 2)if(1 != 3)4;"
assert 3  "a=3;if(1 ==2)a=4;return a;"
assert 2  "if(1 ==2)return 1;else return 2;"
assert 1  "a = 2;if(a == 3)return 10;else return a - 1;"
assert 5  "a = 0;while(a < 5)a=a+1;return a;"
assert 10 "for (a = 0; a < 10;) a = a + 1; return a;"
assert 10 "for (a = 0; a < 10;a = a + 1) b ; return a;"
assert 10 "a = 0;for (; a < 10;a = a + 1) b ; return a;"

echo OK