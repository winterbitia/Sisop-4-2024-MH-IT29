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
#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#define PATH_MAX_LENGTH 512
```

Kedua, mendeklarasikan empat fungsi yang akan digunakan dalam program.
```c
void add_watermark_and_move(const char *image_path);
void handle_image_folder();
void reverse_test_files();
void execute_script();
```

### Fungsi add_watermark_and_move
Fungsi ini berfungsi untuk menambahkan watermark pada setiap gambar dan memindahkannya ke dalam folder dengan prefix "wm". Pada setiap iterasi, fungsi ini menerima path file gambar sebagai argumen, kemudian menambahkan watermark pada gambar tersebut menggunakan perintah shell convert dari ImageMagick. Setelah ditambahkan watermark, gambar akan dipindahkan ke dalam folder "wm". 
```c
void add_watermark_and_move(const char *image_path) {
    char cmd[1024];
    char dest_path[PATH_MAX_LENGTH];

    mkdir("gallery/wm", 0777);

    snprintf(dest_path, sizeof(dest_path), "gallery/wm/%s", strrchr(image_path, '/') + 1);

    snprintf(cmd, sizeof(cmd), "convert \"%s\" -gravity south -pointsize 40 -fill white -draw \"text 0,0 'inikaryakita.id'\" \"%s\" && mv \"%s\" \"%s\"",
        image_path, image_path, image_path, dest_path);

    system(cmd);
}
```

### Fungsi handle_image_folder
Fungsi ini berfungsi untuk memproses setiap file yang terdapat dalam folder "gallery". Di dalamnya, fungsi ini melakukan pengecekan untuk setiap entri dalam direktori "gallery" dan memastikan bahwa hanya file dengan ekstensi gambar seperti .jpeg, .jpg, atau .png yang diproses. Setelah itu, fungsi akan memanggil fungsi add_watermark_and_move untuk menambahkan watermark pada setiap gambar dan memindahkannya ke dalam folder dengan prefix "wm". 
```c
void handle_image_folder() {
    DIR *directory;
    struct dirent *entry;

    if ((directory = opendir("gallery")) != NULL) {
        while ((entry = readdir(directory)) != NULL) {
            if (strstr(entry->d_name, ".jpeg") || strstr(entry->d_name, ".jpg") || strstr(entry->d_name, ".png") ||
                strstr(entry->d_name, ".JPEG") || strstr(entry->d_name, ".JPG") || strstr(entry->d_name, ".PNG")) {
                    
                char source_path[PATH_MAX_LENGTH];
                
                snprintf(source_path, sizeof(source_path), "gallery/%s", entry->d_name);

                add_watermark_and_move(source_path);
                printf("File %s moved to the 'wm' folder with watermark successfully.\n", entry->d_name);
            }
        }
        closedir(directory);
    } else {
        perror("Could not open gallery directory.");
    }
}
```

### Fungsi reverse_test_files
Fungsi ini berfungsi untuk membalikkan isi dari setiap file yang memiliki prefix "test" di dalam folder "bahaya". Dalam implementasinya, fungsi ini membaca setiap entri dalam direktori "bahaya", kemudian melakukan pengecekan untuk setiap file apakah memiliki prefix "test". Jika iya, isi dari file tersebut akan dibalik. Hal ini bertujuan untuk memberikan fitur baru yang diinginkan oleh Adfi, yang mana setiap file dengan prefix "test" akan mengalami pembalikan isi dari file tersebut, sehingga isi file tersebut dapat terbaca secara terbalik.
```c
void reverse_test_files() {
    struct dirent *entry;
    DIR *dir_ptr = opendir("bahaya");

    if (dir_ptr == NULL) {
        perror("opendir: bahaya");
        return;
    }

    while ((entry = readdir(dir_ptr))) {
        if (entry->d_type == DT_REG && strncmp(entry->d_name, "test", 4) == 0) {
            char src_path[PATH_MAX_LENGTH];
            snprintf(src_path, sizeof(src_path), "bahaya/%s", entry->d_name);
            
            char out_path[PATH_MAX_LENGTH];
            snprintf(out_path, sizeof(out_path), "bahaya/reversed_%s", entry->d_name);

            FILE *src_file = fopen(src_path, "r");
            FILE *out_file = fopen(out_path, "w");

            if (src_file == NULL || out_file == NULL) {
                perror("fopen");
                continue;
            }

            fseek(src_file, 0, SEEK_END);
            long file_size = ftell(src_file);
            fseek(src_file, 0, SEEK_SET);

            char *content = malloc(file_size + 1);
            fread(content, 1, file_size, src_file);
            content[file_size] = '\0';

            for (long i = file_size - 1; i >= 0; i--) {
                fputc(content[i], out_file);
            }

            free(content);
            fclose(src_file);
            fclose(out_file);
        }
    }

    closedir(dir_ptr);
}
```

### Fungsi execute_script
Fungsi ini berfungsi untuk menjalankan file "script.sh" yang terdapat dalam folder "bahaya". Langkah ini diperlukan karena "script.sh" merupakan bagian dari tugas yang diberikan oleh Adfi. Sebelumnya, telah dipastikan bahwa permission pada file "script.sh" sudah diubah agar dapat dijalankan melalui proses yang telah dilakukan sebelumnya dalam program. Dengan menjalankan "script.sh" melalui fungsi ini, program akan mengimplementasikan serangkaian aksi yang telah ditentukan dalam "script.sh", seperti penghapusan isi dari folder "gallery".
```c
void execute_script() {
    system("./bahaya/script.sh");
}
```

### Fungsi fuse
Program mendefinisikan fungsi-fungsi yang diperlukan oleh FUSE untuk melakukan operasi filesystem di userspace:
myfs_getattr: Mengembalikan informasi atribut dari file.
myfs_readdir: Membaca isi dari suatu direktori.
myfs_open: Membuka suatu file.
myfs_read: Membaca isi dari suatu file.
Dan struktur fuse_operations untuk menyediakan kaitan antara fungsi-fungsi tersebut dengan implementasi operasi FUSE.
```c
static int myfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;
    char full_path[PATH_MAX_LENGTH];
    snprintf(full_path, sizeof(full_path), "root%s", path);

    dp = opendir(full_path);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    closedir(dp);
    return 0;
}

