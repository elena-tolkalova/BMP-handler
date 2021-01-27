#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "multispec.h"

void make_command(FILE* com_file, int C, Spectr *chan_list);	// ���������� ������ � ����� �������
void line_parse(FILE* com_file, int* channels, char* command);	// ��������� ������ ���������� �����
void channels_parse(FILE* com_file, int* channels);				// ��������� ������ ������������ �������
void command_parse(FILE* com_file, char* command);				// ��������� �������� �������
int command_detect(char* command);								// ����������� ���� �������


#endif // MAIN_H_INCLUDEDs
