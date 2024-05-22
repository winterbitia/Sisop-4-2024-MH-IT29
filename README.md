# Sisop-4-2024-MH-IT29
## Anggota Kelompok:
- Dian Anggraeni Putri (5027231016)
- Amoes Noland (5027231028)
- Malvin Putra Rismahardian (5027231048)

## Soal 1

> Dikerjakan oleh: Dian Anggraeni Putri (5027231016)

## Permasalahan:
Kami diberi tugas oleh Adfi, yang merupakan seorang CEO dari agensi kreatif "Ini Karya Kita", untuk melakukan inovasi dalam manajemen proyek fotografi. Tujuan utamanya adalah meningkatkan daya tarik proyek fotografi yang disajikan kepada klien. Kami memiliki portofolio proyek fotografi yang telah disiapkan dan dapat diunduh dari situs web agensi di www.inikaryakita.id. Setelah diunduh, terdapat dua folder utama yaitu "gallery" dan "bahaya".

Dalam folder "gallery", kami bertanggung jawab untuk menambahkan watermark pada setiap gambar dan memindahkannya ke dalam folder dengan prefix "wm". Sedangkan di folder "bahaya", tugas kami adalah mengubah permission pada file "script.sh" agar dapat dijalankan dan membuat program yang dapat membalik isi dari setiap file yang memiliki prefix "test".

## Penyelesaian:
Pertama, impor berbagai header yang diperlukan untuk operasi file dan direktori, serta mendefinisikan panjang maksimum untuk path file untuk memastikan alokasi memori yang aman dan mencegah buffer overflow.
```c
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
```

Kedua, mendeklarasikan empat fungsi yang akan digunakan dalam program.
```c
// fungsi untuk menambahkan watermark dan memindahkan gambar
void add_watermark_and_move(const char *image_path);

// fungsi untuk memproses file di folder "gallery"
void process_gallery();

// fungsi untuk membalik isi file dengan prefix "test"
void reverseTestFiles();

// fungsi untuk menjalankan script.sh
void run_script();
```

### Fungsi add_watermark_and_move
Fungsi ini berfungsi untuk menambahkan watermark pada setiap gambar dan memindahkannya ke dalam folder dengan prefix "wm". Pada setiap iterasi, fungsi ini menerima path file gambar sebagai argumen, kemudian menambahkan watermark pada gambar tersebut menggunakan perintah shell convert dari ImageMagick. Setelah ditambahkan watermark, gambar akan dipindahkan ke dalam folder "wm". 
```c
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
```

### Fungsi process_gallery
Fungsi ini berfungsi untuk memproses setiap file yang terdapat dalam folder "gallery". Di dalamnya, fungsi ini melakukan pengecekan untuk setiap entri dalam direktori "gallery" dan memastikan bahwa hanya file dengan ekstensi gambar seperti .jpeg, .jpg, atau .png yang diproses. Setelah itu, fungsi akan memanggil fungsi add_watermark_and_move untuk menambahkan watermark pada setiap gambar dan memindahkannya ke dalam folder dengan prefix "wm". 
```c
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
```

### Fungsi reverseTestFiles
Fungsi ini berfungsi untuk membalikkan isi dari setiap file yang memiliki prefix "test" di dalam folder "bahaya". Dalam implementasinya, fungsi ini membaca setiap entri dalam direktori "bahaya", kemudian melakukan pengecekan untuk setiap file apakah memiliki prefix "test". Jika iya, isi dari file tersebut akan dibalik. Hal ini bertujuan untuk memberikan fitur baru yang diinginkan oleh Adfi, yang mana setiap file dengan prefix "test" akan mengalami pembalikan isi dari file tersebut, sehingga isi file tersebut dapat terbaca secara terbalik.
```c
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
```