static int myfs_open(const char *path, struct fuse_file_info *fi) {
    int res;
    char full_path[PATH_MAX_LENGTH];
    snprintf(full_path, sizeof(full_path), "root%s", path);

    res = open(full_path, fi->flags);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}

static int myfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int fd;
    int res;
    char full_path[PATH_MAX_LENGTH];
    snprintf(full_path, sizeof(full_path), "root%s", path);

    fd = open(full_path, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static struct fuse_operations myfs_oper = {
    .getattr = myfs_getattr,
    .readdir = myfs_readdir,
    .open    = myfs_open,
    .read    = myfs_read,
};
```

### Fungsi main
Fungsi ini berfungsi sebagai titik masuk utama program. Di dalamnya, langkah-langkah eksekusi yang diperlukan untuk menyelesaikan tugas dari Adfi diimplementasikan. Pertama, fungsi handle_image_folder dipanggil untuk melakukan pemrosesan folder "gallery". Langkah ini mencakup penambahan watermark pada gambar-gambar yang terdapat di dalamnya. Selanjutnya, dengan memanggil chmod, kami memastikan bahwa permission pada file "script.sh" telah diubah agar dapat dijalankan. Langkah berikutnya adalah memanggil fungsi reverse_test_files untuk melakukan pembalikan isi dari file-file yang memiliki prefix "test" di dalam folder "bahaya". Setelah itu, file "script.sh" dijalankan melalui pemanggilan fungsi execute_script. Terakhir, program menghapus folder "gallery" dan "bahaya" dengan menggunakan perintah system("rm -rf gallery bahaya"). Program kemudian menggunakan fuse_main untuk menjalankan filesystem FUSE dengan operasi yang telah didefinisikan.
```c
int main(int argc, char *argv[]) {

    handle_image_folder();

    if (chmod("bahaya/script.sh", 0755) < 0) {
        perror("chmod: script.sh");
    }

    reverse_test_files();

    execute_script();

    system("rm -rf gallery bahaya");

    return fuse_main(argc, argv, &myfs_oper, NULL);
}
```

## Dokumentasi Hasil Pengerjaan
### terminal
![alt text](https://cdn.discordapp.com/attachments/1223171682500350062/1243261893766811750/Screenshot_1842.png?ex=6650d55e&is=664f83de&hm=20413276fffd694d11d896375e155b9c4386e696d61c0143f494e7825ff2dc25&)

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
>
> ### isi soal

Masih dengan Ini Karya Kita, sang CEO ingin melakukan tes keamanan pada folder sensitif Ini Karya Kita. Karena Teknologi Informasi merupakan departemen dengan salah satu fokus di Cyber Security, maka dia kembali meminta bantuan mahasiswa Teknologi Informasi angkatan 2023 untuk menguji dan mengatur keamanan pada folder sensitif tersebut. Untuk mendapatkan folder sensitif itu, mahasiswa IT 23 harus kembali mengunjungi website Ini Karya Kita pada www.inikaryakita.id/schedule . Silahkan isi semua formnya, tapi pada form subject isi dengan nama kelompok_SISOP24 , ex: IT01_SISOP24 . Lalu untuk form Masukkan Pesanmu, ketik “Mau Foldernya” . Tunggu hingga 1x24 jam, maka folder sensitif tersebut akan dikirimkan melalui email kalian. Apabila folder tidak dikirimkan ke email kalian, maka hubungi sang CEO untuk meminta bantuan.   

* Pada folder "pesan" Adfi ingin meningkatkan kemampuan sistemnya dalam mengelola berkas-berkas teks dengan menggunakan fuse.
-Jika sebuah file memiliki prefix "base64," maka sistem akan langsung mendekode isi file tersebut dengan algoritma Base64.

-Jika sebuah file memiliki prefix "rot13," maka isi file tersebut akan langsung di-decode dengan algoritma ROT13.

-Jika sebuah file memiliki prefix "hex," maka isi file tersebut akan langsung di-decode dari representasi heksadesimalnya.

-Jika sebuah file memiliki prefix "rev," maka isi file tersebut akan langsung di-decode dengan cara membalikkan teksnya.
Contoh:
File yang belum didecode/ dienkripsi rot_13


File yang sudah didecode/ dienkripsi rot_13


* Pada folder “rahasia-berkas”, Adfi dan timnya memutuskan untuk menerapkan kebijakan khusus. Mereka ingin memastikan bahwa folder dengan prefix "rahasia" tidak dapat diakses tanpa izin khusus. 

-Jika seseorang ingin mengakses folder dan file pada “rahasia”, mereka harus memasukkan sebuah password terlebih dahulu (password bebas). 

- Setiap proses yang dilakukan akan tercatat pada logs-fuse.log dengan format :
[SUCCESS/FAILED]::dd/mm/yyyy-hh:mm:ss::[tag]::[information]
Ex:
[SUCCESS]::01/11/2023-10:43:43::[moveFile]::[File moved successfully]


### Penyelesaian
**Pertama**

Saya membuat direktori dan mengisi direktori itu dengan format sesuai dengan perintah yang diberikan. Kurang lebih menampilkan tree seperti ini : 

```sh
├── pastibisa
├── pastibisa.c
└── sensitif
    ├── pesan
    │   ├── enkripsi_rot13.txt
    │   ├── halo.txt
    │   ├── new-hex.txt
    │   ├── notes-base64.txt
    │   └── rev-text.txt
    └── rahasia-berkas
        ├── final.sh
        └── tulisan.tx
```

**Kedua**

Selanjutnya saya mengisi`pastibisa.c`dengan code berikut:

```sh
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
```

Secara garis besar, kode tersebut digunakan untuk membuat sebuah filesystem virtual dengan FUSE (Filesystem in Userspace) yang memiliki kemampuan untuk membaca dan mendekripsi file teks secara otomatis berdasarkan nama file dan metode enkripsi yang telah ditentukan.

Berikut adalah penjelasan singkat tentang setiap bagian dari code tersebut:

`FUSE_USE_VERSION 30`: Menentukan versi API FUSE yang digunakan.
`Header Files`: Mengimpor pustaka yang dibutuhkan seperti FUSE, standard I/O, standard library, dan OpenSSL untuk operasi dekripsi

Mendefinisikan direktori `pesan` dan `rahasia-berkas`.
`Password` yang digunakan dalam kode ini.

Fungsi ``Dekripsi`: Deklarasi fungsi dekripsi yang akan dijelaskan lebih lanjut di bagian implementasi fungsi.

Fungsi `getattr`: Mengambil atribut file atau direktori seperti ukuran, waktu modifikasi, dan lain-lain.

Fungsi `Read`: Membaca isi file dan mendekripsi isinya jika file tersebut berada di direktori `pesan` dan memiliki nama yang sesuai dengan format enkripsi tertentu. 

Implementasi Fungsi `decrypt`: 
`Base64`: Menggunakan OpenSSL untuk mendekripsi teks yang dienkode dengan `Base64`.
`ROT13`: Mendekripsi teks dengan mengganti setiap huruf dengan huruf yang 13 posisi setelahnya dalam alfabet.
`Hexadecimal`: Mengkonversi string hex menjadi karakter ASCII.
`Reverse`: Membalikkan urutan karakter dalam string.

Struktur `fuse_operations`: Mendefinisikan operasi FUSE yang digunakan (`getattr` dan `read`).

Fungsi `main`: Fungsi utama yang memulai FUSE file system dengan operasi yang telah didefinisikan.

**Ketiga**

Untuk langkah terakhir, kita akan melakukan pengujian terkait perintah diatas.

### Langkah-Langkah Pengujian

Mounting File System
```sh
`./pastibisa /home/malvin/sisopmodul4/mountpoint -o nonempty`
```

Akses Direktori dan File
`Navigasi ke direktori mountpoint`:
```sh
cd /path_anda/mountpoint
```

`Akses file yang ada di dalam direktori pesan`:
```sh
cat pesan/enkripsi_rot13.txt
cat pesan/notes-base64.txt
cat pesan/new-hex.txt
cat pesan/rev-text.txt
cat pesan/halo.txt
```

`Mengakses file dalam direktori rahasia-berkas`
```sh
cd /path_anda/rahasia-berkas
ls
```


### kendala
Kendala yang saya alami adalah bahwa file `pastibisa.c` hanya dapat dijalankan dan membuka file yang terenkripsi, namun teks tidak dapat terenkripsi dengan benar. Selain itu, terjadi kesalahan di mana Linux secara otomatis membaca perintah bash yang salah.

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
    // Kode
}
```

Hal pertama yang dilakukan adalah untuk mempersiapkan variabel untuk path lengkap dan path dari setiap part yang akan di delete:

```c
    // Get full path of item and prep part buffer
    char fpath[MAX_BUFFER];
    char ppath[MAX_BUFFER+4];
    sprintf(fpath, "%s%s",dirpath,path);
