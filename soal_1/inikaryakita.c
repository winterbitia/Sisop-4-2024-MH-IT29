#define FUSE_USE_VERSION 31  
#include <fuse.h>            
#include <errno.h>           
#include <dirent.h>          
#include <stdio.h>           
#include <stdlib.h>          
#include <string.h>          
#include <sys/stat.h>        
#include <sys/types.h>       
#include <unistd.h>          

#define MAX_PATH_LENGTH 512  
// Mendefinisikan panjang maksimal path

// Fungsi untuk menambah watermark ke gambar
void apply_watermark(const char *src_path, const char *dst_path) {
    char command[1024];  
    // Untuk menambahkan watermark menggunakan alat convert dari ImageMagick
    snprintf(command, sizeof(command), "convert \"%s\" -gravity south -pointsize 36 -fill white -draw \"text 0,0 'inikaryakita.id'\" \"%s\"",
             src_path, dst_path);
    system(command); 
}

// Fungsi untuk mereverse
void reverse_file_content(const char *src_path, const char *dst_path) {
    FILE *file = fopen(src_path, "r");  
    // Membuka file sumber untuk membaca
    FILE *outputFile = fopen(dst_path, "w");  
    // Membuka file tujuan untuk write

    if (file == NULL || outputFile == NULL) {  
        // Memeriksa eror
        perror("fopen");
        return;
    }

    fseek(file, 0, SEEK_END);  
    // Memindahkan penunjuk file ke akhir file
    long fileSize = ftell(file);  
    // Mendapatkan ukuran file
    fseek(file, 0, SEEK_SET);  
    // Memindahkan penunjuk file kembali ke awal

    char *content = malloc(fileSize + 1);  
    // Mengalokasikan memori untuk menyimpan konten file
    fread(content, 1, fileSize, file);  
    // Membaca konten file ke dalam memori
    content[fileSize] = '\0';  

    for (long i = fileSize - 1; i >= 0; i--) {  
        // Menulis konten file dalam urutan terbalik
        fputc(content[i], outputFile);
    }

    free(content);  
    // Membebaskan memori yang dialokasikan
    fclose(file);  
    // Menutup file sumber
    fclose(outputFile);  
    // Menutup file tujuan
}

// Fungsi FUSE getattr untuk mendapatkan atribut file
static int do_getattr(const char *path, struct stat *st) {
    memset(st, 0, sizeof(struct stat));  
    // Mengosongkan struktur stat
    if (strcmp(path, "/") == 0 || strcmp(path, "/gallery") == 0 || strcmp(path, "/bahaya") == 0) {
        st->st_mode = S_IFDIR | 0755;  
        // Mengatur mode menjadi direktori dengan izin 755
        st->st_nlink = 2;  
        // Mengatur jumlah link menjadi 2
    } else {
        st->st_mode = S_IFREG | 0644;  
        // Mengatur mode menjadi file reguler dengan izin 644
        st->st_nlink = 1;  // Mengatur jumlah link menjadi 1
        st->st_size = 1024;  // Mengatur ukuran arbitrer (dapat disesuaikan sesuai kebutuhan)
    }
    return 0;  
}

// Fungsi FUSE readdir untuk membaca konten direktori
static int do_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    (void)offset;  
    (void)fi;      

    if (strcmp(path, "/") == 0) {  
        // Jika direktori root sedang dibaca
        filler(buf, ".", NULL, 0);  
        // Menambahkan direktori saat ini
        filler(buf, "..", NULL, 0);  
        // Menambahkan direktori induk
        filler(buf, "gallery", NULL, 0);  
        // Menambahkan direktori "gallery"
        filler(buf, "bahaya", NULL, 0);  
        // Menambahkan direktori "bahaya"
    } else if (strcmp(path, "/gallery") == 0 || strcmp(path, "/bahaya") == 0) {  // Jika direktori gallery atau bahaya sedang dibaca
        DIR *dp;
        struct dirent *de;
        if ((dp = opendir(path)) == NULL) {  // Membuka direktori
            return -errno;  // Mengembalikan kesalahan jika gagal
        }
        while ((de = readdir(dp)) != NULL) {  
            // Membaca entri direktori
            filler(buf, de->d_name, NULL, 0);  
            // Menambahkan setiap entri ke buffer
        }
        closedir(dp);  
    }
    return 0;  
}

// Fungsi FUSE open untuk membuka file
static int do_open(const char *path, struct fuse_file_info *fi) {
    return 0;  
}

// Fungsi FUSE read untuk membaca konten file
static int do_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void)fi;  

    char src_path[MAX_PATH_LENGTH];  
    char temp_path[MAX_PATH_LENGTH];  
    snprintf(src_path, sizeof(src_path), ".%s", path);  

    if (strstr(path, "/gallery/") == path) {  
        snprintf(temp_path, sizeof(temp_path), "/tmp/watermarked%s", path + strlen("/gallery"));  
        apply_watermark(src_path, temp_path);  
        // Menerapkan watermark ke file
        FILE *file = fopen(temp_path, "r");  
        // Membuka file yang diberi watermark
        if (file == NULL) {
            return -errno;  
        }
        fseek(file, offset, SEEK_SET);  
        size_t bytes_read = fread(buf, 1, size, file);  
        // Membaca konten file ke buffer
        fclose(file);  
        return bytes_read;  
        // Mengembalikan jumlah byte yang dibaca
    } else if (strstr(path, "/bahaya/") == path) {  
        // Jika membaca dari bahaya
        snprintf(temp_path, sizeof(temp_path), "/tmp/reversed%s", path + strlen("/bahaya"));  
        // Membuat path sementara untuk file yang dibalik
        reverse_file_content(src_path, temp_path);  
        // Membalikkan konten file
        FILE *file = fopen(temp_path, "r");  
        // Membuka file yang dibalik
        if (file == NULL) {
            return -errno;  
        }
        fseek(file, offset, SEEK_SET);  
        size_t bytes_read = fread(buf, 1, size, file);  
        // Membaca konten file ke buffer
        fclose(file);  
        return bytes_read;  
    }
    return -ENOENT;  
    
}

// Mendefinisikan struktur operasi dengan operasi FUSE
static struct fuse_operations operations = {
    .getattr = do_getattr,  
    .readdir = do_readdir,  
    .open = do_open,        
    .read = do_read,        
};

int main(int argc, char *argv[]) {
    // Memastikan direktori ada
    mkdir("gallery", 0755);  // Membuat direktori gallery dengan izin 755
    mkdir("bahaya", 0755);  // Membuat direktori bahaya dengan izin 755

    // Fungsi utama FUSE untuk memasang sistem berkas
    return fuse_main(argc, argv, &operations, NULL);  
    // Memulai sistem berkas FUSE
}
