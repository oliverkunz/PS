#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define BUF_SIZE 80

int is_regular_file(const char*);
char* trim(char* input);
char* ltrim(char* input);
char* rtrim(char* input);
char* remove_linebreak(char* name);
char* remove_kB(char* name);

int main(){
   // printf(" %5s %-15s %5s\n", "PID", "COMMAND", "RSS"); //format is correct not used for test.sh

    int euid = geteuid(); // get effective user id
    char uid[10];
    sprintf(uid, "%d", euid);
    DIR *dir;
    struct dirent *de;
    FILE *fptr;
    struct dirent *innerDe;
    DIR *innerdir;
    dir = opendir ("/proc");
    while (NULL != (de = readdir (dir))) {
        char directory[120] = "/proc/";
        strcat(directory, de->d_name);
        if (strcmp(de->d_name, ".") != 0
            && strcmp(de->d_name, "..") != 0
            && is_regular_file(directory) < 1 && strcmp(de->d_name, "self") && strcmp(de->d_name, "thread-self")) {
            innerdir = opendir(directory);
            while (NULL != (innerDe = readdir (innerdir))) {

                if (strcmp(innerDe->d_name, "status") == 0)  { //prüfen ob es das status directory ist bevor öffnen
		    char pid[50];
                    char rss[50];
                    char name[50];
                    bool rightUid = false;
   

                    strcat(directory, "/");
                    strcat(directory, "status");
                    if ((fptr = fopen(directory, "r")) == NULL) { //absolute path needed
                        printf("Error! opening %s\n", "status");
                        // Program exits if file pointer returns NULL.
                         exit(1);
                    }else {

                        char line[256];
                        while (fgets(line, sizeof(line), fptr)) { //read through the file
                                if (strstr(line,"Uid:") != NULL) { 
                                    if (strstr(line, uid) != NULL) {
                                        rightUid = true;
                                        strcpy(pid, de->d_name);
                                    }
                                } else {
                                    if (strstr(line,"Name:") != NULL) {
                                        strtok(line,":");
                                        strcpy(name, strtok(NULL, ":"));
                                    }
                                    if (strstr(line,"VmRSS:") != NULL) {
                                        strtok(line,":");
                                        strcpy(rss, strtok(NULL, ":")); 
                                    }
                                }
                        }
                        fclose(fptr);

                        if (rightUid) {

                            printf(" %5s %-15s %5s\n", pid, trim(name), trim(remove_kB(rss)));
                        }
                    }
                }
            }
            closedir (innerdir);
        }
    }
    closedir(dir);
}
char *remove_kB(char *name) {
    for(int i = 0; i < strlen(name); i++) {
        if (name[i] == 'k' || name[i] == 'B' || name[i] == ' ') {
            name[i] = ' ';
        }
    }
    return name;
}

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s)); 
}
