#define _CRT_SECURE_NO_WARNINGS

#include "multispec.h"

#define DEBUG
//#define NDEBUG
#include <assert.h>

//////////// ������ � ������� /////////////////

/* ������ �����, ����������� ������������ ����� */
/* � �������� ��������� ���������� ���������� ����� ������������� ������ �
   ��������� �� ������, � ������� �������� ������������ ������                             */
   /* �������� ������������ ����� ����������� � ���� ������� �������� (������).
      ������ �������� �����������, ��� ������ ������������ �������� �������� ��������:
      inputXX.bmp, ��� XX - ���������� ���������� �����                                     */

int channel_input(int numb, Spectr* channel)
{
    char inp_file_name[] = { "input00.bmp" };
    FILE* inp_file = NULL;

    // ������������ �������� ��������� �����
    inp_file_name[5] = (char)(numb / 10) + '0'; // ������ ������ ����� ������ (�������)
    inp_file_name[6] = (char)(numb % 10) + '0'; // ������ ������ ����� ������ (�������)
       
    /* ���� ������� ���� � ��������� ������ ����������� � �������� � �� ��� ������, ��
       ���������� ������ ��������� ����������.  */
    inp_file = fopen(inp_file_name, "rb");

    // ���� �� �������� � ������ �������, �� ������������, ��� ����� ����� �� ����
#ifdef DEBUG
    if (inp_file == NULL)
        return -1;
    // ������ �����
    file_read(inp_file, channel);

    fclose(inp_file);

    return 1;
#else
    // ����� �������, ��� ��������� ���� ����� ��������� � ���������� � �������� ��� ������
    // ������ �����
    file_read(inp_file, channel);

    fclose(inp_file);

    return 1;
#endif
}

// �������, ����������� ���������� �������� ����� �� ������������ �������
void file_read(FILE* f, Spectr* channel)
{
    /* ������ assert() ������������ ����������� �������� BITMAPFILEHEADER (14 ���) 
       � BITMAPINFOHEADER (40 ���), ��� ����������� ���������� ����������          */
    /* ��� ���������� ������� ����� ���������� ������������� ����� assert.h 
       ���������� ���������� ������ NDEBUG                                         */
    assert((sizeof(channel->file) == 14) && (sizeof(channel->info) == 40));

    // ���������� ��������� ��������� �����
    channel->file.bfType = read_u16(f);
    channel->file.bmpSize = read_u32(f);
    channel->file.Reserved1 = read_u16(f);
    channel->file.Reserved2 = read_u16(f);
    channel->file.OffBits = read_u32(f);

    // ���������� ��������� ��������� �����������
    channel->info.biSize = read_u32(f);
    channel->info.Width = read_u32(f);
    channel->info.Height = read_u32(f);
    channel->info.Planes = read_u16(f);
    channel->info.BitCount = read_u16(f);
    channel->info.Compression = read_u32(f);
    channel->info.SizeImage = read_u32(f);
    channel->info.XPelsPerMeter = read_u32(f);
    channel->info.YPelsPerMeter = read_u32(f);
    channel->info.ClrUsed = read_u32(f);
    channel->info.ClrImportant = read_u32(f);

    // ���������� �������
    for (int i = 0; i < 256; i++)
    {
        channel->rgb[i].rgbBlue = fgetc(f);
        channel->rgb[i].rgbGreen = fgetc(f);
        channel->rgb[i].rgbRed = fgetc(f);
        channel->rgb[i].rgbReserved = fgetc(f);
    }

    // ���������� ��������� ���������

    // ��������� ������ ��� ��������
    int mem_check = 1;
    channel->pixels = (BYTE**)malloc(channel->info.Height * sizeof(BYTE*));

    if (!channel->pixels)   // �������� ��������� ������
        mem_check = 0;      

    for (int i = 0; i < channel->info.Height; i++)
    {
        channel->pixels[i] = (BYTE*)malloc(channel->info.Width * sizeof(BYTE));
        if (!channel->pixels[i])   // �������� ��������� ������
            mem_check = 0;
    }
    /* ������ assert() �������� �������� ���������� ���������, ���� �� ����� �������� ������ 
       ��� �������� ��������� ��������� �����������  */
    assert(mem_check);

    // ���������� �������������� ������ (��� �������)
    DWORD padding = 4 - (channel->info.Width % 4);
    for (int i = 0; i < channel->info.Height; i++)
    {
        for (int j = 0; j < channel->info.Width; j++)
        {
            channel->pixels[i][j] = fgetc(f);
        }
        if (padding != 4)
            for (int j = 0; j < padding; j++)
                fgetc(f);
    }

}

