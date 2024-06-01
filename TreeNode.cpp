#include <cassert>
#include <iostream>
#include "Judge.h"
#include "Point.h"
#include "TreeNode.hpp"
#include "SingleChoice.hpp"

bool TreeNode::terminal()
{
	if (x == -1 && y == -1) return false;
	if (userWin(x, y, M, N, board)) return true;
	if (machineWin(x, y, M, N, board)) return true;
	if (isTie(N, top)) return true;
	return false;
}

bool TreeNode::all_expanded()
{
	return exp_ch == ava_ch;
}

TreeNode *TreeNode::expand()
{
	for (int i = 0; i < N; i++)
	{
		if (top[i] > 0 && ch[i] == NULL)
		{
			int **new_board = new int *[M];
			for (int i = 0; i < M; i++)
			{
				new_board[i] = new int[N];
				for (int j = 0; j < N; j++)
				{
					new_board[i][j] = board[i][j];
				}
			}
			new_board[top[i] - 1][i] = self ? 2 : 1;
			int new_top[N];
			for (int j = 0; j < N; j++)
			{
				new_top[j] = top[j];
			}
			new_top[i]--;
			if (i == noY && new_top[i] - 1 == noX)
			{
				new_top[i]--;
			}
			ch[i] = new TreeNode(M, N, new_top, new_board, top[i] - 1, i, this, noX, noY, !self);
			
			exp_ch++;
			for (int i = 0; i < M; i++)
			{
				delete[] new_board[i];
			}
			delete[] new_board;
			return ch[i];
		}
	}
	assert(false);
}

double TreeNode::rollout()
{
	int **new_board = new int *[M];
	for (int i = 0; i < M; i++)
	{
		new_board[i] = new int[N];
		for (int j = 0; j < N; j++)
		{
			new_board[i][j] = board[i][j];
		}
	}
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++)
		{
			new_board[i][j] = board[i][j];
		}
	}
	int new_top[N];
	for (int i = 0; i < N; i++)
	{
		new_top[i] = top[i];
	}
	int new_x = x;
	int new_y = y;
	int new_self = self;
	double award = 0;
	int *ava = new int[N];
	int avas = 0;
	for (int i = 0; i < N; i++)
	{
		if (new_top[i] > 0)
		{
			ava[avas] = i;
			avas++;
		}
	}
	while (true)
	{
		if (userWin(new_x, new_y, M, N, new_board))
		{
			award = self ? 0 : 1;
			break;
		}
		if (machineWin(new_x, new_y, M, N, new_board))
		{
			award = self ? 1 : 0;
			break;
		}
		// if (isTie(N, new_top))
		if (avas == 0)
		{
			assert(isTie(N, new_top));
			award = 0.5; // maybe 0.5
			break;
		}
		
		int new_i = rand() % avas;
		new_y = ava[new_i];
		new_x = new_top[new_y] - 1;
		new_board[new_x][new_y] = new_self ? 2 : 1;
		new_top[new_y]--;
		if (new_y == noY && new_top[new_y] - 1 == noX)
		{
			new_top[new_y]--;
		}
		new_self = !new_self;
		if (new_top[new_y] == 0)
		{
			std::swap(ava[new_i], ava[avas - 1]);
			avas--;
		}
	}
	
	for (int i = 0; i < M; i++)
	{
		delete[] new_board[i];
	}
	delete[] new_board;
	delete[] ava;
	return award;
}

void TreeNode::print()
{
	std::cerr << this << " " << fa << " " << x << " " << y << " " << win << " " << tot << " " << self << " " << win / tot << std::endl;
	if (fa == NULL)
	{
		for (int i = 0; i < N; i++)
		{
			if (ch[i] != NULL)
			{
				ch[i]->print();
			}
		}
	}
}
