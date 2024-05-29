#ifndef SingleChoice_hpp
#define SingleChoice_hpp

#include <stdio.h>
#include <Point.h>
#include <Judge.h>

#endif

void release(int M, int N, int **board, int *top);
Point* SingleChoice(int M, int N, int *const *board, const int *top, int self);
