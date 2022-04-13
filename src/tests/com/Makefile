CC = g++

SRC_COMMON := ../../unity/unity.c

DIR_COM := ../../com

SRC_COM := $(SRC_COMMON) $(DIR_COM)/arq.cpp $(DIR_COM)/enquadramento.cpp $(DIR_COM)/pdafcdc.cpp ./test_com.cpp

com: $(SRC_COM)
	$(CC) $(CFLAGS) $(SRC_COM) $(LDFLAGS) -o $@.out

test:
	./com.out

clear:
	rm *.out
