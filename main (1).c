#include <16F877A.h>// PIC16F877A mikrodenetleyicisi kütüphanesi
#device ADC=10      // ADC 10 bit çözünürlük ayarý
#include <string.h>// C dilinin string (metin) iþlemleri için kütüphane 
#fuses NOWDT,NOPROTECT,NOLVP,HS// Watchdog Timer devre dýþý NOPROTECT: Kod korumasý kapalý  NOLVP: Low Voltage Programming kapalý  HS: High-Speed kristal osilatör(20 MHz)
#use delay(clock=20000000)// Sistemin çalýþma frekansýný 20 MHz(delay_ms vb. fonksiyonlar doðru çalýþýr)
#include <math.h>

#define LCD_ENABLE_PIN PIN_B0 // LCD'nin Enable pini B0 pinine baðlanmýþtýr
#define LCD_RS_PIN PIN_B1
#define LCD_RW_PIN PIN_B2
#define LCD_DATA4 PIN_B4
#define LCD_DATA5 PIN_B5
#define LCD_DATA6 PIN_B6
#define LCD_DATA7 PIN_B7
#define USE_PORTB_LCD TRUE

#include <lcd420.c>//4 satýr 20 karakter LCD kütüphanesi

#define SATIR1 PIN_D0
#define SATIR2 PIN_D1
#define SATIR3 PIN_D2
#define SATIR4 PIN_D3
#define SUTUN1 PIN_D4
#define SUTUN2 PIN_D5
#define SUTUN3 PIN_D6
#define key_debounce 20

#define RL 4.7      // yük direnci kilo-ohm
#define R0 10.0     // temiz havada ölçülen RS
#define PPM_ESIK 600  // ppm eþik deðeri

#define LED_PIN PIN_C0
#define SPEAKER_PIN PIN_C3
#define LDR_ESIK_VOLT 3.0

char dogruSifre[10] = "135791113";
char girilenSifre[10] = "";
int sayac = 0;
int hataliDeneme = 0;
int girisModu = 0;
int deneme=3;
int1 girisIhlali = 0;
int1 kapiAcik = 0;
int1 sistemKilitli = 0;

char tusOku()
{

   output_high(SATIR1); output_low(SATIR2); output_low(SATIR3); output_low(SATIR4);
   if (input(SUTUN1)) return '1';
   if (input(SUTUN2)) return '2';
   if (input(SUTUN3)) return '3';

   output_low(SATIR1); output_high(SATIR2); output_low(SATIR3); output_low(SATIR4);
   if (input(SUTUN1)) return '4';
   if (input(SUTUN2)) return '5';
   if (input(SUTUN3)) return '6';

   output_low(SATIR1); output_low(SATIR2); output_high(SATIR3); output_low(SATIR4);
   if (input(SUTUN1)) return '7';
   if (input(SUTUN2)) return '8';
   if (input(SUTUN3)) return '9';

   output_low(SATIR1); output_low(SATIR2); output_low(SATIR3); output_high(SATIR4);
   if (input(SUTUN1)) return '*';
   if (input(SUTUN2)) return '0';
   if (input(SUTUN3)) return '#';

   return 0xFF;
}

void sifreyiTemizle()
{
   for (int i = 0; i < 9; i++) girilenSifre[i] = 0;
   sayac = 0;
}

