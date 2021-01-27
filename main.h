#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "multispec.h"

void make_command(FILE* com_file, int C, Spectr *chan_list);	// считывание команд и вызов функций
void line_parse(FILE* com_file, int* channels, char* command);	// разбиение строки командного файла
void channels_parse(FILE* com_file, int* channels);				// выделение списка спектральных каналов
void command_parse(FILE* com_file, char* command);				// выделение названия команды
int command_detect(char* command);								// определение кода команды


#endif // MAIN_H_INCLUDEDs
