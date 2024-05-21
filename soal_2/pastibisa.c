#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

static const char *pesan_dir = "/home/malvin/sisopmodul4/sensitif/pesan";
static const char __attribute__((unused)) *rahasia_berkas_dir = "/home/malvin/sisopmodul4/sensitif/rahasia-berkas";
static const char __attribute__((unused)) *password = "password123";

void decrypt(char *buf, size_t size, const char *filename);

static int custom_getattr(const char *path, struct stat *stbuf)
{
    int res;

    res = lstat(path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

static int custom_read(const char *path, char *buf, size_t size, off_t offset,
                       struct fuse_file_info *fi)
{
    int fd;
    int res;

    (void)fi;
    fd = open(path, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);

    if (strncmp(path, pesan_dir, strlen(pesan_dir)) == 0) {
        if (strncmp(path + strlen(pesan_dir) + 1, "notes-base64.txt", 16) == 0) {
            decrypt(buf, size, "base64");
        } else if (strncmp(path + strlen(pesan_dir) + 1, "enkripsi_rot13.txt", 18) == 0) {
            decrypt(buf, size, "rot13");
        } else if (strncmp(path + strlen(pesan_dir) + 1, "new-hex.txt", 11) == 0) {
            decrypt(buf, size, "hex");
        } else if (strncmp(path + strlen(pesan_dir) + 1, "rev-text.txt", 13) == 0) {
            decrypt(buf, size, "rev");
        }
    }

    return res;
}

void decrypt(char *buf, size_t size, const char *filename)
{
    if (strcmp(filename, "base64") == 0) {
        BIO *bio, *b64;
        char *decodeBuf = (char *)malloc(size);
        memset(decodeBuf, 0, size);
        
        bio = BIO_new_mem_buf(buf, -1);
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_push(b64, bio);
        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
        size_t decodeLen = BIO_read(bio, decodeBuf, size);
        memcpy(buf, decodeBuf, decodeLen);
        buf[decodeLen] = '\0';
        BIO_free_all(bio);
        free(decodeBuf);
    } else if (strcmp(filename, "rot13") == 0) {
        for (size_t i = 0; i < size; ++i) {
            if ('a' <= buf[i] && buf[i] <= 'z') {
                buf[i] = 'a' + (buf[i] - 'a' + 13) % 26;
            } else if ('A' <= buf[i] && buf[i] <= 'Z') {
                buf[i] = 'A' + (buf[i] - 'A' + 13) % 26;
            }
        }
    } else if (strcmp(filename, "hex") == 0) {
        size_t i;
        for (i = 0; i < size; i += 2) {
            char hex[3] = {buf[i], buf[i + 1], '\0'};
            buf[i / 2] = (char)strtol(hex, NULL, 16);
        }
        buf[i / 2] = '\0';
    } else if (strcmp(filename, "rev") == 0) {
        for (size_t i = 0; i < size / 2; ++i) {
            char tmp = buf[i];
            buf[i] = buf[size - i - 1];
            buf[size - i - 1] = tmp;
        }
    }
}

static struct fuse_operations fuse_example_oper = {
    .getattr = custom_getattr,
    .read = custom_read,
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &fuse_example_oper, NULL);
}

