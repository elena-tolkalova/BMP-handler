#ifndef MULTISPEC_H_INCLUDED
#define MULTISPEC_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

#define N_chan 100   // максимальное количество спектральных каналов
#define N_com 100    // максимальное количество спектральных команд
#define W_max 2200   // максимальная ширина
#define H_max 2000   // максимальная высота

typedef uint32_t DWORD; // 4 байта
typedef uint16_t WORD;  // 2 байта
typedef uint8_t BYTE;   // 1 байт

// Установим выравнивание в 1 байт. Без этого размер структур может отличаться от ожидаемого
#pragma pack(push, 1)
// Заголовок bmp-файла
typedef struct
{
    WORD bfType;         // Сигнатура bmp-файла
    DWORD bmpSize;       // Размер всего bmp-файла в байтах
    WORD Reserved1;      // Поле зарезервировано (должно быть равно 0)
    WORD Reserved2;      // Поле зарезервировано (должно быть равно 0)
    DWORD OffBits;       // Смещение массива пикселов от начала файла 
} BITMAPFILEHEADER;

// Описание формата изображения
typedef struct
{
    DWORD biSize;        // Размер структуры BITMAPINFOHEADER в байтах. Всегда равен 40
    DWORD Width;         // Ширина картинки в пикселах. Не больше 1000
    DWORD Height;        // Высота картинки в пикселах. Не больше 1000
    WORD Planes;         // Количество цветовых плоскостей. Равно 1
    WORD BitCount;       // Количество битов на 1 пиксел. Равно 8
    DWORD Compression;   // Равно 0
    DWORD SizeImage;     // Размер массива пикселов в байтах
    DWORD XPelsPerMeter; // Равно 0
    DWORD YPelsPerMeter; // Равно 0
    DWORD ClrUsed;       // Количество цветов в палитре. Равно 0
    DWORD ClrImportant;  // Количество цветов в палитре, необходимых для отображения изображения. Равно 0

} BITMAPINFOHEADER;

// Палитра
// Первые три поля имеют одинаковые значения
typedef struct
{
    BYTE rgbBlue;        // Значения 1..255
    BYTE rgbGreen;       // Значения 1..255
    BYTE rgbRed;         // Значения 1..255
    BYTE rgbReserved;    // Равно 0
} RGBQUAD;

// Возвращаем прошлую настройку выравнивания
#pragma pack(pop)

// Растровая развертка изображения

// Структура, описывающая спектральный канал. Содержит все считываемые из исходного файла данные
typedef struct
{
    BITMAPFILEHEADER file; // заголовок файла
    BITMAPINFOHEADER info; // заголовок изображения
    RGBQUAD rgb[256];      // палитра
    BYTE** pixels;         // растровая развертка изображения
}Spectr;

void rotate(int angle, Spectr* channel);         // поворот по часовой стрелке
void flip(char dir, Spectr* channel);            // отражение по вертикали или горизонтали
void upscale(int V, int H, Spectr* channel);     // увеличение количества пикселов
void downscale(int V, int H, Spectr* channel);   // уменьшение количества пикселов
void bright(int f, Spectr* channel);             // изменение яркости

int channel_input(int numb, Spectr* channel);   // работа со входным файлом
void file_read(FILE* f, Spectr* channel);        // считывание входного файла
void channel_output(int numb, Spectr* channel);  // работа с выходным файлом
void file_write(FILE* f, Spectr* channel);       // запись выходного файла

static unsigned short read_u16(FILE* fp);   // чтение 16-битного значения
static unsigned int   read_u32(FILE* fp);   // чтение 32-битного значения

#endif // MULTISPEC_H_INCLUDEDs

