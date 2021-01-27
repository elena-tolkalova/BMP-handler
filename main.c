#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include "main.h"

int main()
{
    FILE* com_file = NULL;
    /* ������ ������� �� ����������, ���� ���� �� ����� ������ �� �����-�� ��������
       (��������, �������� ���� �����������)
       � ����� ������ ���������� ��������� ���������� � ����� 0                 */
    if (com_file = fopen("command.txt", "r"))
    {
        int C = 0; // ���������� ������
        int N = 0; // ���������� ������

        int c1 = fscanf(com_file, "%d", &C);
        int n1 = fscanf(com_file, "%d", &N);

        printf("%d %d\n", C, N);
        // �������� ������������ �����
        // ��� ������������ ��������� C � N ���������� ��������� ���������� � ����� -1
        if ((C < 1) || (C >= N_chan) || (N < 0) || (N > N_com))
            return -1;

        /* �.�. ���������� ������ ������� ����������, ��� �������� �� �����������
           ����� ������������ ������������ ������.                                   */
        Spectr* channel_list = (Spectr*)malloc(sizeof(Spectr) * C);

        // �������� �������� ������ ����������� � ������ �� ����������� � ����������� ������
        // ���� �� ������� ������� ����, ��������� ���������� ���������
        for (int i = 1; i <= C; i++)
            if (channel_input(i, &channel_list[i - 1]) == -1)
                return -1;

        /* ���������� ���������� ���������� ������ */
        for (int i = 0; i < N; i++)
            make_command(com_file, C, channel_list);
        fclose(com_file);

        // ����� ���������� ������ �����������
        for (int i = 1; i <= C; i++)
            channel_output(i, &channel_list[i - 1]);

        // ������������ ������, ���������� ��� �������� ������������ �������
        free(channel_list);
    }

    return 0;
}

/*  ������� ��� �������� ������ ���������� �������:
    <������_������������_�������> : <�������> : <���������_�������> 
    ��� ���������� ����� ������ ������������ � ��������������� �������.   */
void line_parse(FILE *com_file, int* channels, char* command)
{
    // ��������� ������� ������������ ������� 
    channels_parse(com_file, channels);

    fgetc(com_file);    // ���������� ����� ":"
    fgetc(com_file);    // ���������� �������

    // ��������� �����, ������������� ������� 
    command_parse(com_file, command);

    fgetc(com_file);    // ���������� ����� ":"
    fgetc(com_file);    // ���������� �������
}

/* ������� ��� ��������� ������ ������������ �������. 
   ������ �������� � ���� �������������� �������. ���� ����� ������ � ������, 
   �� ������� � �������, ��������������� ������ ������, ����� 1, ��������� ������ - 0. */
void channels_parse(FILE* com_file, int* channels)
{
    int n = 0;
    char symb1, symb2;
    symb1 = fgetc(com_file);   //���������� ������� ����� ������
    do
    {
        symb1 = fgetc(com_file);
        symb2 = fgetc(com_file);
        n = (symb1 - '0') * 10;
        n += (symb2 - '0');
        if ((n < 10) && (n >= 0))   // ������ �� ������ �� ������� �������
            channels[n] = 1;
        symb1 = fgetc(com_file);
    } while (symb1 != ' ');         // ���� �� ���������� ������
}

/* ������� ��� ��������� �������� ����������� �������.
   �������� �������� � ���� ������ (����������� ������� � ����-������������ � �����)   */
void command_parse(FILE* com_file, char* command)
{
    int i = 0;
    char symb = fgetc(com_file);    // ���������� ������� �������
    while (symb != ' ')             // ���� �� ������� ������
    {
        command[i] = symb;
        i++;
        symb = fgetc(com_file);
    }

    command[i] = '\0';  // ��������� ������� ����� ������
}

/* ����������� ���� ��������� ������� */
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
    return 0;       // ���� �� ���� ������� �� ��������
}

/* �������, ������� ���������� ���������� ������ � �� ���������� �� �����, 
   � ����� �������� ��������������� ������� �� ���������� � ��������� �����������  */
void make_command(FILE* com_file, int C, Spectr *chan_list)
{
    int err = 0;
    int channels[N_chan] = { 0 };    // ������, ���������� ������ ������������ �������
    char command[15] = { 0 };        // ������, ���������� ��������� �������

    line_parse(com_file, channels, command);    // ��������� ������

    /* ����������� ���� ��������� �������.
       ����������� ���������� ������� � ����� ��������������� ������� �� ������ ����������� ����. */
    int com = command_detect(command);
    switch (com)
    {
    case 1:
    {
        int ang = 0;
        err = fscanf(com_file, "%d", &ang);
        if (channels[0] == 1)               // ����������� ������ �� ���� �������
        {
            for (int i = 0; i < C; i++)
                rotate(ang, &chan_list[i]);
        }
        break;
    }
    case 2:
    {
        char dir = fgetc(com_file);
        if (channels[0] == 1)               // �� ���� �������
        {
            for (int i = 0; i < C; i++)
                flip(dir, &chan_list[i]);
        }
        else                                // � �������������
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
        if (channels[0] == 1)               // ����������� ������ �� ���� �������
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
        if (channels[0] == 1)               // ����������� ������ �� ���� �������
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
        if (channels[0] == 1)               // �� ���� �������
        {
            for (int i = 0; i < C; i++)
                bright(f, &chan_list[i]);
        }
        else
            for (int i = 1; i <= C; i++)
            {
                if (channels[i] == 1)       // � �������������
                    bright(f, &chan_list[i - 1]);
            }
        break;
    }
    default:
        printf("Unknown command\n");
    }
}