#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <sys/stat.h>

/*  
    Soal_3 archeology.c
    VERSION 3 - no longer makes unused combination files
    Amoes Noland 5027231028
*/

// Definite global variables
#define MAX_BUFFER 1028
static const char *dirpath =
"/home/winter/Documents/ITS/SISOP/Modul4/soal_3/relics";

static int arc_getattr(const char *path, struct stat *stbuf)
{
    // Get file attributes
    char fpath[MAX_BUFFER];
    if(strcmp(path,"/") == 0){
        path = dirpath;
        sprintf(fpath,"%s",path);
    } else {
        sprintf(fpath, "%s%s",dirpath,path);
    }

    int res = lstat(fpath, stbuf);
    if (res == -1) return -errno;

    return 0;
}

static int arc_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
    // Read directory
    char fpath[MAX_BUFFER];
    if(strcmp(path,"/") == 0){
        path = dirpath;
        sprintf(fpath,"%s",path);
    } else {
        sprintf(fpath, "%s%s",dirpath,path);
    }

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;
    
    dp = opendir(fpath);
    if (!dp) return -errno;

    int filecount = 0;
    printf("dir: %s\n", fpath);

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        if (de->d_name[0] == '.') continue;

        char cut[MAX_BUFFER];
        strcpy(cut, de->d_name);
        cut[strlen(cut)-4] = '\0';

        printf("ls%d %s\n", filecount++, cut);

        // char cmd[MAX_BUFFER];
        // sprintf(cmd, "cat %s* >> %s", cut, cut); 
        // system(cmd);

        if (filler(buf, cut, &st, 0)) break;
    }

    closedir(dp); return 0;
}

static int arc_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    // Read data from file
    char fpath[MAX_BUFFER];
    if(strcmp(path,"/") == 0){
        path = dirpath;
        sprintf(fpath,"%s",path);
    } else {
        sprintf(fpath, "%s%s",dirpath,path);
    } 

    (void) fi;

    int fd = open(fpath, O_RDONLY);
    if (fd == -1) return -errno;

    printf("from read: %s\n", fpath);

    int res = pread(fd, buf, size, offset);
    if (res == -1) res = -errno;

    close(fd); return res;
}

static struct fuse_operations arc_oper = {
    .getattr    = arc_getattr,
    .readdir    = arc_readdir,
    .read       = arc_read,
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &arc_oper, NULL);
}