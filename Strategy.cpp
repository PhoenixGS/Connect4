#include <iostream>
#include <cassert>
#include <cmath>
#include <unistd.h>
#include "Point.h"
#include "Strategy.h"
#include "Judge.h"

const int C = 1;
const int MAX_ITER = 100000;

using namespace std;

Point *UctSearch(int M, int N, const int *top, int **board, int lastX, int lastY, int noX, int noY);

/*
	策略函数接口,该函数被对抗平台调用,每次传入当前状态,要求输出你的落子点,该落子点必须是一个符合游戏规则的落子点,不然对抗平台会直接认为你的程序有误
	
	input:
		为了防止对对抗平台维护的数据造成更改，所有传入的参数均为const属性
		M, N : 棋盘大小 M - 行数 N - 列数 均从0开始计， 左上角为坐标原点，行用x标记，列用y标记
		top : 当前棋盘每一列列顶的实际位置. e.g. 第i列为空,则_top[i] == M, 第i列已满,则_top[i] == 0
		_board : 棋盘的一维数组表示, 为了方便使用，在该函数刚开始处，我们已经将其转化为了二维数组board
				你只需直接使用board即可，左上角为坐标原点，数组从[0][0]开始计(不是[1][1])
				board[x][y]表示第x行、第y列的点(从0开始计)
				board[x][y] == 0/1/2 分别对应(x,y)处 无落子/有用户的子/有程序的子,不可落子点处的值也为0
		lastX, lastY : 对方上一次落子的位置, 你可能不需要该参数，也可能需要的不仅仅是对方一步的
				落子位置，这时你可以在自己的程序中记录对方连续多步的落子位置，这完全取决于你自己的策略
		noX, noY : 棋盘上的不可落子点(注:涫嫡饫锔?龅膖op已经替你处理了不可落子点，也就是说如果某一步
				所落的子的上面恰是不可落子点，那么UI工程中的代码就已经将该列的top值又进行了一次减一操作，
				所以在你的代码中也可以根本不使用noX和noY这两个参数，完全认为top数组就是当前每列的顶部即可,
				当然如果你想使用lastX,lastY参数，有可能就要同时考虑noX和noY了)
		以上参数实际上包含了当前状态(M N _top _board)以及历史信息(lastX lastY),你要做的就是在这些信息下给出尽可能明智的落子点
	output:
		你的落子点Point
*/
extern "C" Point *getPoint(const int M, const int N, const int *top, const int *_board,
						   const int lastX, const int lastY, const int noX, const int noY)
{
	/*
		不要更改这段代码
	*/
	int x = -1, y = -1; //最终将你的落子点存到x,y中
	int **board = new int *[M];
	for (int i = 0; i < M; i++)
	{
		board[i] = new int[N];
		for (int j = 0; j < N; j++)
		{
			board[i][j] = _board[i * N + j];
		}
	}

	/*
		根据你自己的策略来返回落子点,也就是根据你的策略完成对x,y的赋值
		该部分对参数使用没有限制，为了方便实现，你可以定义自己新的类、.h文件、.cpp文件
	*/
	//Add your own code below

	//a naive example
	/*for (int i = N-1; i >= 0; i--) {
		if (top[i] > 0) {
			x = top[i] - 1;
			y = i;
			break;
		}
	}*/
	
	Point* pt = UctSearch(M, N, top, board, lastX, lastY, noX, noY);
	x = pt->x;
	y = pt->y;
	// printf("ME %d %d\n", x, y);

	/*
		不要更改这段代码
	*/
	clearArray(M, N, board);
	return new Point(x, y);
}

/*
	getPoint函数返回的Point指针是在本so模块中声明的，为避免产生堆错误，应在外部调用本so中的
	函数来释放空间，而不应该在外部直接delete
*/
extern "C" void clearPoint(Point *p)
{
	delete p;
	return;
}

/*
	清除top和board数组
*/
void clearArray(int M, int N, int **board)
{
	for (int i = 0; i < M; i++)
	{
		delete[] board[i];
	}
	delete[] board;
}

/*
	添加你自己的辅助函数，你可以声明自己的类、函数，添加新的.h .cpp文件来辅助实现你的想法
*/

struct TreeNode
{
	int M, N;
	int win, tot;
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
	int rollout();

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

bool TreeNode::terminal()
{
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
	std::cerr << "EXPAND" << exp_ch << " " << ava_ch << std::endl;
	assert(false);
}

TreeNode *select(TreeNode *u)
{
	while (! (u->terminal()))
	{
		if (u->all_expanded())
		{
			TreeNode *v = NULL;
			double max_ucb = -1;
			for (int i = 0; i < u->N; i++)
			{
				if (u->ch[i] != NULL)
				{
					double ucb = (double)u->ch[i]->win / u->ch[i]->tot + C * sqrt(2 * log(u->tot) / u->ch[i]->tot);
					if (ucb > max_ucb)
					{
						max_ucb = ucb;
						v = u->ch[i];
					}
				}
			}
			u = v;
		}
		else
		{
			/* std::cerr << "EXPAND" << u->exp_ch << " " << u->ava_ch << std::endl;
			for (int i = 0; i < u->N; i++)
			{
				std::cerr << u->ch[i] << " ";
			}
			std::cerr << std::endl;*/
			return u->expand();
		}
	}
	return u;
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

Point *UctSearch(int M, int N, const int *top, int **board, int lastX, int lastY, int noX, int noY)
{
	TreeNode *root = new TreeNode(M, N, top, board, lastX, lastY, NULL, noX, noY, true);
	// printf("NEW SUCCESS");

	int T = 0;
	while (T < MAX_ITER)
	{
		TreeNode *v = select(root);
		// printf("begin roll\n");
		int reward = v->rollout();
		// printf("end roll\n");
		while (v != NULL)
		{
			v->tot++;
			v->win += reward;
			v = v->fa;
		}
		T++;
	}
	
	// printf("########\n");
	TreeNode *best = NULL;
	double max_rate = -1;
	for (int i = 0; i < root->N; i++)
	{
		if (root->ch[i] != NULL)
		{
			double rate = (double)root->ch[i]->win / root->ch[i]->tot;
			if (rate > max_rate)
			{
				max_rate = rate;
				best = root->ch[i];
			}
			// printf("%d %d %d %d\n", root->ch[i]->x, root->ch[i]->y, root->ch[i]->win, root->ch[i]->tot);
			// std::cerr << root->ch[i]->x << " " << root->ch[i]->y << " " << root->ch[i]->win << " " << root->ch[i]->tot << " " << rate << std::endl;
		}
	}
	
	assert(best != NULL);
	int x = best->x;
	int y = best->y;
	delete root;
	return new Point(x, y);
}

