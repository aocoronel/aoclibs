PROGRAMS = printh bashgen zshgen normalize_shell_name

all: $(PROGRAMS)

.PHONY: printh
printh:
	gcc -I../../ printh.c ../../src/cli/printh.c ../../src/mem/*.c ../../src/error/*.c ../../src/io/print_indent.c -o printh

.PHONY: bashgen
bashgen:
	gcc bashgen.c -I../../ ../../src/mem/* ../../src/error/* ../../src/cli/bashgen.c ../../src/cli/normalize_shell_name.c -o bashgen

.PHONY: zshgen
zshgen:
	gcc zshgen.c -I../../ ../../src/mem/* ../../src/error/* ../../src/cli/zshgen.c ../../src/cli/normalize_shell_name.c -o zshgen

.PHONY: normalize_shell_name
normalize_shell_name:
	gcc normalize_shell_name.c -I../../ ../../src/cli/zshgen.c ../../src/cli/normalize_shell_name.c -o normalize_shell_name
