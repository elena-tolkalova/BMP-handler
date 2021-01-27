#define _CRT_SECURE_NO_WARNINGS

#include "multispec.h"

#define DEBUG
//#define NDEBUG
#include <assert.h>

//////////// РАБОТА С ФАЙЛАМИ /////////////////

/* Чтение файла, содержащего спектральный канал */
/* В качестве аргумента передается порядковый номер спектрального канала и
   указатель на массив, в котором хранятся спектральные каналы                             */
   /* Название открываемого файла формируется в виде массива символов (строки).
      Массив является статическим, его размер определяется заданным форматом названия:
      inputXX.bmp, где XX - двузначный порядковый номер                                     */

int channel_input(int numb, Spectr* channel)
{
    char inp_file_name[] = { "input00.bmp" };
    FILE* inp_file = NULL;

    // формирование названия исходного файла
    inp_file_name[5] = (char)(numb / 10) + '0'; // запись первой цифры номера (десятки)
    inp_file_name[6] = (char)(numb % 10) + '0'; // запись второй цифры номера (единицы)
       
    /* Если входной файл с указанным именем отсутствует в каталоге и не был открыт, то
       дальнейшая работа программы невозможна.  */
    inp_file = fopen(inp_file_name, "rb");

    // если мы работаем в режиме отладки, то предполагаем, что файла может не быть
#ifdef DEBUG
    if (inp_file == NULL)
        return -1;
    // чтение файла
    file_read(inp_file, channel);

    fclose(inp_file);

    return 1;
#else
    // иначе считаем, что указанный файл точно находится в директории и доступен для чтения
    // чтение файла
    file_read(inp_file, channel);

    fclose(inp_file);

    return 1;
#endif
}

