# Linux Syslog Yöneticisi (Çift Yönlü Bağlı Liste İle)

Bu proje, Linux işletim sistemindeki sistem günlüklerinin (`syslog`) C programlama dili ve **Çift Yönlü Bağlı Liste (Doubly Linked List)** veri yapısı kullanılarak bellekte tutulmasını ve işlenmesini simüle eden bir konsol uygulamasıdır.

## 📌 Projenin Amacı
Sistemde oluşan log dosyalarının, doğru veri yapısı seçimi ile bellek üzerinde ne kadar efektif işlenebileceğini göstermektir. Standart bir dizi (array) yerine bağlı liste kullanılarak dinamik bellek yönetimi sağlanmış ve logların sayısından bağımsız, esnek bir yapı kurgulanmıştır.

## ⚙️ Veri Yapısı Tercihi: Neden Çift Yönlü Bağlı Liste?

Projede veri yapısı olarak Çift Yönlü Bağlı Liste ve Kuyruk (Tail) işaretçisi kullanılmasının iki temel teknik nedeni vardır:

1. **O(1) Karmaşıklığında Sona Ekleme:** Syslog kayıtları doğası gereği kronolojik olarak sürekli dosyanın sonuna eklenir. Kuyruk (tail) işaretçisi sayesinde yeni gelen log, tüm listeyi baştan sona gezmeye gerek kalmadan anında listenin en sonuna eklenir.
2. **Sondan Başa (Reverse) Okuma:** Sistem yöneticileri hata ayıklarken (debugging) arama işlemine daima en güncel logdan başlayıp en eskiye doğru giderler. Çift yönlü düğümlerdeki `onceki` (prev) işaretçisi sayesinde, listeyi kuyruktan başa doğru okumak son derece performanslıdır.

## 🚀 Kurulum ve Çalıştırma
Projeyi kendi bilgisayarınızda derlemek ve çalıştırmak için sisteminizde bir C derleyicisi (örneğin GCC) kurulu olmalıdır.

1. Repoyu bilgisayarınıza klonlayın:
   git clone https://github.com/KULLANICI_ADINIZ/linux-syslog-analyzer.git

2. Proje dizinine gidin ve kaynak kodu derleyin:
   gcc main.c -o syslog_app

3. Uygulamayı çalıştırın:
   ./syslog_app

## 📁 Proje Yapısı
* `main.c`: Bağlı liste veri yapısını, düğüm oluşturma, listeye ekleme ve tersten okuma fonksiyonlarını barındıran ana kaynak kodu.
* `syslog.txt`: Uygulama çalıştığında okuma ve ayrıştırma işlemlerini test edebilmek için program tarafından otomatik olarak oluşturulan örnek log dosyası.

## 💻 Örnek Çıktı
Uygulama çalıştırıldığında logları en yeniden en eskiye doğru şu formatta listeler:

Linux Syslog Bagli Liste Uygulamasi Baslatiliyor...

--- SYSLOG KAYITLARI (En Yeniden En Eskiye Dogru) ---

1. Log -> Tarih: Oct_12_10:25 | Sunucu: Ubuntu-PC | Servis: SSHD 
   Mesaj: Basarisiz_parola_denemesi
2. Log -> Tarih: Oct_12_10:18 | Sunucu: Ubuntu-PC | Servis: Apache 
   Mesaj: Web_sunucusu_baslatildi
3. Log -> Tarih: Oct_12_10:15 | Sunucu: Ubuntu-PC | Servis: Kernel 
   Mesaj: Sisteme_yeni_bir_usb_baglandi

------------------------------------------------------
