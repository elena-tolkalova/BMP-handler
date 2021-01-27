#ifndef MULTISPEC_H_INCLUDED
#define MULTISPEC_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

#define N_chan 100   // ������������ ���������� ������������ �������
#define N_com 100    // ������������ ���������� ������������ ������
#define W_max 2200   // ������������ ������
#define H_max 2000   // ������������ ������

typedef uint32_t DWORD; // 4 �����
typedef uint16_t WORD;  // 2 �����
typedef uint8_t BYTE;   // 1 ����

// ��������� ������������ � 1 ����. ��� ����� ������ �������� ����� ���������� �� ����������
#pragma pack(push, 1)
// ��������� bmp-�����
typedef struct
{
    WORD bfType;         // ��������� bmp-�����
    DWORD bmpSize;       // ������ ����� bmp-����� � ������
    WORD Reserved1;      // ���� ��������������� (������ ���� ����� 0)
    WORD Reserved2;      // ���� ��������������� (������ ���� ����� 0)
    DWORD OffBits;       // �������� ������� �������� �� ������ ����� 
} BITMAPFILEHEADER;

// �������� ������� �����������
typedef struct
{
    DWORD biSize;        // ������ ��������� BITMAPINFOHEADER � ������. ������ ����� 40
    DWORD Width;         // ������ �������� � ��������. �� ������ 1000
    DWORD Height;        // ������ �������� � ��������. �� ������ 1000
    WORD Planes;         // ���������� �������� ����������. ����� 1
    WORD BitCount;       // ���������� ����� �� 1 ������. ����� 8
    DWORD Compression;   // ����� 0
    DWORD SizeImage;     // ������ ������� �������� � ������
    DWORD XPelsPerMeter; // ����� 0
    DWORD YPelsPerMeter; // ����� 0
    DWORD ClrUsed;       // ���������� ������ � �������. ����� 0
    DWORD ClrImportant;  // ���������� ������ � �������, ����������� ��� ����������� �����������. ����� 0

} BITMAPINFOHEADER;

// �������
// ������ ��� ���� ����� ���������� ��������
typedef struct
{
    BYTE rgbBlue;        // �������� 1..255
    BYTE rgbGreen;       // �������� 1..255
    BYTE rgbRed;         // �������� 1..255
    BYTE rgbReserved;    // ����� 0
} RGBQUAD;

// ���������� ������� ��������� ������������
#pragma pack(pop)

// ��������� ��������� �����������

// ���������, ����������� ������������ �����. �������� ��� ����������� �� ��������� ����� ������
typedef struct
{
    BITMAPFILEHEADER file; // ��������� �����
    BITMAPINFOHEADER info; // ��������� �����������
    RGBQUAD rgb[256];      // �������
    BYTE** pixels;         // ��������� ��������� �����������
}Spectr;

void rotate(int angle, Spectr* channel);         // ������� �� ������� �������
void flip(char dir, Spectr* channel);            // ��������� �� ��������� ��� �����������
void upscale(int V, int H, Spectr* channel);     // ���������� ���������� ��������
void downscale(int V, int H, Spectr* channel);   // ���������� ���������� ��������
void bright(int f, Spectr* channel);             // ��������� �������

int channel_input(int numb, Spectr* channel);   // ������ �� ������� ������
void file_read(FILE* f, Spectr* channel);        // ���������� �������� �����
void channel_output(int numb, Spectr* channel);  // ������ � �������� ������
void file_write(FILE* f, Spectr* channel);       // ������ ��������� �����

static unsigned short read_u16(FILE* fp);   // ������ 16-������� ��������
static unsigned int   read_u32(FILE* fp);   // ������ 32-������� ��������

#endif // MULTISPEC_H_INCLUDEDs

