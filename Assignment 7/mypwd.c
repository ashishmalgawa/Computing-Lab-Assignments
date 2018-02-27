#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#define ANSI_COLOR_GREEN   "\x1b[32m"
void get_cwd(char *cwd){
    FILE *fp;
    fp = fopen("cwd.txt", "r");
    fgets(cwd, 1024, (FILE*)fp);
    fclose(fp);
}
int main() {
    char cwd[1024];
    get_cwd(cwd);
    printf(ANSI_COLOR_GREEN "Current working dir: %s\n", cwd);
    return 0;
}
