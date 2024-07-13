CC=gcc
CFLAGS = -std=c11 -Wall -Werror -Wextra

EXEC = SimpleBash
VERSION = 1.0
BRANCH = develop

CLANG = clang-format -style=Google

SOURCECAT = $(wildcard cat/s21_*.c)
SOURCEGREP = $(wildcard grep/s21_*.c)

EXECCAT = s21_cat
EXECGREP = s21_grep

all: clean $(EXECCAT) $(EXECGREP)

clean:
	rm -rf cat/$(EXECCAT)
	rm -rf grep/$(EXECGREP)

$(EXECCAT): $(SOURCECAT)
	$(CC) $(CFLAGS) $^ -o cat/$@

$(EXECGREP): $(SOURCEGREP)
	$(CC) $(CFLAGS) $^ -o grep/$@

test:
	bash common/TestScript

format:
	$(CLANG) -n $(SOURCECAT) $(SOURCEGREP)

setformat:
	$(CLANG) -i $(SOURCECAT) $(SOURCEGREP)
commit:
	@read -p "Enter commit message: " input; \
	git add -A; \
	git commit -m "$$input - $(EXEC) v:$(VERSION) - $(shell date +'%Y-%m-%d %H:%M')"; \
	git push origin $(BRANCH)