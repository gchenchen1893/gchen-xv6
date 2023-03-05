#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

int main(int argc, char const *argv[]) {
	char buf = 'P';
	int p_p2c[2];
	int p_c2p[2];

	// p_c2p: child[WR] -> parent[RD]
	// p_p2c: parent[WR] -> child[RD]
	// fprinf, 0 -> standard input
	// 1 -> standard outour
	// 2 -> error output

	pipe(p_p2c);
	pipe(p_c2p);
	int pid = fork();
	int exit_code = 0;

	if (pid < 0) {
		fprintf(2, "failed to fork child process");
		close(p_p2c[0]);
		close(p_p2c[1]);
		close(p_c2p[0]);
		close(p_c2p[1]);
		exit(1);
	} else if (pid == 0) {
		// in child process, child reads/writes message from parent
		close(p_p2c[WR]);
		close(p_c2p[RD]);

		if (read(p_p2c[RD], &buf, sizeof(char)) != sizeof(char)) {
			fprintf(2, "failed to read from pipe\n");
			exit_code = 1;
		} else {
			fprintf(1, "%d: received ping\n", getpid());
		}

		if (write(p_c2p[WR], &buf, sizeof(char)) != sizeof(char)) {
			fprintf(2, "failed to write to pipe\n");
			exit_code = 1;
		}

		close(p_p2c[RD]);
		close(p_c2p[WR]);
		exit(exit_code);
	} else {
		// in parent process
		close(p_p2c[RD]);
		close(p_c2p[WR]);

		if (write(p_p2c[WR], &buf, sizeof(char)) != sizeof(char)) {
			fprintf(2, "failed to write to pipe\n");
			exit_code = 1;
		}

		if (read(p_c2p[RD], &buf, sizeof(char)) != sizeof(char)) {
			fprintf(2, "failed to read from pipe\n");
			exit_code = 1;
		} else {
			fprintf(1, "%d: received pong\n", getpid());
		}

		close(p_p2c[WR]);
		close(p_c2p[RD]);

		exit(exit_code);
	}
}