### Fungsi run_script
Fungsi ini berfungsi untuk menjalankan file "script.sh" yang terdapat dalam folder "bahaya". Langkah ini diperlukan karena "script.sh" merupakan bagian dari tugas yang diberikan oleh Adfi. Sebelumnya, telah dipastikan bahwa permission pada file "script.sh" sudah diubah agar dapat dijalankan melalui proses yang telah dilakukan sebelumnya dalam program. Dengan menjalankan "script.sh" melalui fungsi ini, program akan mengimplementasikan serangkaian aksi yang telah ditentukan dalam "script.sh", seperti penghapusan isi dari folder "gallery".
```c
// fungsi untuk menjalankan script.sh
void run_script() {
    // menjalankan script.sh
    system("./bahaya/script.sh");
}
```

### Fungsi main
Fungsi ini berfungsi sebagai titik masuk utama program. Di dalamnya, langkah-langkah eksekusi yang diperlukan untuk menyelesaikan tugas dari Adfi diimplementasikan. Pertama, fungsi process_gallery dipanggil untuk melakukan pemrosesan folder "gallery". Langkah ini mencakup penambahan watermark pada gambar-gambar yang terdapat di dalamnya. Selanjutnya, dengan memanggil chmod, kami memastikan bahwa permission pada file "script.sh" telah diubah agar dapat dijalankan. Langkah berikutnya adalah memanggil fungsi reverseTestFiles untuk melakukan pembalikan isi dari file-file yang memiliki prefix "test" di dalam folder "bahaya". Setelah itu, file "script.sh" dijalankan melalui pemanggilan fungsi run_script. Terakhir, kami menghapus folder "gallery" dan "bahaya" beserta isinya sesuai dengan aksi yang telah ditentukan dalam "script.sh".
```c
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
```

