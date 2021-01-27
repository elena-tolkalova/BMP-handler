#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include "main.h"

int main()
{
    FILE* com_file = NULL;
    /* Данное условие не выполнится, если файл не будет открыт по каким-то причинам
       (например, исходный файл отсутствует)
       В таком случае выполнение программы завершится с кодом 0                 */
    if (com_file = fopen("command.txt", "r"))
    {
        int C = 0; // количество файлов
        int N = 0; // количество команд

        int c1 = fscanf(com_file, "%d", &C);
        int n1 = fscanf(com_file, "%d", &N);

        printf("%d %d\n", C, N);
        // проверка корректности ввода
        // при недопустимых значениях C и N выполнение программы завершится с кодом -1
        if ((C < 1) || (C >= N_chan) || (N < 0) || (N > N_com))
            return -1;

        /* т.к. количество файлов заранее неизвестно, для хранения их содержимого
           будет использована динамическая память.                                   */
        Spectr* channel_list = (Spectr*)malloc(sizeof(Spectr) * C);

        // открытие исходных файлов изображения и запись их содержимого в оперативную память
        // если не удалось открыть файл, завершаем выполнение программы
        for (int i = 1; i <= C; i++)
            if (channel_input(i, &channel_list[i - 1]) == -1)
                return -1;

        /* выполнение указанного количества команд */
        for (int i = 0; i < N; i++)
            make_command(com_file, C, channel_list);
        fclose(com_file);

        // вывод полученных файлов изображения
        for (int i = 1; i <= C; i++)
            channel_output(i, &channel_list[i - 1]);

        // освобождение памяти, выделенной для хранения спектральных каналов
        free(channel_list);
    }

    return 0;
}

/*  Функция для парсинга строки следующего формата:
    <Список_спектральных_каналов> : <Команда> : <Параметры_команды> 
    Все отделенные части строки записываются в соответствующие массивы.   */
void line_parse(FILE *com_file, int* channels, char* command)
{
    // отделение номеров спектральных каналов 
    channels_parse(com_file, channels);

    fgetc(com_file);    // считывание знака ":"
    fgetc(com_file);    // считывание пробела

    // Отделение слова, обозначающего команду 
    command_parse(com_file, command);

    fgetc(com_file);    // считывание знака ":"
    fgetc(com_file);    // считывание пробела
}

/* Функция для отделения списка спектральных каналов. 
   Список хранится в виде целочисленного массива. Если канал входит в список, 
   то элемент с номером, соответствующим номеру канала, равен 1, противном случае - 0. */
void channels_parse(FILE* com_file, int* channels)
{
    int n = 0;
    char symb1, symb2;
    symb1 = fgetc(com_file);   //считывание символа конца строки
    do
    {
        symb1 = fgetc(com_file);
        symb2 = fgetc(com_file);
        n = (symb1 - '0') * 10;
        n += (symb2 - '0');
        if ((n < 10) && (n >= 0))   // защита от выхода за пределы массива
            channels[n] = 1;
        symb1 = fgetc(com_file);
    } while (symb1 != ' ');         // пока не встретится пробел
}

/* Функция для отделения названия выполняемой команды.
   Название хранится в виде строки (символьного массива с нуль-терминатором в конце)   */
void command_parse(FILE* com_file, char* command)
{
    int i = 0;
    char symb = fgetc(com_file);    // считывание первого символа
    while (symb != ' ')             // пока не считаем пробел
    {
        command[i] = symb;
        i++;
        symb = fgetc(com_file);
    }

    command[i] = '\0';  // установка символа конца строки
}

/* Определение кода введенной команды */
int command_detect(char* command)
{
    if (strcmp(command, "rotate") == 0)
        return 1;
    if (strcmp(command, "flip") == 0)
        return 2;
    if (strcmp(command, "upscale") == 0)
        return 3;
    if (strcmp(command, "downscale") == 0)
        return 4;
    if (strcmp(command, "bright") == 0)
        return 5;
    return 0;       // если ни одна команда не подойдет
}

/* Функция, которая организует считывание команд и их параметров из файла, 
   а также вызывает соответствующую функцию из библиотеки с заданными аргументами  */
void make_command(FILE* com_file, int C, Spectr *chan_list)
{
    int err = 0;
    int channels[N_chan] = { 0 };    // массив, содержащий номера спектральных каналов
    char command[15] = { 0 };        // строка, содержащая введенную команду

    line_parse(com_file, channels, command);    // обработка строки

    /* Определение кода введенной команды.
       Определение параметров команды и вызов соответствующей функции на основе полученного кода. */
    int com = command_detect(command);
    switch (com)
    {
    case 1:
    {
        int ang = 0;
        err = fscanf(com_file, "%d", &ang);
        if (channels[0] == 1)               // применяется только ко всем каналам
        {
            for (int i = 0; i < C; i++)
                rotate(ang, &chan_list[i]);
        }
        break;
    }
    case 2:
    {
        char dir = fgetc(com_file);
        if (channels[0] == 1)               // ко всем каналам
        {
            for (int i = 0; i < C; i++)
                flip(dir, &chan_list[i]);
        }
        else                                // к перечисленным
            for (int i = 1; i <= C; i++)
            {
                if (channels[i] == 1)
                    flip(dir, &chan_list[i-1]);
            }
        break;
    }
    case 3:
    {
        int V = 0, H = 0;
        err = fscanf(com_file, "%d %d", &V, &H);
        if (channels[0] == 1)               // применяется только ко всем каналам
        {
            for (int i = 0; i < C; i++)
                upscale(V, H, &chan_list[i]);
        }
        break;
    }
    case 4:
    {
        int V = 0, H = 0;
        err = fscanf(com_file, "%d %d", &V, &H);
        if (channels[0] == 1)               // применяется только ко всем каналам
        {
            for (int i = 0; i < C; i++)
                downscale(V, H, &chan_list[i]);
        }
        break;
    }
    case 5:
    {
        int f = 0;
        err = fscanf(com_file, "%d", &f);
        if (channels[0] == 1)               // ко всем каналам
        {
            for (int i = 0; i < C; i++)
                bright(f, &chan_list[i]);
        }
        else
            for (int i = 1; i <= C; i++)
            {
                if (channels[i] == 1)       // к перечисленным
                    bright(f, &chan_list[i - 1]);
            }
        break;
    }
    default:
        printf("Unknown command\n");
    }
}