void main()
{
    int16 adc_deger, adc_value, adc_value_term, adc_value_gas;
    float sicaklik, volt, Vout, RS, ratio, ppm;
    float m = -0.42;
    float b = 0.38;

    setup_adc_ports(ALL_ANALOG);
    setup_adc(ADC_CLOCK_INTERNAL);
    lcd_init();
    printf(lcd_putc, "\f*'a Basiniz");

    set_tris_d(0xF0);
    output_low(LED_PIN);
    output_low(SPEAKER_PIN);

    while(TRUE)
    {
        // LDR kontrolü
        set_adc_channel(2);
        delay_us(20);
        adc_deger = read_adc();
        volt = (adc_deger * 5.0) / 1023.0;

        if(volt < LDR_ESIK_VOLT) {
            output_low(LED_PIN);
            output_high(SPEAKER_PIN);
        } else {
            output_low(LED_PIN);
            output_low(SPEAKER_PIN);
        }

        // Þifre sistemi
        set_adc_channel(0);
        adc_value = read_adc();

        if(!kapiAcik)
        {
            if(adc_value < 450)
            {
                if(!girisIhlali)
                {
                    girisIhlali = 1;
                    lcd_putc("\f\n   GIRIS IHLALI!");
                    sifreyiTemizle();
                    girisModu = 0;
                }
                lcd_gotoxy(1, 4);
                printf(lcd_putc, "ADC:%4lu   ", adc_value);
                output_high(PIN_C0);
                delay_ms(300);
                output_low(PIN_C0);
                delay_ms(300);
            }
            else
            {
                if(girisIhlali)
                {
                    girisIhlali = 0;
                    lcd_putc("\f*'a Basiniz");
                }
                output_low(PIN_C0);
            }
        }
        else
        {
            output_low(PIN_C0);
        }

        if(!girisIhlali && !kapiAcik)
        {
            char tus = tusOku();
            if (tus != 0xFF)
            {
                delay_ms(key_debounce);
                while(tusOku() != 0xFF);

                if (!girisModu && tus == '*')
                {
                    sifreyiTemizle();
                    girisModu = 1;
                    lcd_putc("\fSifre Giriniz:\n");
                }
                else if (girisModu && tus != '#' && tus!= '*' && sayac < 9)
                {
                    girilenSifre[sayac++] = tus;
                    lcd_gotoxy(sayac, 2);
                    lcd_putc('*');
                }
                else if (girisModu && tus == '#')
                {
                    girilenSifre[sayac] = '\0';
                    if (strcmp(girilenSifre, dogruSifre) == 0)
                    {
                        lcd_putc("\fGiris Basarili");
                        output_high(PIN_C1);
                        kapiAcik = 1;
                        delay_ms(5000);
                        output_low(PIN_C1);
                        kapiAcik = 0;
                        hataliDeneme = 0;
                        lcd_putc("\f*'a Basiniz");
                    }
                    else
                    {
                        deneme--;
                        lcd_putc("\fYanlis Sifre");
                        printf(lcd_putc,"\nSon %d deneme", deneme);
                        if(deneme==0)
                        {
                            lcd_gotoxy(1,2);
                            printf(lcd_putc,"                ");
                            deneme=3;
                        }
                        delay_ms(2000);
                        hataliDeneme++;

                        if (hataliDeneme >= 3)
                        {
                            lcd_putc("\fHatali Giris\nSistem Kilitlendi");
                            delay_ms(2000);
                            sistemKilitli = 1;
                            int kalanSure = 60;
                            while(kalanSure > 0)
                            {
                                adc_value = read_adc();
                                if (adc_value < 450)
                                {
                                    if (!girisIhlali)
                                    {
                                        girisIhlali = 1;
                                        lcd_putc("\fGIRIS IHLALI!");
                                    }
                                    output_high(PIN_C0);
                                }
                                else
                                {
                                    if (girisIhlali)
                                    {
                                        girisIhlali = 0;
                                        lcd_putc("\fSistem Kitlendi");
                                    }
                                    output_low(PIN_C0);
                                    lcd_gotoxy(1, 2);
                                    printf(lcd_putc, "Kalan: %2d sn       ", kalanSure);
                                }

                                delay_ms(1000);
                                kalanSure--;
                            }
                            girisIhlali=0;
                            sistemKilitli = 0;
                            hataliDeneme = 0;
                            lcd_putc("\f*'a Basiniz");
                        }
                    }
                    lcd_putc("\f*'a Basiniz");
                    sifreyiTemizle();
                    girisModu = 0;
                }
            }
        }

        // ?? Yangýn sensörü (gaz + sýcaklýk)
        set_adc_channel(3);
        delay_us(20);
        adc_value_term = read_adc();
        sicaklik = (adc_value_term * 5.0 / 1023.0) * 100;

        set_adc_channel(1);
        delay_us(20);
        adc_value_gas = read_adc();
        Vout = adc_value_gas * 5.0 / 1023.0;
        RS = RL * (5.0 - Vout) / Vout;
        ratio = RS / R0;
        ppm = pow(10, (log10(ratio) - b) / m);

        if(ppm > 600 && sicaklik > 50)
        {
            output_high(PIN_C2);
            delay_ms(500);
            output_low(PIN_C2);
            delay_ms(500);
        }
        else
        {
            output_low(PIN_C2);
        }
         
        lcd_gotoxy(1, 4);
        printf(lcd_putc, "PPM: %.0f   ", ppm);
        delay_ms(100);
    }
}
