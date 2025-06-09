# Kurulum Linux OS
Dosyaları klasöre çıkarttıktan sonra:

## 1. Seçenek
(opsiyonel) eğer izin reddedildi gibi bir hata çıkarsa ```chmod +x <shell_dosya>``` komutu ile shell dosyasına izin verilmelidir.
- ```build.sh``` çalıştırılarak cmake / make operasyonları ile dosyaları derle
- Oluşan ```'build/'``` klasöründe bulunan ```mpi_parallel_sort_simulator``` isimli executable'ı çalıştır. 

- ör: ```./mpi_parallel_sort_simulator```

- örnek komut: ```./mpi_parallel_sort_simulator <İŞLEMCİ_SAYISI> <İŞLEMCİ_BAŞINA_DÜŞEN_ELEMAN_SAYISI>```

- örnek komut: ```./mpi_parallel_sort_simulator 32 10000```

## 2. Seçenek
(opsiyonel) eğer izin reddedildi gibi bir hata çıkarsa ```chmod +x <shell_dosya>``` komutu ile shell dosyasına izin verilmelidir.
- ```build_and_run.sh``` dosyası çalıştırılarak derleme ve çıktıyı çalıştırma operasyonlarını otomotize et, oluşturulan ```mpi_parallel_sort_simulator``` programını otomatik çalıştır.


## 3. Seçenek
(opsiyonel) eğer izin reddedildi gibi bir hata çıkarsa ```chmod +x <shell_dosya>``` komutu ile shell dosyasına izin verilmelidir.
- ```build.sh``` dosyası çalıştırılarak cmake / make derleme operasyonlarını otomotize et
- Derleme bittikten sonra ```run.sh``` dosyasını çalıştırarak programı default 5 işlemci_sayısı ile aç
