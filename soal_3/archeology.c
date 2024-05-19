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
    VERSION 4 - proper combined listing and reading
    Amoes Noland 5027231028
*/

// Definite global variables
#define MAX_BUFFER 1028
static const char *dirpath =
"/home/winter/Documents/ITS/SISOP/Modul4/soal_3/relics";

// Get file attributes
static int arc_getattr(const char *path, struct stat *stbuf)
{
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        // Set link to . and itself if dir with dir perms
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;

        // DEBUGGING
        // printf("attr dir\n");
    }

    // Get full path of item
    char fpath[MAX_BUFFER];
    sprintf(fpath, "%s%s",dirpath,path);

    // DEBUGGING
    // printf("atr: %s\n", fpath);

    // Set link to only itself with read only perms
    stbuf->st_mode = S_IFREG | 0444;
    stbuf->st_nlink = 1;
    stbuf->st_size = 0;

    // Buffers for part looping
    char ppath[MAX_BUFFER+4];
    FILE *fd; int i = 0;

    // Gets total size of all parts in a loop
    while(1){
        //DEBUGGING
        // printf("%s.%03d\n", fpath, i);

        // Loops through all part numbers
        sprintf(ppath, "%s.%03d", fpath, i++);
        fd = fopen(ppath, "rb");
        if (!fd) break;

        // Adds filesize to stat buffer
        fseek(fd, 0L, SEEK_END);
        stbuf->st_size += ftell(fd);
        fclose(fd);
    }

    if (i==1) return -errno;
    return 0;
}

// Read directory
static int arc_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

    // Fills directory listing
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    // Handle path
    char fpath[MAX_BUFFER];
    sprintf(fpath, "%s%s",dirpath,path);

    // Handles directory opening
    DIR *dp;
    struct dirent *de;
    dp = opendir(fpath);
    if (!dp) return -errno;

    // DEBUGGING
    // int filecount = 1;
    printf("dir: %s\n", fpath);

    // Main loop for directory checking
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        if (strstr(de->d_name, ".000") == NULL) continue;

        char cut[MAX_BUFFER];
        strcpy(cut, de->d_name);
        cut[strlen(cut)-4] = '\0';

        // DEBUGGING
        // printf("ls%02d %s\n", filecount++, cut);

        if (filler(buf, cut, &st, 0)) break;
    }

    closedir(dp); return 0;
}

// Read data from file
static int arc_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    (void) fi;

    // Get full path of item
    char fpath[MAX_BUFFER];
    sprintf(fpath, "%s%s",dirpath,path);

    // Buffers for part looping
    char ppath[MAX_BUFFER+4];
    FILE *fd; int i = 0;

    // Prepare read variable
    size_t size_read;
    size_t total_read = 0;

    while (size > 0){
        // Loops through all part numbers
        sprintf(ppath, "%s.%03d", fpath, i++);
        fd = fopen(ppath, "rb");
        if (!fd) break;

        // Get part size
        fseek(fd, 0L, SEEK_END);
        size_t size_part = ftell(fd);
        fseek(fd, 0L, SEEK_SET);

        // Adjust offset
        if (offset >= size_part) {
            offset -= size_part;
            fclose(fd); continue;
        }

        // Read buffer
        fseek(fd, offset, SEEK_SET);
        size_read = fread(buf, 1, size, fd);
        fclose(fd);

        // Modify loop numbers
        buf += size_read;
        size -= size_read;
        total_read += size_read;

        // Reset offset
        offset = 0;
    }
    return total_read;
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