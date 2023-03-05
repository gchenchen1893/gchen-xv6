#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

// process 1[WR] -> process 2[RD]
const uint INT_LEN = sizeof(int);

int get_first_left(int lpipe[2], int* first) {
	if (read(lpipe[RD], first, INT_LEN) == INT_LEN) {
		printf("prime %d\n", *first);
		return 0;
	}
	return -1;
}

// Push other primes to the right pipe
void push_others_to_right(int lpipe[2], int rpipe[2], int first) {
	int data;
	while (read(lpipe[RD], &data, INT_LEN) == INT_LEN) {
		if (data % first != 0) {
			write(rpipe[WR], &data, INT_LEN);
		}
	}
	close(lpipe[RD]);
	close(rpipe[WR]);
}

void primes(int lpipe[2]) {
	// the first ele of left pipe is a prime number
	// push others to the right pipe recursively.
	// 
	close(lpipe[WR]);
	int first;
	if (get_first_left(lpipe, &first) == 0) {
		int rpipe[2];
		pipe(rpipe);
		push_others_to_right(lpipe, rpipe, first);

		int pid = fork();
		if (pid == 0) {
			primes(rpipe);
		} else {
			close(rpipe[RD]);
			wait(0);
		}
	}
	exit(0);
}

int main(int argc, char const* argv[]) {
	int p[2];
	int exit_code = 0;
	pipe(p);
	for (int i = 2; i <= 35; i++) {
		if (write(p[WR], &i, INT_LEN) != INT_LEN) {
			fprintf(2, "Failed to write\n");
			exit_code = 1;
		}
	}

	int pid = fork();
	if (pid == 0) {
		primes(p);
	} else if (pid == 0) {
		close(p[WR]);
		close(p[RD]);
		wait(0);
	}

	exit(exit_code);
}
