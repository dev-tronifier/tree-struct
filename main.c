/**
 * TODO: Automatic repo and branch finder from .git/config. Format follows:-
 *	url = https://....
 *	url = https://....
 *
 *	[branch ".."]
 *	[branch ".."]
 * In case of multiple url/branches, give the user a prompt to choose.
 *
 * TODO: Write a CMakeLists.txt for the following and dividing the file into multiple
 *	modules for quick upgradation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

int is_dir(const char *path) {
	struct stat tmp;
	if(stat(path, &tmp) != 0)
		return 0;
	return S_ISDIR(tmp.st_mode);
}

#define SINGLE "└── "
#define BIDIRECT "├── "
#define INTER "│"
#define SPACE "   "

#define __bit(x, i) ((x & (1 << i)) >> i)
#define __bitoff(x, i) (x & ~(1 << i))
#define __biton(x, i) (x | (1 << i))

char SHOW_HIDDEN = 0;

#define ls(__LIST, __PATH) scandir(__PATH, &__LIST, 0, alphasort);

int dir = 0;

char * branch = NULL;
char * repo = NULL;

int level = 0;

void print_data(char *path) {
	struct dirent **list = NULL;
	char *tmp;
	char *loc = NULL;
	int n = ls(list, path);
	if(n < 0)
		return;
	for(int i = 0; i < n; i++) {
		if(strcmp(list[i]->d_name, ".") != 0
		&& strcmp(list[i]->d_name, "..") != 0) {

			level = i == n-1 ? __bitoff(level, dir) : __biton(level, dir);

			if(list[i]->d_name[0] == '.' && !SHOW_HIDDEN) {
				goto skip;
			}
			for(int j = 0; j < dir; j++) {
					printf("%s%s", __bit(level, j) ? INTER : " ", SPACE);
			}


                        tmp = malloc(strlen(path) + strlen(list[i]->d_name) + 2);
                        strcpy(tmp, path);
                        strcat(tmp, "/");
                        strcat(tmp, list[i]->d_name);

			loc = malloc(strlen(repo) + strlen(branch) + 8 + strlen(tmp));
			strcpy(loc, repo);
			strcat(loc, is_dir(tmp) ? "/tree/" : "/blob/");
			strcat(loc, branch);
			strcat(loc, tmp+1);

			printf("%s", i==n-1 ? SINGLE: BIDIRECT);

			printf("<a href = %s>%s</a>\n", loc, list[i]->d_name);

			if(is_dir(tmp)) {
				dir++;
				print_data(tmp);
				dir--;
			}
			free(tmp);
			free(loc);
		}

skip:
		free(list[i]);
	}
	free(list);
}

int main(int argc, char **argv)
{
	if(argc > 1)
		if(strcmp(argv[1], "show-hidden") == 0)
			SHOW_HIDDEN = 1;

	repo = argv[1];
	branch = argv[2];

	print_data(".");
	return 0;
}
