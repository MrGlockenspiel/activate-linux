#include "options.h"
#include "log.h"
#include "osrelease.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char *get_release_info()
{
    const char *PRETTY="PRETTY_NAME";
    const char *SIMPLE="NAME";

    FILE* osrelease;

    char *line, *pretty, *fallback;
    size_t size;
    ssize_t nread;
    line=NULL;

    __debug__("Attempting to read /etc/os-release\n");
    if((osrelease=fopen("/etc/os-release","r"))==NULL) {
        __error__("Error reading /etc/os-release: %s\n", strerror(errno));
        exit(1);
    }

    pretty = (char*)malloc(sizeof(char)*1024);
    memset(pretty, 0, 1024);
    fallback = (char*)malloc(sizeof(char)*1024);
    memset(fallback, 0, 1024);

    while((nread=getline(&line, &size, osrelease))!=-1) {
        if(strncmp(PRETTY, line, strlen(PRETTY))==0)
            copy_value(PRETTY, line, pretty);
        if(strncmp(SIMPLE, line, strlen(SIMPLE))==0)
            copy_value(SIMPLE, line, fallback);
    }
    free(line);

    if(pretty[0]==0)
        return fallback;
    else
        return pretty;
}

void copy_value(const char *var, char *line, char *dest)
{
    size_t i;
    int j=0;
    char *buffer;

    buffer=strstr(line,"=")+1;
    for(i=0; i<(strlen(line)-strlen(var)); i++) {
        if(buffer[i]=='"')
            continue;
        if(buffer[i]=='\n')
            continue;
        dest[j++]=buffer[i];
    }
    dest[j]=0;
}