// Функция, реализующая считывание входного файла со спектральным каналом
void file_read(FILE* f, Spectr* channel)
{
    /* Макрос assert() контролирует размерность структур BITMAPFILEHEADER (14 бит) 
       и BITMAPINFOHEADER (40 бит), что гарантирует корректное считывание          */
    /* Для выключения макроса перед включением заголовочного файла assert.h 
       необходимо определить макрос NDEBUG                                         */
    assert((sizeof(channel->file) == 14) && (sizeof(channel->info) == 40));

    // заполнение структуры заголовка файла
    channel->file.bfType = read_u16(f);
    channel->file.bmpSize = read_u32(f);
    channel->file.Reserved1 = read_u16(f);
    channel->file.Reserved2 = read_u16(f);
    channel->file.OffBits = read_u32(f);

    // заполнение структуры заголовка изображения
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

    // считывание палитры
    for (int i = 0; i < 256; i++)
    {
        channel->rgb[i].rgbBlue = fgetc(f);
        channel->rgb[i].rgbGreen = fgetc(f);
        channel->rgb[i].rgbRed = fgetc(f);
        channel->rgb[i].rgbReserved = fgetc(f);
    }

    // считывание растровой развертки

    // выделение памяти для хранения
    int mem_check = 1;
    channel->pixels = (BYTE**)malloc(channel->info.Height * sizeof(BYTE*));

    if (!channel->pixels)   // проверка выделения памяти
        mem_check = 0;      

    for (int i = 0; i < channel->info.Height; i++)
    {
        channel->pixels[i] = (BYTE*)malloc(channel->info.Width * sizeof(BYTE));
        if (!channel->pixels[i])   // проверка выделения памяти
            mem_check = 0;
    }
    /* Макрос assert() аварийно завершит выполнение программы, если не будет выделена память 
       для хранения растровой развертки изображения  */
    assert(mem_check);

    // считывание дополнительный байтов (при наличии)
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

/* Запись файла, содержащего спектральный канал */
/* В качестве аргумента передается порядковый номер спектрального канала и
   указатель на массив, в котором хранятся спектральные каналы                           */
   /* Название открываемого файла формируется в виде массива символов (строки).
      Массив является статическим, его размер определяется заданным форматом названия:
      outputXX.bmp, где XX - двузначный порядковый номер                                     */
void channel_output(int numb, Spectr* channel)
{
    // вывод полученных файлов изображения
    char out_file_name[] = { "output00.bmp" };
    FILE* out_file = NULL;
    // формирование название исходного файла
    // нужно заменить 5 и 6 символы строки
    out_file_name[6] = (char)(numb / 10) + '0';
    out_file_name[7] = (char)(numb % 10) + '0';

    out_file = fopen(out_file_name, "wb");

    // запись файла
    file_write(out_file, channel);

    fclose(out_file);
}

// Функция, реализующая считывание входного файла со спектральным каналом
void file_write(FILE* f, Spectr* channel)
{
    // запись двух структур заголовков
    fwrite(&channel->file, sizeof(BITMAPFILEHEADER), 1, f);
    fwrite(&channel->info, sizeof(BITMAPINFOHEADER), 1, f);

    // запись палитры
    for (int i = 0; i < 256; i++)
        fwrite(&channel->rgb[i], sizeof(RGBQUAD), 1, f);

    // запись растровой развертки изображения
    BYTE s;
    DWORD padding = 4 - (channel->info.Width % 4);
    // запись строк
    for (int i = 0; i < channel->info.Height; i++)
    {
        // запись столбцов
        for (int j = 0; j < channel->info.Width; j++)
        {
            s = channel->pixels[i][j];
            fwrite(&s, sizeof(BYTE), 1, f); // запись пикселя
        }
        if (padding != 4)       // если нужны дополнительные байты
            for (int j = 0; j < padding; j++)
            {
                s = 0;
                fwrite(&s, sizeof(BYTE), 1, f);
            }
    }

    // освобождение памяти, выделенной для хранения массива пикселей
    for (int i = 0; i < channel->info.Height; i++)
    {
        free(channel->pixels[i]);
        channel->pixels[i] = NULL;
    }
    free(channel->pixels);
    channel->pixels = NULL;
}

// Чтение 16-битного беззнакового значения
static unsigned short read_u16(FILE* fp)
{
    unsigned char b0, b1;

    b0 = getc(fp);
    b1 = getc(fp);
    
    return ((b1 << 8) | b0);
}

// Чтение 32-битного беззнакового значения
static unsigned int read_u32(FILE* fp)
{
    unsigned char b0, b1, b2, b3;

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}


/////// ФУНКЦИИ ПО ЗАДАНИЮ //////////////

/* Поворот цветовой компоненты вправо по часовой стрелке на угол ANGLE */
/* Чтобы размерность изображений по всем спектральным каналам сохранялась одинаковой,
       поле <Список спектральных каналов> может иметь только значение "00" */
void rotate(int angle, Spectr* channel)
{
    switch (angle)
    {
    case 0:
    {
        // ничего не делаем
        break;
    }
    case 90:
    {
        // меняем местами ширину и высоту
        DWORD temp0 = channel->info.Height;
        channel->info.Height = channel->info.Width;
        channel->info.Width = temp0;

        DWORD height = channel->info.Height;
        DWORD width = channel->info.Width;
        // выделяем память для нового массива
        BYTE** temp = (BYTE**)malloc(sizeof(BYTE*) * height);
        for (int i = 0; i < height; i++)
            temp[i] = (BYTE*)malloc(width * sizeof(BYTE));

        // записываем в новый массив транспонированную матрицу
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                temp[i][j] = channel->pixels[j][i];

        // сохраняем в структуре ссылку на новый массив
        BYTE** old = channel->pixels;
        channel->pixels = temp;

        // отражаем результат по горизонтали
        flip('H', channel);

        // освобождение ранее выделенной памяти
        // в освобождаемом массиве высота равна текущей ширине
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
        flip('H', channel); // отражаем по горизонтали
        flip('V', channel); // отражаем по вертикали
        break;
    }
    case 270:
    {
        rotate(180, channel);   // выполняем поворот на 180 градусов
        rotate(90, channel);    // выполняем поворот на 90 градусов
        break;
    }
    case 360:
    {
        // ничего не делаем
        break;
    }
    }
}

/* Зеркальное отражение относительно вертикальной оси (если DIR = 'V')
или горизонтальной оси (если (DIR='H') */
void flip(char dir, Spectr* channel)
{
    // отражение относительно вертикальной оси
    if (dir == 'V')
    {
        BYTE temp;
        DWORD height = channel->info.Height;
        DWORD width = channel->info.Width;
        // внутри каждой строки "отзеркаливаем" значения
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < (width / 2); j++)   // доходим до половины
            {
                // меняем местами два значения
                temp = channel->pixels[i][j];
                channel->pixels[i][j] = channel->pixels[i][width - 1 - j];
                channel->pixels[i][width - 1 - j] = temp;
            }
        }
    }
    // отражение относительно горизонтальной оси
    else if (dir == 'H')
    {
        BYTE* temp;
        DWORD height = channel->info.Height;
        DWORD width = channel->info.Width;
        // попарно меняем местами строки (первая станет последней и т.д.)
        for (int i = 0; i < (height / 2); i++)
        {
            temp = channel->pixels[i];
            channel->pixels[i] = channel->pixels[height - 1 - i];
            channel->pixels[height - 1 - i] = temp;
        }
    }
}

