#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <libgen.h>
#include <pwd.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
typedef long long int num;
int gettimesinceboot() {
    long Hertz=sysconf(_SC_CLK_TCK);
    FILE *procuptime;
    int sec, ssec;

    procuptime = fopen("/proc/uptime", "r");
    fscanf(procuptime, "%d.%ds", &sec, &ssec);
    fclose(procuptime);
    return (int)((sec*Hertz) +ssec);
}


int check_if_number (char *str)
{
    int i;
    for (i=0; str[i] != '\0'; i++)
    {
        if (!isdigit (str[i]))
        {
            return 0;
        }
    }
    return 1;
}

const char *get_username(int uid)
{
    struct passwd *pw = getpwuid(uid);
    if (pw)
    {
        return pw->pw_name;
    }

    return "";
}


int main (int argc, char *argv[])
{
    printf("%8s %6s %10s %10s %7s %7s %5s %10s [%s]\n","USERNAME", "PID","CPU_USAGE%","MEM_USAGE%", "VM_SIZE", "VM_RSS", "STATE", "START_TIME", "COMMAND");
    DIR *dirp;
    FILE *fp;
    struct dirent *pid_entry;
    char path[1024], read_buf[1024],temp_buf[1024];
    char uid_int_str[6]={0},*line;
    char uptime_str[10];
    char *user;
    size_t len=0;
    dirp = opendir ("/proc/");
    if (dirp == NULL)
    {
        perror ("proc error");
        exit(0);
    }
    strcpy(path,"/proc/");
    strcat(path,"uptime");

    fp=fopen(path,"r");
    if(fp!=NULL)
    {
        getline(&line,&len,fp);
        sscanf(line,"%s ",uptime_str);
    }

    long uptime=atof(uptime_str);
    long Hertz=sysconf(_SC_CLK_TCK);
    strcpy(path,"/proc/");
    strcat(path,"meminfo");

    fp=fopen(path,"r");
    unsigned long long total_memory;
    if(fp!=NULL)
    {
        getline(&line,&len,fp);
        sscanf(line,"MemTotal:        %llu kB",&total_memory);
    }

    while ((pid_entry = readdir (dirp)) != NULL)
    {
        if (check_if_number (pid_entry->d_name))
        {
            strcpy(path,"/proc/");
            strcat(path,pid_entry->d_name);
            strcat(path,"/status");
            unsigned long long vm_rss;
            fp=fopen(path,"r");
            unsigned long long vm_size;

            if(fp!=NULL)
            {   int i;
                vm_size=0;
                for(i=0;i<8;i++)
                    getline(&line,&len,fp);//skipping till nth line
                sscanf(line,"Uid:    %s ",uid_int_str);//scanning nth line where particular value is stored
                for(;i<17;i++)
                    getline(&line,&len,fp);
                sscanf(line,"VmSize:    %llu kB",&vm_size);
                for(;i<21;i++)
                    getline(&line,&len,fp);
                sscanf(line,"VmRSS:\t    %llu kB",&vm_rss);
            }
            else
            {
                fprintf(stdout,"FP is NULL\n");
            }
            int current_user=getuid();
            if(current_user!=atoi(uid_int_str))
                continue;//displaying processes only of current user
            float memory_usage=100*vm_rss/total_memory;
            strcpy (path, "/proc/");
            strcat (path, pid_entry->d_name);
            strcat (path, "/comm");

            fp = fopen (path, "r");
            if (fp != NULL)
            {
                fscanf (fp, "%s", read_buf);
                fclose(fp);
            }
            strcpy(path,"/proc/");
            strcat(path,pid_entry->d_name);
            strcat(path,"/stat");
            fp=fopen(path,"r");
            getline(&line,&len,fp);
            char comm[10],state;
            unsigned int flags;
            int pid,ppid,pgrp,session,tty_nr,tpgid;
            unsigned long minflt,cminflt,majflt,cmajflt,utime,stime;
            unsigned long long starttime;
            long cutime,cstime,priority,nice,num_threads,itreavalue;
            int skip=(int)strlen(pid_entry->d_name)+(int)strlen(read_buf)+4;
            sscanf(&line[skip],"%c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld  %ld %llu",&state,&ppid,&pgrp,&session,&tty_nr,&tpgid,&flags,&minflt,&cminflt,&majflt,&cmajflt,&utime,&stime,&cutime,&cstime,&priority,&nice,&num_threads,&itreavalue,&starttime);
            unsigned long total_time=utime+stime;
            total_time=total_time+(unsigned long)cutime+(unsigned long)cstime;
            float seconds=uptime-(starttime/Hertz);
            float cpu_usage=100*((total_time/Hertz)/seconds);
            if(isnan(cpu_usage))//if entry is missing in proc
            {
                cpu_usage=0.0;
            }
            if(isnan(memory_usage))//if entry is missing in proc
            {
                memory_usage=0.0;
            }

            char *userName= get_username(atoi(uid_int_str));
            if(strlen(userName)<9)
            {
                user=userName;
            }
            else
            {
                user=uid_int_str;
            }
            int sinceboot = gettimesinceboot();
            int running = (int) (sinceboot - starttime);
            time_t rt = time(NULL) - (running / Hertz);
            char start_time[1024];

            strftime(start_time, sizeof(start_time), "%H:%M", localtime(&rt));
            fprintf(stdout,"%8s %6s %10.1f %10.1f %7llu %7llu %5c %10s [%s]\n",user,pid_entry->d_name,cpu_usage,memory_usage,vm_size,vm_rss,state,start_time,read_buf);

        }
    }
    closedir (dirp);
    return 0;
}
