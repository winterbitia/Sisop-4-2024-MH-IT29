#include <dirent.h> // directory-related functions
#include <stdio.h> // standard input/output
#include <stdlib.h> // general utility functions
#include <string.h> // string manipulation functions
#include <sys/stat.h> // file status and mode related functions
#include <sys/types.h> // system data types
#include <unistd.h> // posix api functions

#define MAX_PATH_LENGTH 512  
// mendefinisikan panjang maksimum untuk path
// to ensure safe memory allocation and prevent buffer overflows

// fungsi untuk menambahkan watermark dan memindahkan gambar
void add_watermark_and_move(const char *image_path);

// fungsi untuk memproses file di folder "gallery"
void process_gallery();

// fungsi untuk membalik isi file dengan prefix "test"
void reverseTestFiles();

// fungsi untuk menjalankan script.sh
void run_script();

// fungsi untuk menambahkan watermark pada gambar dan memindahkannya ke folder "wm"
void add_watermark_and_move(const char *image_path) {
    char command[1024]; // buffer perintah shell
    char destination_path[MAX_PATH_LENGTH]; // buffer path tujuan

    // membuat folder "wm" jika belum ada
    mkdir("gallery/wm", 0777);

    // membuat path tujuan untuk menyimpan file gambar di dalam folder "gallery/wm"
    snprintf(destination_path, sizeof(destination_path), "gallery/wm/%s", strrchr(image_path, '/') + 1);

    // instruksi untuk menambahkan watermark dan memindahkan gambar
    snprintf(command, sizeof(command), "convert \"%s\" -gravity south -pointsize 36 -fill white -draw \"text 0,0 'inikaryakita.id'\" \"%s\" && mv \"%s\" \"%s\"",
        image_path, image_path, image_path, destination_path);
             
    // eksekusi instruksi watermark dan pemindahan gambar
    system(command);
}

// fungsi untuk memproses file di dalam folder "gallery"
void process_gallery() {
    DIR *dir;  // pointer ke direktori
    struct dirent *ent;  // struktur untuk menyimpan informasi entri direktori

    // membuka direktori "gallery"
    if ((dir = opendir("gallery")) != NULL) {
        
        // membaca setiap entri dalam direktori
        while ((ent = readdir(dir)) != NULL) {
            
            // memeriksa ekstensi file untuk memastikan itu adalah gambar
            if (strstr(ent->d_name, ".jpeg") || strstr(ent->d_name, ".jpg") || strstr(ent->d_name, ".png") ||
                strstr(ent->d_name, ".JPEG") || strstr(ent->d_name, ".JPG") || strstr(ent->d_name, ".PNG")) {
                    
                char src_path[MAX_PATH_LENGTH];  // buffer untuk menyimpan path sumber
                
                // menggabungkan nama file dari entri direktori ke dalam path "gallery" dan menyimpan hasilnya di `src_path`.
                snprintf(src_path, sizeof(src_path), "gallery/%s", ent->d_name);

                // menambahkan watermark dan memindahkan file ke folder "wm"
                add_watermark_and_move(src_path);
                printf("file %s moved to the 'wm' folder with watermark.\n", ent->d_name);
            }
        }
        
        // menutup direktori setelah selesai
        closedir(dir);
    } else {
        // menampilkan pesan kesalahan jika direktori tidak dapat dibuka
        perror("could not open gallery directory");
    }
}

// fungsi untuk membalik isi file dengan prefix "test" dan menyimpannya dalam file baru
void reverseTestFiles() {
    struct dirent *entry;  // struktur untuk menyimpan informasi entri direktori
    DIR *dp = opendir("bahaya");  // membuka direktori "bahaya"

    // memeriksa apakah direktori berhasil dibuka
    if (dp == NULL) {
        perror("opendir: bahaya");  // menampilkan pesan kesalahan
        return;
    }

    // membaca setiap entri dalam direktori
    while ((entry = readdir(dp))) {
        
        // memeriksa apakah entri adalah file reguler dan memiliki prefix "test"
        if (entry->d_type == DT_REG && strncmp(entry->d_name, "test", 4) == 0) {
            
            char filePath[MAX_PATH_LENGTH];  // buffer untuk path file sumber
            
            // buat path file dengan nama dari entri direktori di folder "bahaya".
            snprintf(filePath, sizeof(filePath), "bahaya/%s", entry->d_name);
            
            char outputPath[MAX_PATH_LENGTH];  // buffer untuk path file output
            
            // buat path file dengan nama "reversed_" ditambah nama asli file di dalam folder "bahaya"
            snprintf(outputPath, sizeof(outputPath), "bahaya/reversed_%s", entry->d_name);

            // membuka file sumber "filePath" dalam mode "r" (read) untuk membaca isinya
            FILE *file = fopen(filePath, "r");
            
            // membuka file output "outputPath" dalam mode "w" (write) untuk menulis data baru ke dalamnya
            FILE *outputFile = fopen(outputPath, "w");  

            // memeriksa apakah file berhasil dibuka
            if (file == NULL || outputFile == NULL) {
                perror("fopen");  // menampilkan pesan kesalahan
                continue;
            }

            // mendapatkan ukuran file 
            fseek(file, 0, SEEK_END); // memposisikan pointer file ke akhir file
            long fileSize = ftell(file); // mendapatkan posisi pointer file saat ini, yang merupakan ukuran file
            fseek(file, 0, SEEK_SET); // memposisikan pointer file kembali ke awal file

            // membaca isi file
            char *content = malloc(fileSize + 1);  
            // alokasi memori untuk isi file
            fread(content, 1, fileSize, file);
            // add null terminator (if buffer allows), handle potential overflow
            content[fileSize] = '\0';

            // membalik isi file dan menulisnya ke file output
            for (long i = fileSize - 1; i >= 0; i--) {
                fputc(content[i], outputFile);
            }

            // membebaskan memori dan menutup file
            free(content);
            fclose(file);
            fclose(outputFile);
        }
    }

    // menutup direktori setelah selesai
    closedir(dp);
}

// fungsi untuk menjalankan script.sh
void run_script() {
    // menjalankan script.sh
    system("./bahaya/script.sh");
}

int main() {
    // memproses folder "gallery" untuk menambahkan watermark pada gambar
    process_gallery();

    // mengubah permission script.sh jika berpotensi menghapus "gallery"
    if (chmod("bahaya/script.sh", 0755) < 0) {
        perror("chmod: script.sh");  // menampilkan pesan kesalahan
    }

    // memproses file dalam folder "bahaya" untuk membalik isi file dengan prefix "test"
    reverseTestFiles();

    // menjalankan script.sh
    run_script();

    // menghapus folder "gallery" dan "bahaya" beserta isinya sesuai script.sh
    system("rm -rf gallery bahaya");

    return 0;  
}

