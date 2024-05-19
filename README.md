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

