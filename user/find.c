#include "kernel/types.h"

#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char* path, const char* filename) {
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	fd = open(path, 0);
	if (fd < 0) {
		fprintf(2, "failed to open path: %s\n", path);
		return;
	}

	if (fstat(fd, &st) < 0) {
		fprintf(2, "failed to fetch file stats: %s\n", path);
		close(fd);
		return;
	}

	// path must be a directory
	if (st.type != T_DIR) {
		fprintf(2, "usage: find <DIRECTORY> <filename>\n");
		return;
	}

	if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
		printf("ls: path too long\n");
		return;
	}


	// copy prefix
	// p points to the last '/'
	strcpy(buf, path);
	p = buf + strlen(buf);
	*p = '/';
	p++;
	while (read(fd, &de, sizeof(de)) == sizeof(de)) {
		if (de.inum == 0) continue;
		// append subdir name to p
		memmove(p, de.name, DIRSIZ);
		p[DIRSIZ] = 0;
		if (stat(buf, &st) < 0) {
			printf("ls: cannot stat %s\n", buf);
			continue;
		}
		// stop recursive in '.' and '..'
		if (st.type == T_DIR && strcmp(p, ".") != 0 &&
		    strcmp(p, "..") != 0) {
			find(buf, filename);
		} else if (strcmp(p, filename) == 0) {
			printf("%s\n", buf);
		}
	}
	close(fd);
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		fprintf(2, "Not enough argument\n");
		exit(0);
	}

	find(argv[1], argv[2]);
	exit(0);
}
