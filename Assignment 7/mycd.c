#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
void get_workingdir(char *cwd){
    FILE *fp;
    fp = fopen("working_dir.txt", "r");
    fgets(cwd, 1024, (FILE*)fp);
    printf("in cwd:%s\n",cwd);
    fclose(fp);
}

void set_cwd(char *cwd){
    FILE *fp;

    fp = fopen("cwd.txt", "w+");
    fprintf(fp, cwd);
    fclose(fp);
}
void get_cwd(char *cwd){
    FILE *fp;
    fp = fopen("cwd.txt", "r");
    fgets(cwd, 1024, (FILE*)fp);
    fclose(fp);
}
int main(int arg,char **args) {
    char cwd[1024];
    char *pointer=NULL;
    char old_cwd[1024];//setting file value to working directory
    get_workingdir(old_cwd);//getting working directory of the main program
    get_cwd(cwd);//getting current working directory of myshell
    if(arg>2){
        printf("\n Only one argument is allowed with cd\n");
        exit(0);
    }
    chdir(cwd);//going to current working directory according to file
    if(chdir(args[1])==0){//changing to directory specified by user
        if (getcwd(cwd, sizeof(cwd)) == NULL){
            perror("cwd error");
            exit(0);
        }
        chdir(old_cwd);//changing back to old working directory
        set_cwd(cwd);//setting current working directory of myshell to new path
    }else{
        perror("cd error");
    }
    free(pointer);
    fflush(stdout);
    return 0;
}
