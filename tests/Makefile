CC = clang

DIR_BUILD = ../build
DIR_INCLUDE = ../
DIR_TEST = ../tests

TEST_BIN = $(DIR_TEST)/test

LIBS = \
	$(DIR_BUILD)/libaoclibs.a \
	$(DIR_BUILD)/libdebug.a \
	$(DIR_BUILD)/libtunit.a

SAN ?= asan

ifeq ($(SAN),asan)
  SANITIZER_OPTIONS = -fsanitize=address,undefined
endif
ifeq ($(SAN),msan)
  SANITIZER_OPTIONS = -fsanitize=memory
endif

CFLAGS = \
	-Wno-nullability-completeness \
	-DDEBUG_HEAP \
	-DTUNIT \
	-I$(DIR_INCLUDE) \
	$(SANITIZER_OPTIONS)

LDFLAGS = \
	-L$(DIR_BUILD) \
	$(SANITIZER_OPTIONS)

TEST_SRCS = $(wildcard $(DIR_TEST)/*.c)
TEST_OBJS = $(TEST_SRCS:.c=.o)

all: $(TEST_BIN)

.PHONY: all clean

$(TEST_BIN): $(TEST_OBJS)
	$(CC) $^ $(LDFLAGS) $(LIBS) -o $@

$(DIR_TEST)/%.o: $(DIR_TEST)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TEST_OBJS) $(TEST_BIN)