```

Lalu dimulai loop melewati seluruh part dari sebuah file utuh dan melakukan unlink untuk delete:

```c
    int pcurrent = 0;    
    while(1){
        // Loop through all parts
        sprintf(ppath, "%s.%03d", fpath, pcurrent++);
```

Untuk unlink dan menunjukkan output, terdapat 3 kemungkinan
* Delete berhasil dan tidak ada output tambahan
* Delete gagal karena tidak ada file, maka break akan dilakukan dari while loop, bisa berarti bahwa tidak ada file tambahan
* Error selain tidak ada file, return error number

```c
        int res = unlink(ppath);
        if (res == -1){
            if (errno == ENOENT) break;
            return -errno;
        }
    }
```
### Tambahan: Fungsi Time

Fungsi tambahan ini diperlukan saat membuat sebuah file karena diperlukan oleh FUSE untuk mencatat secara real-time. Menggunakan cara yang cukup mirip dengan sebelum-sebelumnya untuk melakukan looping melalui semua part dari satu file gabungan, fungsinya sebagai berikut:

```c
// Function to get time attr by the ns (nanosecond)
static int arc_utimens(const char *path, const struct timespec ts[2])
{
    // Get full path of item
    char fpath[MAX_BUFFER];
    sprintf(fpath, "%s%s", dirpath, path);

    // Buffers for part looping
    char ppath[MAX_BUFFER+4];
    int pcurrent = 0;

    // Loop through all parts
    while (1) {
        sprintf(ppath, "%s.%03d", fpath, pcurrent++);
        int res = utimensat(AT_FDCWD, ppath, ts, 0);
        if (res == -1) {
            if (errno == ENOENT) break;
            return -errno;
        }
    }

    return 0;
}
```

### Samba Server

Direktori report akan berisi seluruh relic yang sudah digabung melalui FUSE, dari FUSE akan dicopy ke dalam report dengan command `cp <fuse>/* report` dan dapat dimulai sebuah Samba server untuk membagikan folder dengan cara menambahkan contoh konfigurasi sebagai berikut:

```conf
[report]
    comment = Archeology Relic Report
    path = /home/winter/Documents/ITS/SISOP/Modul4/soal_3/report
    read only = no
    browsable = yes
    writable = yes
    guest ok = no
```

### Dokumentasi

#### Listing
![list](https://media.discordapp.net/attachments/1071478813566976151/1242728335646130217/Screenshot_20240522_133143.png?ex=664ee473&is=664d92f3&hm=be727694c0bc3961f3231f4a008eba3bc5cde68e17308ccd48a833d8d2c7f6ed&=&format=webp&quality=lossless&width=1210&height=681)

#### Copy ke luar FUSE
![out copy](https://media.discordapp.net/attachments/1071478813566976151/1242728335088291871/Screenshot_20240522_133231.png?ex=664ee473&is=664d92f3&hm=b4d652b86e879d2c7bdd5b0620845434114f0565e7e72d9babd0c5a0323bfb55&=&format=webp&quality=lossless&width=1210&height=681)

#### Copy ke dalam FUSE
![in copy](https://media.discordapp.net/attachments/1071478813566976151/1242728334513803285/Screenshot_20240522_133314.png?ex=664ee473&is=664d92f3&hm=e36d0973183c5a2963a871309edc5bd7f485167670969ee20e856cd68ba1a305&=&format=webp&quality=lossless&width=1210&height=681)

#### Delete file melalui FUSE
![delete](https://media.discordapp.net/attachments/1071478813566976151/1242728334123597864/Screenshot_20240522_133346.png?ex=664ee473&is=664d92f3&hm=79f29ccc6f3a9ee1a2256c6badc1d591377757784eaaabf2ce35e29b3713c4e2&=&format=webp&quality=lossless&width=1210&height=681)

#### Samba Server 
![samba](https://media.discordapp.net/attachments/1071478813566976151/1242728333658165318/Screenshot_20240522_133742.png?ex=664ee473&is=664d92f3&hm=925299523689aaa12e81b4937709a0e68e05a29e9ce334db4986618fd34f0a9c&=&format=webp&quality=lossless&width=1210&height=681)

### Kendala

Tidak ada, dari yang saya temukan.