/* ������ �����, ����������� ������������ ����� */
/* � �������� ��������� ���������� ���������� ����� ������������� ������ �
   ��������� �� ������, � ������� �������� ������������ ������                           */
   /* �������� ������������ ����� ����������� � ���� ������� �������� (������).
      ������ �������� �����������, ��� ������ ������������ �������� �������� ��������:
      outputXX.bmp, ��� XX - ���������� ���������� �����                                     */
void channel_output(int numb, Spectr* channel)
{
    // ����� ���������� ������ �����������
    char out_file_name[] = { "output00.bmp" };
    FILE* out_file = NULL;
    // ������������ �������� ��������� �����
    // ����� �������� 5 � 6 ������� ������
    out_file_name[6] = (char)(numb / 10) + '0';
    out_file_name[7] = (char)(numb % 10) + '0';

    out_file = fopen(out_file_name, "wb");

    // ������ �����
    file_write(out_file, channel);

    fclose(out_file);
}

// �������, ����������� ���������� �������� ����� �� ������������ �������
void file_write(FILE* f, Spectr* channel)
{
    // ������ ���� �������� ����������
    fwrite(&channel->file, sizeof(BITMAPFILEHEADER), 1, f);
    fwrite(&channel->info, sizeof(BITMAPINFOHEADER), 1, f);

    // ������ �������
    for (int i = 0; i < 256; i++)
        fwrite(&channel->rgb[i], sizeof(RGBQUAD), 1, f);

    // ������ ��������� ��������� �����������
    BYTE s;
    DWORD padding = 4 - (channel->info.Width % 4);
    // ������ �����
    for (int i = 0; i < channel->info.Height; i++)
    {
        // ������ ��������
        for (int j = 0; j < channel->info.Width; j++)
        {
            s = channel->pixels[i][j];
            fwrite(&s, sizeof(BYTE), 1, f); // ������ �������
        }
        if (padding != 4)       // ���� ����� �������������� �����
            for (int j = 0; j < padding; j++)
            {
                s = 0;
                fwrite(&s, sizeof(BYTE), 1, f);
            }
    }

    // ������������ ������, ���������� ��� �������� ������� ��������
    for (int i = 0; i < channel->info.Height; i++)
    {
        free(channel->pixels[i]);
        channel->pixels[i] = NULL;
    }
    free(channel->pixels);
    channel->pixels = NULL;
}

// ������ 16-������� ������������ ��������
static unsigned short read_u16(FILE* fp)
{
    unsigned char b0, b1;

    b0 = getc(fp);
    b1 = getc(fp);
    
    return ((b1 << 8) | b0);
}

// ������ 32-������� ������������ ��������
static unsigned int read_u32(FILE* fp)
{
    unsigned char b0, b1, b2, b3;

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}


/////// ������� �� ������� //////////////

/* ������� �������� ���������� ������ �� ������� ������� �� ���� ANGLE */
/* ����� ����������� ����������� �� ���� ������������ ������� ����������� ����������,
       ���� <������ ������������ �������> ����� ����� ������ �������� "00" */