/* Увеличение количества пикселов по вертикали в V раз, по горизонтали в H раз.
Чтобы размерность изображений по всем спектральным каналам сохранялась одинаковой,
поле <Список спектральных каналов> может иметь только значение "00".
Команда выполняется только в том случае, если после ее применения размер изображения
не будет превышать предельных значений размеров изображения
по вертикали и горизонтали одновременно,
допустимыми в соответствии с форматом bmp-файла */
void upscale(int V, int H, Spectr* channel)
{
    DWORD height = channel->info.Height;
    DWORD width = channel->info.Width;
    // проверка допустимости изменений
    if ((height * V > H_max) || (width * H > W_max))
        return;

    // запись новых значений высоты, ширины и размеров файла
    channel->info.Height = height * V;
    channel->info.Width = width * H;
    channel->info.SizeImage = height * V * width * H;
    // заголовок файла, заголовок изображения, палитра
    channel->file.bmpSize = 14 + 40 + 1024 + channel->info.SizeImage;
    // растровая развертка
    DWORD padding = 4 - (channel->info.Width % 4);
    if (padding != 4)
        channel->file.bmpSize += (padding * channel->info.Height);

    // выделение памяти для нового массива
    BYTE** temp = (BYTE**)malloc(sizeof(BYTE*) * height * V);
    for (int i = 0; i < (height * V); i++)
        temp[i] = (BYTE*)malloc(width * H * sizeof(BYTE));

    // копирование
    for (int ii = 0, i = 0; i < height; ii += V, i++)
        for (int jj = 0, j = 0; j < width; jj += H, j++)
        {
            for (int k = 0; k < V; k++)
                for (int kk = 0; kk < H; kk++)
                    temp[ii + k][jj + kk] = channel->pixels[i][j];
        }

    // сохраняем в структуре ссылку на новый массив
    BYTE** old = channel->pixels;
    channel->pixels = temp;

    // освобождение ранее выделенной памяти
    // в освобождаемом массиве высота равна текущей ширине
    for (int i = 0; i < height; i++)
    {
        free(old[i]);
        old[i] = NULL;
    }
    free(old);
    old = NULL;
}

/* Уменьшение количества пикселов по вертикали в V раз, по горизонтали в H раз.
Чтобы размерность изображений по всем спектральным каналам сохранялась одинаковой,
поле <Список спектральных каналов> может иметь только значение "00".
Команда выполняется только в том случае, если значения V и H кратны текущим размерам
изображения ROWS и COLS по каждой модифицируемой цветовой компоненте одновременно.
В противном случае команда полностью игнорируется */
void downscale(int V, int H, Spectr* channel)
{
    DWORD height = channel->info.Height;
    DWORD width = channel->info.Width;
    // проверка кратности
    if ((height % V != 0) || (width % H != 0))
        return;

    // запись новых значений высоты, ширины и размеров файла
    channel->info.Height = height / V;
    channel->info.Width = width / H;
    channel->info.SizeImage = channel->info.Height * channel->info.Width;
    // заголовок файла, заголовок изображения, палитра
    channel->file.bmpSize = 14 + 40 + 1024 + channel->info.SizeImage;
    // растровая развертка
    DWORD padding = 4 - (channel->info.Width % 4);
    if (padding != 4)
        channel->file.bmpSize += (padding * channel->info.Height);

    // выделение памяти для нового массива
    BYTE** temp = (BYTE**)malloc(sizeof(BYTE*) * channel->info.Height);
    for (int i = 0; i < channel->info.Height; i++)
        temp[i] = (BYTE*)malloc(channel->info.Width * sizeof(BYTE));

    // копирование
    for (int i = 0, ii = 0; i < height; i += V, ii++)
        for (int j = 0, jj = 0; j < width; j += H, jj++)
            temp[ii][jj] = channel->pixels[i][j];

    // сохраняем в структуре ссылку на новый массив
    BYTE** old = channel->pixels;
    channel->pixels = temp;

    // освобождение ранее выделенной памяти
    // в освобождаемом массиве высота равна текущей ширине
    for (int i = 0; i < height; i++)
    {
        free(old[i]);
        old[i] = NULL;
    }
    free(old);
    old = NULL;
}

/* Изменение яркости пикселов компоненты путем сдвига ее значений
влево (при f>0) или вправо (при f<0) на f разрядов.
 */
void bright(int f, Spectr* channel)
{
    DWORD height = channel->info.Height;
    DWORD width = channel->info.Width;
    BYTE temp;
    if ((f > 0) && (f <= 8))
    {
        // поочередный побитовый сдвиг влево всех элементов массива
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
            f *= (-1);  // меняем знак
            // поочередный побитовый сдвиг вправо всех элементов массива
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
