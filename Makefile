CFLAGS = -msse2 --std gnu99 -O0 -Wall -Wextra

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) main.c
	$(CC) $(CFLAGS) -o main main.c -DVER="ALL"

naive: main main.c impl.c
	$(CC) $(CFLAGS) -o main_naive main.c -DVER="NAIVE"

sse: main main.c impl.c
	$(CC) $(CFLAGS) -o main_sse main.c -DVER="SSE"

sse_prefetch: main main.c impl.c
	$(CC) $(CFLAGS) -o main_sse_prefetch main.c -DVER="SSE_PREFETCH"

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

clean:
	$(RM) main
