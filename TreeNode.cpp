#include <cassert>
#include <iostream>
#include "TreeNode.hpp"
#include "Judge.h"

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
	/*printf("%d %d %d\n", self, ava_ch, tot);
	for (int i = 0; i < N; i++)
	{
		printf("%d ", top[i]);
	}
	printf("\n");*/
	for (int i = 0; i < N; i++)
	{
		if (top[i] > 0 && ch[i] == NULL)
		{
			// std::cerr << "EX " << i << std::endl;
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
			// std::cerr << "READY TO NEW" << std::endl;
			ch[i] = new TreeNode(M, N, new_top, new_board, top[i] - 1, i, this, noX, noY, !self);
			// std::cerr << "NEWED" << std::endl;
			
			exp_ch++;
			for (int i = 0; i < M; i++)
			{
				delete[] new_board[i];
			}
			delete[] new_board;
			return ch[i];
		}
	}
	// std::cerr << "EXPAND" << exp_ch << " " << ava_ch << std::endl;
	assert(false);
}

int TreeNode::rollout()
{
	// printf("ROLLBEG\n");
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
	int award = 0;
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
		if (isTie(N, new_top))
		{
			award = 0; // maybe 0.5
			break;
		}
		int new_ava_ch = 0;
		for (int i = 0; i < N; i++)
		{
			// printf("%d ", new_top[i]);
			if (new_top[i] > 0)
			{
				new_ava_ch++;
			}
		}
		// printf("\n");
		int new_i = rand() % new_ava_ch;
		for (int i = 0; i < N; i++)
		{
			if (new_top[i] > 0)
			{
				if (new_i == 0)
				{
					new_x = new_top[i] - 1;
					new_y = i;
					new_board[new_top[i] - 1][i] = new_self ? 2 : 1;
					new_top[i]--;
					if (i == noY && new_top[i] - 1 == noX)
					{
						new_top[i]--;
					}
					new_self = !new_self;
					break;
				}
				new_i--;
			}
		}
	}
	
	// printf("ROLLEND\n");
	for (int i = 0; i < M; i++)
	{
		delete[] new_board[i];
	}
	delete[] new_board;
	return award;
}

void TreeNode::print()
{
	// printf("%d %d %d %d %d %d %d\n", this, fa, x, y, win, tot, self);
	std::cerr << this << " " << fa << " " << x << " " << y << " " << win << " " << tot << " " << self << std::endl;
	if (fa == NULL)
	for (int i = 0; i < N; i++)
	{
		if (ch[i] != NULL)
		{
			ch[i]->print();
		}
	}
}
