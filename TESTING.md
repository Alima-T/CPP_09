## Testing All Exercises

Complete test suite for all three exercises:

### ex00: Bitcoin Exchange

```bash
cd /Users/alima/Desktop/CPP_09/ex00
make fclean && make

# Basic cases:
./btc
./btc input.txt

# Nonexistent file:
./btc nofile.txt

# Date before first (before 2009-01-01) in DB:
printf "date | value\n2008-12-31 | 1\n" > t1.txt
./btc t1.txt
rm t1.txt

# Boundary range testing:
printf "date | value\n2011-01-03 | 0\n2011-01-03 | 1000\n2011-01-03 | 1000.01\n" > t2.txt
./btc t2.txt
rm t2.txt

# Invalid format:
printf "date | value\n2011-01-03, 3\nabcd\n" > t3.txt
./btc t3.txt
rm t3.txt

# Edge cases:
printf "date | value\n2011-01-03 | -5\n2011-01-03 | 2147483648\n" > t4.txt
./btc t4.txt
rm t4.txt
```

### ex01: RPN Calculator

```bash
cd /Users/alima/Desktop/CPP_09/ex01
make fclean && make

# Valid RPN expressions:
./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"
./RPN "7 7 * 7 -"
./RPN "1 2 * 2 / 2 * 2 4 - +"
./RPN "5 5 5 * +"
./RPN "10 2 /"
./RPN "3 4 + 2 *"

# Invalid expressions (should error):
./RPN "(1 + 1)"
./RPN "1 2 3"
./RPN "+"
./RPN "1 2 + 3"
./RPN ""

# No arguments:
./RPN
```

### ex02: PmergeMe

```bash
cd /Users/alima/Desktop/CPP_09/ex02
make fclean && make

# Basic cases:
./PmergeMe 3 2 1
./PmergeMe 1
./PmergeMe 10 4 8 2 7 3 9 1 6 5
./PmergeMe 5 5 2 9 2 9

# Invalid args:
./PmergeMe
./PmergeMe 0 2 1
./PmergeMe -1 2
./PmergeMe 2147483648 1
./PmergeMe 12abc 3
./PmergeMe +5 2
./PmergeMe "1 2" 3

# Random tests (macOS):
./PmergeMe $(jot -r 10 1 100)
./PmergeMe $(jot -r 100 1 100000)
./PmergeMe $(jot -r 1000 1 1000000)

# Random tests (Linux):
./PmergeMe $(shuf -i 1-100000 -n 100)
./PmergeMe $(shuf -i 1-1000000 -n 1000)
./PmergeMe $(shuf -i 1-1000000 -n 3000)

# Test from subject:
./PmergeMe $(jot -r 3000 1 1000000)
./PmergeMe $(shuf -i 1-1000000 -n 3000)

# Measure external execution time:
/usr/bin/time -p ./PmergeMe $(jot -r 3000 1 1000000)
/usr/bin/time -p ./PmergeMe $(shuf -i 1-1000000 -n 3000)

# Check for memory leaks (macOS):
leaks --atExit -- ./PmergeMe $(jot -r 200 1 100000)

# Check for memory leaks (Linux):
valgrind ./PmergeMe $(shuf -i 1-100000 -n 200)
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --errors-for-leak-kinds=all --error-exitcode=1 ./PmergeMe $(shuf -i 1-100000 -n 200)
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --errors-for-leak-kinds=all --error-exitcode=1 ./PmergeMe $(awk 'BEGIN{srand(); for(i=0;i<200;i++) printf "%d ", 1+int(rand()*100000)}')

# Verify return error codes:
./PmergeMe 0 2 1 >/dev/null 2>&1; echo $?
./PmergeMe -1 2 >/dev/null 2>&1; echo $?
```