void rotate(int angle, Spectr* channel)
{
    switch (angle)
    {
    case 0:
    {
        // ������ �� ������
        break;
    }
    case 90:
    {
        // ������ ������� ������ � ������
        DWORD temp0 = channel->info.Height;
        channel->info.Height = channel->info.Width;
        channel->info.Width = temp0;

        DWORD height = channel->info.Height;
        DWORD width = channel->info.Width;
        // �������� ������ ��� ������ �������
        BYTE** temp = (BYTE**)malloc(sizeof(BYTE*) * height);
        for (int i = 0; i < height; i++)
            temp[i] = (BYTE*)malloc(width * sizeof(BYTE));

        // ���������� � ����� ������ ����������������� �������
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                temp[i][j] = channel->pixels[j][i];

        // ��������� � ��������� ������ �� ����� ������
        BYTE** old = channel->pixels;
        channel->pixels = temp;

        // �������� ��������� �� �����������
        flip('H', channel);

        // ������������ ����� ���������� ������
        // � ������������� ������� ������ ����� ������� ������
        for (int i = 0; i < width; i++)
        {
            free(old[i]);
            old[i] = NULL;
        }
        free(old);
        old = NULL;

        break;
    }
    case 180:
    {
        flip('H', channel); // �������� �� �����������
        flip('V', channel); // �������� �� ���������
        break;
    }
    case 270:
    {
        rotate(180, channel);   // ��������� ������� �� 180 ��������
        rotate(90, channel);    // ��������� ������� �� 90 ��������
        break;
    }
    case 360:
    {
        // ������ �� ������
        break;
    }
    }
}

/* ���������� ��������� ������������ ������������ ��� (���� DIR = 'V')
��� �������������� ��� (���� (DIR='H') */
void flip(char dir, Spectr* channel)
{
    // ��������� ������������ ������������ ���
    if (dir == 'V')
    {
        BYTE temp;
        DWORD height = channel->info.Height;
        DWORD width = channel->info.Width;
        // ������ ������ ������ "�������������" ��������
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < (width / 2); j++)   // ������� �� ��������
            {
                // ������ ������� ��� ��������
                temp = channel->pixels[i][j];
                channel->pixels[i][j] = channel->pixels[i][width - 1 - j];
                channel->pixels[i][width - 1 - j] = temp;
            }
        }
    }
    // ��������� ������������ �������������� ���
    else if (dir == 'H')
    {
        BYTE* temp;
        DWORD height = channel->info.Height;
        DWORD width = channel->info.Width;
        // ������� ������ ������� ������ (������ ������ ��������� � �.�.)
        for (int i = 0; i < (height / 2); i++)
        {
            temp = channel->pixels[i];
            channel->pixels[i] = channel->pixels[height - 1 - i];
            channel->pixels[height - 1 - i] = temp;
        }
    }
}

/* ���������� ���������� �������� �� ��������� � V ���, �� ����������� � H ���.
����� ����������� ����������� �� ���� ������������ ������� ����������� ����������,
���� <������ ������������ �������> ����� ����� ������ �������� "00".
������� ����������� ������ � ��� ������, ���� ����� �� ���������� ������ �����������
�� ����� ��������� ���������� �������� �������� �����������
�� ��������� � ����������� ������������,
����������� � ������������ � �������� bmp-����� */
void upscale(int V, int H, Spectr* channel)
{
    DWORD height = channel->info.Height;
    DWORD width = channel->info.Width;
    // �������� ������������ ���������
    if ((height * V > H_max) || (width * H > W_max))
        return;

    // ������ ����� �������� ������, ������ � �������� �����
    channel->info.Height = height * V;
    channel->info.Width = width * H;
    channel->info.SizeImage = height * V * width * H;
    // ��������� �����, ��������� �����������, �������
    channel->file.bmpSize = 14 + 40 + 1024 + channel->info.SizeImage;
    // ��������� ���������
    DWORD padding = 4 - (channel->info.Width % 4);
    if (padding != 4)
        channel->file.bmpSize += (padding * channel->info.Height);

    // ��������� ������ ��� ������ �������
    BYTE** temp = (BYTE**)malloc(sizeof(BYTE*) * height * V);
    for (int i = 0; i < (height * V); i++)
        temp[i] = (BYTE*)malloc(width * H * sizeof(BYTE));

    // �����������
    for (int ii = 0, i = 0; i < height; ii += V, i++)
        for (int jj = 0, j = 0; j < width; jj += H, j++)
        {
            for (int k = 0; k < V; k++)
                for (int kk = 0; kk < H; kk++)
                    temp[ii + k][jj + kk] = channel->pixels[i][j];
        }

    // ��������� � ��������� ������ �� ����� ������
    BYTE** old = channel->pixels;
    channel->pixels = temp;

    // ������������ ����� ���������� ������
    // � ������������� ������� ������ ����� ������� ������
    for (int i = 0; i < height; i++)
    {
        free(old[i]);
        old[i] = NULL;
    }
    free(old);
    old = NULL;
}

