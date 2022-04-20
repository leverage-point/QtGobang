#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <vector>
#include<queue>
#include <utility>//pair类型
using namespace std;

//游戏类型，双人还是AI（固定让AI下黑子）
enum GameType
{
    PERSON,
    BOT,
    BOT2
};

//游戏状态
enum GameStatus
{
    PLAYING,
    WIN,
    DEAD
};

//计分方向
enum GameScore
{
    HORIZONTAL,
    VERTICAL,
    DOWNLEFT_UPRIGHT,
    DOWNRIGHT_UPLEFT
};

//棋盘尺寸
const int kBoardSizeNum = 15;

class GameModel
{
public:
    friend class MainWindown;
    GameModel();
    struct Piece{//计分时往前看一步
      int lastDot;
      int curDot;
      int curLength;//当前连子长度，右侧为最大
      Piece(int last, int cur, int len)
          : lastDot(last), curDot(cur), curLength(len)
      {}
    };
    void startGame(GameType type); //开始游戏
    int calculateBoardScore(int dotType = -1); //计算评分，dotType=1，计算Person优势分值（防守）；dotType=-1，计算BOT优势分值（进攻，默认值）
    int calculateLineScore(vector<Piece> temp, int dotType2Cal);//计算单行评分
    int maxValue(pair<int,int> nextDot, int player, int alph, int beta, int curDepth);//curDepth用来限制扩展的深度
    int minValue(pair<int,int> nextDot, int player, int alph, int beta, int curDepth);//curDepth用来限制扩展的深度
    bool checkFourinLine(int row, int col);//从(row,col)处检查是否连活四（单侧或双侧）
    bool checkFourinLine2(int row, int col);//从(row,col)处检查是否连活四（双侧）
    void actionByPerson(int row, int col); //人执行下棋
    pair<int,int> actionByAI(); //机器执行下棋，返回值可以快速判定输赢
    pair<int,int> actionByAI2(); //机器执行下棋，返回值可以快速判定输赢
    void updateGameMap(int row, int col); //每次落子后更新游戏棋盘
    bool isWin(int row, int col); //判断游戏是否胜利
    bool isDeadGame(); //判断是否和棋

    bool playerFlag; //标示下棋方
    GameType gameType; //游戏模式
    GameStatus gameStatus; //游戏状态(是否进行，用于弹出胜利/死局对话框）
    std::vector<std::vector<int>> gameMapVec; //存储当前游戏棋盘和棋子的情况,空白为0，白子1，黑子-1

    struct state
    {
        double totalScore;//当前状态评分
        pair<int, int> dot;//(row, col)
    };

    struct CompareScore {//分值高排前
        bool operator()(state const& s1, state const& s2)
        {
            return s1.totalScore < s2.totalScore;
        }
    };

//    priority_queue<state, std::vector<state>, CompareScore> score;//最优选择队列

};

#endif // GAMEMODEL_H