## Dokumentasi Hasil Pengerjaan
### terminal
![alt text](https://cdn.discordapp.com/attachments/1223171682500350062/1241840806961086614/IMG_20240520_024018.jpg?ex=664ba9e0&is=664a5860&hm=885e7f22111780afb0f275779a2150cf2fc6b9b9d09445fa86df7d79dab44003&)

## Sebelum 
### foto tidak ada watermark dan belum dipindah ke folder wm
![alt text](https://cdn.discordapp.com/attachments/1223171682500350062/1241840682570743959/IMG_20240520_024736.jpg?ex=664ba9c3&is=664a5843&hm=4645039d67927be2eea051a8442abdb10677653748405bfcf70494f4dfe05193&)

## Sesudah
### foto telah dipindah ke folder wm
![alt text](https://cdn.discordapp.com/attachments/1223171682500350062/1241840682247651490/IMG_20240520_024639.jpg?ex=664ba9c2&is=664a5842&hm=1e4159c8f2c1b7fa69ac5e7fce695b0a49cd415d14185f38b1540ac93dfdb5b9&)

### foto telah berhasil diberi watermark
![alt text](https://cdn.discordapp.com/attachments/1223171682500350062/1241840680922382386/IMG_20240520_024540.jpg?ex=664ba9c2&is=664a5842&hm=447ab5c047a85d5356b9a9c8ec631af8c6832f38a39958aec004e57b3e3b8c76&)

![alt text](https://cdn.discordapp.com/attachments/1223171682500350062/1241840681862037576/IMG_20240520_024917.jpg?ex=664ba9c2&is=664a5842&hm=debd9ef08b1e7cb62a84121f7779b0a03718632e1a118d40b075c82da63ebcad&)

### membuat folder reverse untuk menyimpan hasil pembalikan (reverse) isi dari file dengan prefix "test"
![alt text](https://cdn.discordapp.com/attachments/1223171682500350062/1241840681572368404/IMG_20240520_024837.jpg?ex=664ba9c2&is=664a5842&hm=035fac719a481686dc76c0786292de771869dd5668148437047c1e591ad691ad&)

### hasil pembalikan (reverse) isi dari file dengan prefix "test"
![alt text](https://cdn.discordapp.com/attachments/1223171682500350062/1241840680733507685/IMG_20240520_024518.jpg?ex=664ba9c2&is=664a5842&hm=9caf7fe6d4d5d810addb54ac38ef4f267e648ada2734c749f4ec26f803b7d843&)

### berhasil mengubah permission pada file "script.sh" (jika dijalankan maka dapat menghapus semua dan isi dari "gallery"), isi dari "gallery" telah berhasil terhapus
![alt text](https://cdn.discordapp.com/attachments/1223171682500350062/1241840681295806584/IMG_20240520_024751.jpg?ex=664ba9c2&is=664a5842&hm=8ec2697fe02bb285b656ec8272979f5268728e10ecf441db64811db69e632d7c&)


## Soal 2

> Dikerjakan oleh: Malvin Putra Rismahardian (5027231048)

## Soal 3

> Dikerjakan oleh: Amoes Noland (5027231028)

Di dalam soal 3 ini, kita disuruh untuk membuat sebuah FUSE untuk menggabungkan pecahan relic berharga tanpa menyentuh isi folder relic secara langsung agar tergabung dengan mudah.

### Inisialisasi FUSE

Untuk membangun sebuah FUSE yang cocok bagi kasus ini, dimulai dari menentukan beberapa library dan variabel penting:

```c
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
#include <time.h>

// Definite global variables
#define MAX_BUFFER 1028  // For universal buffer
#define MAX_SPLIT  10000 // For 10K splits
static const char *dirpath =
"/home/winter/Documents/ITS/SISOP/Modul4/soal_3/relics"; // Full path
```

Dari library fuse yang sudah ter-install dan terdefinisi, dapat dipanggil melalui main dengan menentukan daftar fungsi dan menambahkan permission lengkap di dalam main:

```c
static struct fuse_operations arc_oper = {
    .getattr    = arc_getattr,
    .readdir    = arc_readdir,
    .read       = arc_read,
    .write      = arc_write,
    .unlink     = arc_unlink,
    .create     = arc_create,
    .utimens    = arc_utimens,
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &arc_oper, NULL);
}
```

### Listing

Ketika dilakukan listing di melalui FUSE, yang muncul adalah isi folder relics yang sudah tergabung seluruh pecahannya menjadi satu bagian utuh.

Untuk memperoleh atribut dari setiap file yang akan digabung, diperlukan fungsi `getattr`, yang diimplementasikan sebagai berikut:

```c
// Function to get file attributes
static int arc_getattr(const char *path, struct stat *stbuf)
{
    // kode
}
```

Informasi pertama yang saya masukkan adalah data pemilik dan waktu akses/modifikasi (sesuai library `time.h`):

```c
    // Set time and ownership attributes
    stbuf->st_uid   = getuid();
    stbuf->st_gid   = getgid();
    stbuf->st_atime = time(NULL);
    stbuf->st_mtime = time(NULL);
```

Setelah itu, informasi berikutnya adalah mode permission dan menentukan jumlah link (2 untuk direktori karena direktori terhubung dengan `.` dan direktori itu tersendiri), hal ini dilakukan berdasarkan full path apabila `/` atau sebuah item biasa:

```c
    if (strcmp(path, "/") == 0) {
        // Set link to . and itself if dir with dir perms
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    // Get full path of item
    char fpath[MAX_BUFFER];
    sprintf(fpath, "%s%s",dirpath,path);

    // Set link to only itself with read only perms
    stbuf->st_mode = S_IFREG | 0644;
    stbuf->st_nlink = 1;
    stbuf->st_size = 0;
```

Setelah diketahui bahwa sumber atribut yang dicari bukan merupakan direktori, maka dilakukan looping sejumlah seluruh part yang ada dari sebuah item dari relics, sehingga dapat dijumlahkan file size untuk dikirim ke stat buffer:

```c
    // Buffers for part looping
    char ppath[MAX_BUFFER+4];
    FILE *fd; int i = 0;

    // Gets total size of all parts in a loop
    while(1){
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
```

Setelah memiliki fungsi untuk mendapatkan atribut sebuah item, maka dapat mulai dikirim stat ke dalam fungsi membaca direktori:

```c
static int arc_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    // kode
}
```

Dalam setiap command `ls -a` diperlukan informasi lengkap, sehingga kita juga tambahkan tampilan untuk direktori saat ini, dan direktori parent:

```c
    // Fills directory listing
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
```

Setelah itu mulai persiapan membaca direktori relics dengan mendapatkan path lengkap dan mempersiapkan sistem pembacaan direktori:

```c
    // Handle path
    char fpath[MAX_BUFFER];
    sprintf(fpath, "%s%s",dirpath,path);

    // Handles directory opening
    DIR *dp;
    struct dirent *de;
    dp = opendir(fpath);
    if (!dp) return -errno;
```

Loop utama dimulai dengan menyiapkan stat yang diambil dari getattr dengan menghubungkannya dengan node directory entry:

```c
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
```

Loop dilakukan melewati seluruh file di dalam direktori, tetapi agar hanya dilakukan listing satu kali pada setiap item saat `ls` dipanggil, dilakukan pengecekan agar hanya terpanggil pada part `.000` yang dimiliki semua file pada folder relics. Lalu dilakukan pemotongan nama file sehingga hanya ditunjukkan nama file tanpa angka part:

```c
        if (strstr(de->d_name, ".000") == NULL) continue;

        char cut[MAX_BUFFER];
        strcpy(cut, de->d_name);
        cut[strlen(cut)-4] = '\0';
```

Untuk mengakhiri loop maka dipanggil fungsi filler, dan akan break dari loop bila filler menunjukkan return value 1 untuk jaga-jaga. Selain itu seluruh fungsi diakhiri oleh menutup direktori yang sudah dibuka sebelumnya.

```c
        if (filler(buf, cut, &st, 0)) break;
    }
    closedir(dp); return 0;
```

### Copy dari FUSE ke luar

Dalam melakukan copy dari FUSE ke luar, file yang dicopy adalah hasil gabungan seluruh relics menjadi satu bagian yang utuh. Untuk ini diperlukan fungsi untuk membaca isi file secara utuh:

```c
static int arc_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    // Kode
}
```

Langkah pertama yang dilakukan adalah menyiapkan beberapa variabel untuk:
* Path lengkap file dari FUSE
* Path untuk part dalam FUSE
* Jumlah part yang dilewati
* Size file yang dibaca
* Total size file yang sudah dibaca

```c
    // Get full path of item
    char fpath[MAX_BUFFER];
    sprintf(fpath, "%s%s",dirpath,path);

    // Buffers for part looping
    char ppath[MAX_BUFFER+4];
    FILE *fd; int i = 0;

    // Prepare read variables
    size_t size_read;
    size_t total_read = 0;
```

Loop dilakukan melewati semua part dalam sebuah nama part utuh, dan berakhir bila size yang dibawa oleh parameter FUSE sudah terbaca semua:

```c
    while (size > 0){
        // Loops through all part numbers
        sprintf(ppath, "%s.%03d", fpath, i++);
        fd = fopen(ppath, "rb");
        if (!fd) break;
```

Mendapatkan file size dari part dilakukan dengan menggeser pointer dari part dan meluruskan offset bila melebihi ukuran file size dari part:

```c
        // Get part size
        fseek(fd, 0L, SEEK_END);
        size_t size_part = ftell(fd);
        fseek(fd, 0L, SEEK_SET);

        // Adjust offset
        if (offset >= size_part) {
            offset -= size_part;
            fclose(fd); continue;
        }
```

Setelah itu dilakukan pembacaan sesuai offset dan atur angka-angka yang berhubungan untuk mengakhiri loop:

```c
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
```

Di akhir fungsi dilakukan return total filesize yang dibaca untuk memenuhi kriteria dari FUSE:

```c
    return total_read;
```

### Copy/Membuat file ke dalam FUSE

Dalam menambahkan file baru ke dalam FUSE, harus dibentuk pecahan dalam folder relics sesuai dengan ukuran data yang tertulis. Ada dua fungsi yang terlibat dalam keperluan ini:

```c
// Function to write data into file
static int arc_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    // Kode
}

// Function to create item
static int arc_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    // Kode
}
```

Di dalam fungsi create, dilakukan dengan membentuk part `.000` dalam folder relics:

```c
    // Get full path of item
    char fpath[MAX_BUFFER];
    sprintf(fpath, "%s%s.000",dirpath,path);

    // Create item
    int res = creat(fpath, mode);
    if (res == -1) return -errno;

    close(res); return 0;
```

Di dalam fungsi write, dimulai dengan mendapatkan segala variabel yang terlibat:
* Path lengkap file yang akan dibuat FUSE
* Path untuk part dalam FUSE
* Jumlah part yang sedang ditulis (offset dibagi 10K)
* Offset part yang sedang ditulis (offset modulo 10K)
* Total yang sudah ditulis

```c
    // Get full path of item
    char fpath[MAX_BUFFER];
    sprintf(fpath, "%s%s",dirpath,path);

    // Buffers for part looping
    char ppath[MAX_BUFFER+4];
    FILE *fd;

    // Prepare write variable
    int    pcurrent = offset / MAX_SPLIT;
    size_t poffset  = offset % MAX_SPLIT;
    size_t total_write = 0;
```

Loop dilakukan melewati semua part dalam sebuah nama part utuh, dan berakhir bila size yang dibawa oleh parameter FUSE sudah terbaca semua. Dari setiap file yang terbaca, akan dicoba untuk dibuka dengan permission read+, bila file tidak ada, maka akan dicoba dengan permission hanya write:

```c
    // Main write loop
    while (size > 0){
        // Loops through all part numbers
        sprintf(ppath, "%s.%03d", fpath, pcurrent++);
        fd = fopen(ppath, "r+b");
        if (!fd) {
            fd = fopen(ppath, "wb");
            if (!fd) return -errno;
        }
```

Langkah berikutnya adalah menentukan ukuran yang ditulis dengan membandingkan dengan parameter size yang disampaikan melalui FUSE agar berjumlah sesuai size, atau mengurangi 10K dengan offset part yang sedang dilalui:

```c
        // Determine write size buffer
        fseek(fd, poffset, SEEK_SET);
        size_t size_write;
        if (size > (MAX_SPLIT - poffset))
             size_write = MAX_SPLIT - poffset;
        else size_write = size;
```

Setelah itu dilakukan pembacaan sesuai offset dan atur angka-angka yang berhubungan untuk mengakhiri loop:

```c
        // Write into one file
        fwrite(buf, 1, size_write, fd);
        fclose(fd);

        // Modify loop numbers
        buf += size_write;
        size -= size_write;
        total_write += size_write;

        // Reset offset
        poffset = 0;
    }
```

Di akhir fungsi dilakukan return total filesize yang dibaca untuk memenuhi kriteria dari FUSE:

```c
    return total_write;
```
### Delete melalui FUSE

Untuk melakukan penghapusan melalui FUSE, harus dilakukan delete seluruh part yang terlibat dari satu file utuh yang di dalam FUSE. Hal ini dilakukan dengan fungsi:

```c
// Function to unlink item
static int arc_unlink(const char *path)
{
    // Get full path of item and prep part buffer
    char fpath[MAX_BUFFER];
    char ppath[MAX_BUFFER+4];
    sprintf(fpath, "%s%s",dirpath,path);

    int pcurrent = 0;    
    while(1){
        // DEBUGGING
        printf("unlink: %s\n", ppath);

        // Loop through all parts
        sprintf(ppath, "%s.%03d", fpath, pcurrent++);
        int res = unlink(ppath);
        if (res == -1){
            if (errno == ENOENT) break;
            return -errno;
        }
    }
    return 0;
}
```

(work in progress)

