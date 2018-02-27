#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
void get_cwd(char *cwd){
    FILE *fp;
    fp = fopen("cwd.txt", "r");
    fgets(cwd, 1024, (FILE*)fp);
    fclose(fp);
}
int main(int args,char **argv){
    for (int i = 1; i <args ; ++i) {
        if(argv[i][0]=='/') {//if path is absolute
            if(mkdir(argv[i],0700)==0){
                printf("Directory successfully created!\n");
            }else{
                perror("mkdir error");
            }
        }else{
            char cwd[1024];
            get_cwd(cwd);
            strcat(cwd,"/");
            strcat(cwd,argv[i]);
            //appending cwd to defined directory
            if(mkdir(cwd,0700)==0){
                printf("Directory successfully created!\n");
            }else{
                perror("mkdir error");
                fflush(stdout);
            }
        }

    }
    return 0;
}