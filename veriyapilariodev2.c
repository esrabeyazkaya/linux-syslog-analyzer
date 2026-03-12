#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * LOG DÜÐÜMÜ VERÝ YAPISI
 * Amacý: Syslog dosyasýndaki her bir satýrdaki bilgileri (tarih, sunucu, vb.) 
 * tek bir yapý içinde tutmak için tasarlanmýþtýr. Çift yönlü baðlý liste mantýðýyla çalýþýr.
 */
struct LogDugumu {
    char tarih[50];      // Logun oluþma zamanýný tutmak amacýyla tasarlanmýþtýr.
    char sunucu[50];     // Logu üreten bilgisayarýn adýný saklamak için tasarlanmýþtýr.
    char servis[50];     // Hata veren uygulamanýn adýný tutmak için hazýrlanmýþtýr.
    char mesaj[256];     // Logun detaylý hata mesajýný barýndýrmak için hazýrlanmýþtýr.
    
    struct LogDugumu* sonraki; // Listede bir sonraki log kaydýný iþaret etmesi için.
    struct LogDugumu* onceki;  // Listede bir önceki log kaydýný iþaret etmesi için (Tersten okumayý saðlar).
};

// Listenin baþýný ve sonunu takip etmek için hazýrlanan global iþaretçiler
struct LogDugumu* ilkLog = NULL;  // Listenin en baþýndaki logu tutar
struct LogDugumu* sonLog = NULL;  // Listenin en sonundaki logu tutar (Hýzlý ekleme için)

/*
 * YENÝ LOG OLUÞTURMA FONKSÝYONU
 * Amacý: Dosyadan okunan yeni log bilgileri için bellekte yer açmak 
 * ve verileri bu ayrýlan yere kopyalamaktýr.
 */
struct LogDugumu* yeniDugumOlustur(char* p_tarih, char* p_sunucu, char* p_servis, char* p_mesaj) {
    struct LogDugumu* yeni = (struct LogDugumu*)malloc(sizeof(struct LogDugumu));
    
    strcpy(yeni->tarih, p_tarih);
    strcpy(yeni->sunucu, p_sunucu);
    strcpy(yeni->servis, p_servis);
    strcpy(yeni->mesaj, p_mesaj);
    
    yeni->sonraki = NULL;
    yeni->onceki = NULL;
    
    return yeni;
}

/*
 * LÝSTEYE LOG EKLEME FONKSÝYONU
 * Amacý: Gelen yeni log kaydýný, çift yönlü baðlý listenin en sonuna eklemektir.
 * Syslog mantýðýnda yeni loglar hep sona eklendiði için bu yapý tercih edilmiþtir.
 */
void listeyeEkle(char* p_tarih, char* p_sunucu, char* p_servis, char* p_mesaj) {
    struct LogDugumu* yeniDugum = yeniDugumOlustur(p_tarih, p_sunucu, p_servis, p_mesaj);
    
    if (ilkLog == NULL) {
        // Eðer liste tamamen boþsa, yeni eklenen log hem ilk hem son log olur.
        ilkLog = yeniDugum;
        sonLog = yeniDugum;
    } else {
        // Liste doluysa, yeni logu en sona (sonLog'un arkasýna) ekle
        sonLog->sonraki = yeniDugum;
        yeniDugum->onceki = sonLog;
        sonLog = yeniDugum; // Artýk yeni son logumuz bu oldu
    }
}

/*
 * LOGLARI TERSTEN YAZDIRMA FONKSÝYONU
 * Amacý: Sistem yöneticilerinin hatalarý ararken en güncel (en son) logdan 
 * baþlamasý gerektiði için, listeyi sondan baþa doðru ekrana yazdýrmaktýr.
 */
void loglariTerstenGoster() {
    struct LogDugumu* gecici = sonLog; // Aramaya en sondan baþlýyoruz
    int siraNo = 1;
    
    printf("\n--- SYSLOG KAYITLARI (En Yeniden En Eskiye Dogru) ---\n\n");
    
    // gecici deðiþkeni NULL olana kadar (listenin baþýna gelene kadar) geriye git
    while (gecici != NULL) {
        printf("%d. Log -> Tarih: %s | Sunucu: %s | Servis: %s \n   Mesaj: %s\n", 
               siraNo, gecici->tarih, gecici->sunucu, gecici->servis, gecici->mesaj);
        
        gecici = gecici->onceki; // Çift yönlü listenin avantajý: Bir öncekine geçiyoruz
        siraNo++;
    }
    printf("\n------------------------------------------------------\n");
}

/*
 * TEST DOSYASI OLUÞTURMA FONKSÝYONU
 * Amacý: Program çalýþtýðýnda okuyacak bir dosya bulamazsa hata vermesin diye,
 * otomatik olarak örnek bir 'syslog.txt' dosyasý oluþturmaktýr.
 */
void ornekDosyaOlustur() {
    FILE* dosya = fopen("syslog.txt", "w");
    if(dosya != NULL) {
        // Örnek Linux log satýrlarý yazdýrýyoruz
        fprintf(dosya, "Oct_12_10:15 Ubuntu-PC Kernel: Sisteme_yeni_bir_usb_baglandi\n");
        fprintf(dosya, "Oct_12_10:18 Ubuntu-PC Apache: Web_sunucusu_baslatildi\n");
        fprintf(dosya, "Oct_12_10:25 Ubuntu-PC SSHD: Basarisiz_parola_denemesi\n");
        fclose(dosya);
    }
}

/*
 * DOSYADAN OKUMA VE PARÇALAMA FONKSÝYONU
 * Amacý: syslog.txt dosyasýný açýp satýr satýr okumak ve verileri ayrýþtýrýp 
 * baðlý listeye aktarmaktýr.
 */
void dosyadanLoglariOku() {
    FILE* dosya = fopen("syslog.txt", "r");
    if (dosya == NULL) {
        printf("Hata: Dosya bulunamadi!\n");
        return;
    }

    char tarih[50], sunucu[50], servis[50], mesaj[256];
    
    // fscanf fonksiyonu, boþluklara göre satýrdaki kelimeleri ayýrmak için tasarlanmýþtýr
    while (fscanf(dosya, "%s %s %s %s", tarih, sunucu, servis, mesaj) != EOF) {
        listeyeEkle(tarih, sunucu, servis, mesaj);
    }
    
    fclose(dosya);
}

int main() {
    printf("Linux Syslog Bagli Liste Uygulamasi Baslatiliyor...\n");
    
    // 1. Adým: Test için örnek dosya oluþtur (Hata almamak için)
    ornekDosyaOlustur();
    
    // 2. Adým: Dosyadaki verileri oku ve listeye kaydet
    dosyadanLoglariOku();
    
    // 3. Adým: Çift yönlü baðlý listemizi tersten okuyarak ekrana yazdýr
    loglariTerstenGoster();
    
    return 0;
}
