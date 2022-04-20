#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <vector>
#include<queue>
#include <utility>//pair����
using namespace std;

//��Ϸ���ͣ�˫�˻���AI���̶���AI�º��ӣ�
enum GameType
{
    PERSON,
    BOT,
    BOT2
};

//��Ϸ״̬
enum GameStatus
{
    PLAYING,
    WIN,
    DEAD
};

//�Ʒַ���
enum GameScore
{
    HORIZONTAL,
    VERTICAL,
    DOWNLEFT_UPRIGHT,
    DOWNRIGHT_UPLEFT
};

//���̳ߴ�
const int kBoardSizeNum = 15;

class GameModel
{
public:
    friend class MainWindown;
    GameModel();
    struct Piece{//�Ʒ�ʱ��ǰ��һ��
      int lastDot;
      int curDot;
      int curLength;//��ǰ���ӳ��ȣ��Ҳ�Ϊ���
      Piece(int last, int cur, int len)
          : lastDot(last), curDot(cur), curLength(len)
      {}
    };
    void startGame(GameType type); //��ʼ��Ϸ
    int calculateBoardScore(int dotType = -1); //�������֣�dotType=1������Person���Ʒ�ֵ�����أ���dotType=-1������BOT���Ʒ�ֵ��������Ĭ��ֵ��
    int calculateLineScore(vector<Piece> temp, int dotType2Cal);//���㵥������
    int maxValue(pair<int,int> nextDot, int player, int alph, int beta, int curDepth);//curDepth����������չ�����
    int minValue(pair<int,int> nextDot, int player, int alph, int beta, int curDepth);//curDepth����������չ�����
    bool checkFourinLine(int row, int col);//��(row,col)������Ƿ������ģ������˫�ࣩ
    bool checkFourinLine2(int row, int col);//��(row,col)������Ƿ������ģ�˫�ࣩ
    void actionByPerson(int row, int col); //��ִ������
    pair<int,int> actionByAI(); //����ִ�����壬����ֵ���Կ����ж���Ӯ
    pair<int,int> actionByAI2(); //����ִ�����壬����ֵ���Կ����ж���Ӯ
    void updateGameMap(int row, int col); //ÿ�����Ӻ������Ϸ����
    bool isWin(int row, int col); //�ж���Ϸ�Ƿ�ʤ��
    bool isDeadGame(); //�ж��Ƿ����

    bool playerFlag; //��ʾ���巽
    GameType gameType; //��Ϸģʽ
    GameStatus gameStatus; //��Ϸ״̬(�Ƿ���У����ڵ���ʤ��/���ֶԻ���
    std::vector<std::vector<int>> gameMapVec; //�洢��ǰ��Ϸ���̺����ӵ����,�հ�Ϊ0������1������-1

    struct state
    {
        double totalScore;//��ǰ״̬����
        pair<int, int> dot;//(row, col)
    };

    struct CompareScore {//��ֵ����ǰ
        bool operator()(state const& s1, state const& s2)
        {
            return s1.totalScore < s2.totalScore;
        }
    };

//    priority_queue<state, std::vector<state>, CompareScore> score;//����ѡ�����

};

#endif // GAMEMODEL_H
