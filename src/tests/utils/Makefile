CC = g++

SRC_COMMON := ../../unity/unity.c

DIR_UTILS := ../../utils

SRC_BASE64 := $(SRC_COMMON) $(DIR_UTILS)/base64url/base64url.cpp test_base64url.cpp
SRC_JSON := $(SRC_COMMON) $(DIR_UTILS)/json/json.cpp test_json.cpp
SRC_JSONDAF := $(SRC_COMMON) $(DIR_UTILS)/json/jsondaf.cpp $(DIR_UTILS)/json/json.cpp test_jsondaf.cpp

jwt: $(SRC_JWT)
	$(CC) $(CFLAGS) $(SRC_JWT) $(LDFLAGS) -o $@.out

base64: 
	$(CC) $(CFLAGS) $(SRC_BASE64) $(LDFLAGS) -o $@.out

json: 
	$(CC) $(CFLAGS) $(SRC_JSON) $(LDFLAGS) -o $@.out

jsondaf: 
	$(CC) $(CFLAGS) $(SRC_JSONDAF) $(LDFLAGS) -o $@.out

all:
	$(MAKE) base64
	$(MAKE) json
	$(MAKE) jsondaf

test_all:
	./jwt.out
	./base64.out
	./json.out
	./jsondaf.out

clear:
	rm *.out
