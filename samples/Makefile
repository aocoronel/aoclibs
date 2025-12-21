CC = clang

SRC_DIR = ../build
SAMPLE_DIR = .
INCLUDE = -I../

LIBS = $(SRC_DIR)/libaoclibs.a
CFLAGS = -Wno-nullability-completeness $(INCLUDE)

SRCS = $(wildcard $(SAMPLE_DIR)/*.c)
BINS = $(SRCS:.c=.out)

all: $(BINS)

.PHONY: all clean

%.out: %.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

clean:
	rm -f $(BINS)
