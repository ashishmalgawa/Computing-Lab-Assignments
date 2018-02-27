#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

//defining color constants
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"
void freeArgumentArray(char **args,int spacecount){
    for(int i=0;i<spacecount;i++){
        free(args[i]);//frees each element of the array
    }
    free(args);//frees the array
}
char ** createArgumentArray(char **args,char *line,int spacecount,int startIndex,int stopIndex){//creates a dynamic array of arguments
    int i;
    args=(char**)malloc(sizeof(char*)*spacecount);
    int prev_space=startIndex-1,j=0;
    int space_count2=0;
    for(i = startIndex; i < stopIndex && line[i] != '\n'; ++i){
        if(line[i]==' ') {
            space_count2++;
            prev_space++;
            char *arg = (char *) malloc(sizeof(char *) * (i - prev_space+1));
            for (int k = 0; k <(i-prev_space) ; ++k) {//copies all characters from previous space to current space
                arg[k]=line[prev_space+k];
            }
            arg[i-prev_space]='\0';//adds \0 to argument string to end the string
            args[j] = arg;//adds argument to dynamic argument array
            prev_space = i;
            j++;
            if(space_count2==spacecount)//if we have reached last space then break now last argument will end with \n
                break;
        }
    }
    //for last argument
    prev_space++;
    char *arg = (char *) malloc(sizeof(char *) * (i - prev_space + 1));
    for (int k = 0; k <(i-prev_space) ; ++k) {
        arg[k]=line[prev_space+k];
    }
    arg[i-prev_space]='\0';
    args[j] = arg;
    j++;
    args[j]=NULL;//adding NULL in the end of argument array for execvp
    return args;
}
void set_cwd(char *cwd){//sets current working directory
    FILE *fp;

    fp = fopen("cwd.txt", "w+");
    fprintf(fp, cwd);
    fclose(fp);
}
void get_cwd(char *cwd){//gets current working directory
    FILE *fp;
    fp = fopen("cwd.txt", "r");
    fgets(cwd, 1024, (FILE*)fp);
    fclose(fp);
}
void set_workingdir(char *cwd){//sets working directory of program
    FILE *fp;

    fp = fopen("working_dir.txt", "w+");
    fprintf(fp, cwd);
    fclose(fp);
}
void command_loop(){
    char *line = NULL;
    short pipeFlag;
    int pipeIndex=0,pipeSpaceCount=0;
    ssize_t bufsize = 0; // have getline allocate a buffer for us
    while(1){
	pipeFlag=0;
        char cwd[1024];
        get_cwd(cwd);
        fflush(stdout);
        printf(ANSI_COLOR_BLUE"\n$%s:>"ANSI_COLOR_RESET,cwd);
        getline(&line, &bufsize, stdin);//get input from user
        if(strcmp(line,"myexit\n")==0){
            break;
        }

        int i,spacecount=0,cmd_len=0;
        for(i = 0; line[i] != '\0'; ++i){
            if(line[i]==' ') {
                spacecount++;
                if (line[i+1]=='|' && line[i+2]==' '){
                    pipeFlag=1;
                    pipeSpaceCount=spacecount;//stores after how many words pipe is there
                    pipeIndex=i+3;//stores after how many indices new argument array should be build for execvp
                }
            }if(spacecount==1&&line[i]==' ')
                cmd_len=i;
        }
        spacecount+=2;//1 for last argument
        if(cmd_len==0)
            cmd_len=i-1;//if there is no space
        char **args;
        char **argsPipe;
        if(pipeFlag){
            args=createArgumentArray(args,line,pipeSpaceCount+1,0,pipeIndex-3);//creates argument array before pipe
            argsPipe=createArgumentArray(argsPipe,line,spacecount-pipeSpaceCount-1,pipeIndex,strlen(line)+1);//creates argument array after pipe
        }else{
            args=createArgumentArray(args,line,spacecount,0,strlen(line)+1);
        }
        int doablePipe=0;
        if(pipeFlag){
            if((strcmp(args[0],"mytail")==0)||(strcmp(args[0],"myps")==0)||(strcmp(args[0],"mypwd")==0)||(strcmp(args[0],"myls")==0)||(strcmp(args[0],"mycat")==0)){
                if((strcmp(argsPipe[0],"mytail")==0)||(strcmp(argsPipe[0],"mycat")==0))
                    doablePipe=1;
            }
        }
        if (pipeFlag&&doablePipe){
            int fds[2];
            pipe (fds);//creates pipe
            /*----------------------------------
            * appends ./ in command
            ---------------------------------- */
            char *cmd=(char*)malloc(sizeof(char)*(cmd_len+3)); //command_length=i-1 (+2) for ./ +1 for \0
            cmd[0]='.';
            cmd[1]='/';
            strcpy(cmd,"./");
            strcat(cmd,args[0]);
            /*----------------------------------
             * forks a child process to run given command
             ---------------------------------- */
            int pid=fork();
            if(pid==0){
                /* This is the child process. Closes our copy of the read end of
                the file descriptor. */
                close (fds[0]);
                /* Connect the write end of the pipe to standard output. */
                dup2 (fds[1], STDOUT_FILENO);
                if(execvp(cmd,args)<0)
                    printf("\n Error! Invalid command\n");
                fflush(stdout);
                exit(0);
            }
            wait(NULL);//waiting for child
            free(cmd);//frees the command

            /*----------------------------------
             * appends ./ in for second command after command
             ---------------------------------- */
            cmd=(char*)malloc(sizeof(char)*(cmd_len+3)); //command_length=i-1 (+2) for ./ +1 for \0
            cmd[0]='.';
            cmd[1]='/';
            strcpy(cmd,"./");
            strcat(cmd,argsPipe[0]);
            /*----------------------------------
             * forks a child process to run given command
             ---------------------------------- */

                close (fds[1]);//closes write stream so that read stream doesn't expect any input from it and returns -1 on scanf and 0 on read
            pid=fork();
            if(pid==0){
                /* This is the child process. Close our copy of the write end of
                the file descriptor. */
                close (fds[1]);
                /* Connect the read end of the pipe to standard input. */
                dup2 (fds[0], STDIN_FILENO);

                if(execvp(cmd,argsPipe)<0)
                    printf("\n Error! Invalid command\n");
                fflush(stdout);
                exit(0);
            }
            wait(NULL);//waiting for child
            free(cmd);//frees the command
            close (fds[0]);

        }else if(pipeFlag){
            printf("\n Invalid set of commands in pipe\n");
        }

        if(!pipeFlag){//if there is no pipe
            /*----------------------------------
         * appends ./ in command
         ---------------------------------- */
            char *cmd=(char*)malloc(sizeof(char)*(cmd_len+3)); //command_length=i-1 (+2) for ./ +1 for \0
            cmd[0]='.';
            cmd[1]='/';
            strcpy(cmd,"./");
            strcat(cmd,args[0]);
            /*----------------------------------
             * forks a child process to run given command
             ---------------------------------- */
            int fds[2];
            pipe (fds);
            close (fds[1]);//closes write stream so that read stream doesn't expect any input from it and returns -1 on scanf
            //in this case the program will understand that it should not read data from the pipe
            int pid=fork();
            if(pid==0){
                /* This is the child process. Close our copy of the write end of
                the file descriptor. */
                close (fds[1]);
                /* Connect the read end of the pipe to standard input. */
                dup2 (fds[0], STDIN_FILENO);
                if(execvp(cmd,args)<0)
                    printf("\n Error! Invalid command\n");
                fflush(stdout);
                exit(0);
            }
            wait(NULL);//waiting for child
            free(cmd);//frees the command
            /*----------------------------------
             * appends ./ in command
             ---------------------------------- */

        }
        /*----------------------------------
          frees dynamic allocated memory
        ----------------------------------*/
         if(pipeFlag){
            freeArgumentArray(args,pipeSpaceCount+1);
            freeArgumentArray(argsPipe,spacecount-pipeSpaceCount-1);
        }else{
            freeArgumentArray(args,spacecount);
        }

    }

}

int main() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) //checking if program is able to get current working directory
    {
        set_cwd(cwd);//setting current working directory
        set_workingdir(cwd);//setting the working directory of program
    }else{
        perror("getcwd() error:");
        exit(0);
    }
    command_loop();
    remove("cwd.txt");
    remove("working_dir.txt");
    return 0;
}
