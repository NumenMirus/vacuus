rm ./a.out
g++ lib/Base64.cpp main.cpp -g -O0
valgrind --leak-check=full ./a.out