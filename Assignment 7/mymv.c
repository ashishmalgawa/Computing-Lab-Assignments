#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
void get_cwd(char *cwd){
    FILE *fp;
    fp = fopen("cwd.txt", "r");
    fgets(cwd, 1024, (FILE*)fp);
    fclose(fp);
}
int isDirectory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}
int main(int args,char **argv){
    if(args>3){//multiple files one directory
        char cwd[1024];
        //converting relative to absolute path
        if(argv[args-1][0]!='/'){
            get_cwd(cwd);
            strcat(cwd,"/");
            strcat(cwd,argv[args-1]);
        }else{
            strcpy(cwd,argv[args-1]);
        }
        if(isDirectory(cwd)){//if last argument is a directory
            for(int i=1;i<args-1;i++){
                char arg[1024],arg2[1024];
                //converting relative to absolute path
                if(argv[args-1][0]!='/'){
                    //converting relative to absolute path
                    get_cwd(arg);
                    strcat(arg,"/");
                    strcat(arg,argv[i]);
                }else{
                    strcpy(arg,argv[i]);
                }
                //converting relative to absolute path
                strcpy(arg2,cwd);
                strcat(arg2,"/");
                strcat(arg2,argv[i]);
                //converted
                printf("%s\n %s \n",arg,arg2);
                if(rename(arg,arg2)==0){
                    printf("Succefully moved\n");
                }else{
                    perror("mv error");
                }
            }
        }else{
            printf("\n Last argument should be a directory in case of multiple files");
        }
        exit(0);
    }
    if(args<3){
        printf("There should be atleast 3 arguments");
        exit(0);
    }

    char arg[1024],cwd[1024];
    //converting relative to absolute path
    if(argv[1][0]!='/'){
        get_cwd(arg);
        strcat(arg,"/");
        strcat(arg,argv[1]);
    }else{
        strcpy(arg,argv[1]);
    }
    //converting relative to absolute path
    if(argv[2][0]!='/'){
        get_cwd(cwd);
        strcat(cwd,"/");
        strcat(cwd,argv[2]);
    }else{
        strcpy(arg,argv[2]);
    }
    if(rename(arg,cwd)==0){
        printf("Succefully moved\n");
    }else{
        perror("mv error");
    }
    return 0;
}