#ifndef TreeNode_hpp
#define TreeNode_hpp

#include <stdio.h>

#endif

struct TreeNode
{
	int M, N;
	double win;
	int tot;
	bool self;
	int **board;
	int *top;
	int x, y;
	int ava_ch, exp_ch;
	int noX, noY;
	TreeNode *fa;
	TreeNode **ch;

	bool terminal();
	bool all_expanded();
	TreeNode *expand();
	double rollout();
	void print();

	TreeNode(int M, int N, const int *top, int **board, int x, int y, TreeNode *fa, int noX, int noY, bool self)
	{
		this->M = M;
		this->N = N;
		this->win = 0;
		this->tot = 0;
		this->self = self;
		this->board = new int *[M];
		for (int i = 0; i < M; i++)
		{
			this->board[i] = new int[N];
			for (int j = 0; j < N; j++)
			{
				this->board[i][j] = board[i][j];
			}
		}
		this->top = new int[N];
		for (int i = 0; i < N; i++)
		{
			this->top[i] = top[i];
		}
		this->x = x;
		this->y = y;
		this->fa = fa;
		this->ch = new TreeNode *[N];
		for (int i = 0; i < N; i++)
		{
			ch[i] = NULL;
		}
		this->noX = noX;
		this->noY = noY;
		this->ava_ch = 0;
		for (int i = 0; i < N; i++)
		{
			if (top[i] > 0)
			{
				this->ava_ch++;
			}
		}
		this->exp_ch = 0;
	}

	~TreeNode()
	{
		for (int i = 0; i < M; i++)
		{
			delete[] board[i];
		}
		delete[] board;
		delete[] top;
		if (ch != NULL)
		{
			for (int i = 0; i < N; i++)
			{
				delete ch[i];
			}
			delete[] ch;
		}
	}
};
