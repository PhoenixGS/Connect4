#include "SingleChoice.hpp"

void release(int M, int N, int **board, int *top)
{
	for (int i = 0; i < M; i++)
	{
		delete[] board[i];
	}
	delete[] board;
	delete[] top;
}

Point* SingleChoice(int M, int N, int *const *_board, const int *_top, int self)
{
	int **board = new int *[M];
	for (int i = 0; i < M; i++)
	{
		board[i] = new int[N];
		for (int j = 0; j < N; j++)
		{
			board[i][j] = _board[i][j];
		}
	}
	int *top = new int[N];
	for (int i = 0; i < N; i++)
	{
		top[i] = _top[i];
	}
	Point *pt = new Point{-1, -1};
	
	// to win
	for (int i = 0; i < N; i++)
	{
		if (top[i] > 0)
		{
			board[top[i] - 1][i] = self ? 2 : 1;
			if ((self && machineWin(top[i] - 1, i, M, N, board)) || (! self && userWin(top[i] - 1, i, M, N, board)))
			{
				pt->x = top[i] - 1;
				pt->y = i;
				release(M, N, board, top);
				return pt;
			}
			board[top[i] - 1][i] = 0;
		}
	}
	
	// to not lose
	for (int i = 0; i < N; i++)
	{
		if (top[i] > 0)
		{
			board[top[i] - 1][i] = self ? 1 : 2;
			if ((self && userWin(top[i] - 1, i, M, N, board)) || (! self && machineWin(top[i] - 1, i, M, N, board)))
			{
				pt->x = top[i] - 1;
				pt->y = i;
				release(M, N, board, top);
				return pt;
			}
			board[top[i] - 1][i] = 0;
		}
	}
	
	// no
	return pt;
}