/* ���������� ���������� �������� �� ��������� � V ���, �� ����������� � H ���.
����� ����������� ����������� �� ���� ������������ ������� ����������� ����������,
���� <������ ������������ �������> ����� ����� ������ �������� "00".
������� ����������� ������ � ��� ������, ���� �������� V � H ������ ������� ��������
����������� ROWS � COLS �� ������ �������������� �������� ���������� ������������.
� ��������� ������ ������� ��������� ������������ */
void downscale(int V, int H, Spectr* channel)
{
    DWORD height = channel->info.Height;
    DWORD width = channel->info.Width;
    // �������� ���������
    if ((height % V != 0) || (width % H != 0))
        return;

    // ������ ����� �������� ������, ������ � �������� �����
    channel->info.Height = height / V;
    channel->info.Width = width / H;
    channel->info.SizeImage = channel->info.Height * channel->info.Width;
    // ��������� �����, ��������� �����������, �������
    channel->file.bmpSize = 14 + 40 + 1024 + channel->info.SizeImage;
    // ��������� ���������
    DWORD padding = 4 - (channel->info.Width % 4);
    if (padding != 4)
        channel->file.bmpSize += (padding * channel->info.Height);

    // ��������� ������ ��� ������ �������
    BYTE** temp = (BYTE**)malloc(sizeof(BYTE*) * channel->info.Height);
    for (int i = 0; i < channel->info.Height; i++)
        temp[i] = (BYTE*)malloc(channel->info.Width * sizeof(BYTE));

    // �����������
    for (int i = 0, ii = 0; i < height; i += V, ii++)
        for (int j = 0, jj = 0; j < width; j += H, jj++)
            temp[ii][jj] = channel->pixels[i][j];

    // ��������� � ��������� ������ �� ����� ������
    BYTE** old = channel->pixels;
    channel->pixels = temp;

    // ������������ ����� ���������� ������
    // � ������������� ������� ������ ����� ������� ������
    for (int i = 0; i < height; i++)
    {
        free(old[i]);
        old[i] = NULL;
    }
    free(old);
    old = NULL;
}

/* ��������� ������� �������� ���������� ����� ������ �� ��������
����� (��� f>0) ��� ������ (��� f<0) �� f ��������.
 */
void bright(int f, Spectr* channel)
{
    DWORD height = channel->info.Height;
    DWORD width = channel->info.Width;
    BYTE temp;
    if ((f > 0) && (f <= 8))
    {
        // ����������� ��������� ����� ����� ���� ��������� �������
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                temp = channel->pixels[i][j];
                temp = temp << f;
                channel->pixels[i][j] = temp;
            }
        }
    }
    else
        if ((f < 0) && (f >= -8))
        {
            f *= (-1);  // ������ ����
            // ����������� ��������� ����� ������ ���� ��������� �������
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    temp = channel->pixels[i][j];
                    temp = temp >> f;
                    channel->pixels[i][j] = temp;
                }
            }
        }
}
