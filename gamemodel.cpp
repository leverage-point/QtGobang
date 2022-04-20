#include <utility>//pair����
#include <time.h>//�����
#include<stdlib.h>//�����
#include <iostream>//std::cout
#include <algorithm>//std::find
#include <vector>//std::vector
#include<queue>
#include "GameModel.h"
#include<QDebug>
using namespace std;

const int Infi = 100000;
const int Black = -1;
const int White = 1;


GameModel::GameModel()
{}


void GameModel::startGame(GameType type)
{
    gameType = type;
    //��ʼ����
    gameMapVec.clear();
    for (int i = 0; i < kBoardSizeNum; i++)
    {
        std::vector<int> lineBoard;
        for (int j = 0; j < kBoardSizeNum; j++)
            lineBoard.push_back(0);
        gameMapVec.push_back(lineBoard);
    }
    //������Ϊtrue,�Է���Ϊfalse���˴�����Ϊ��������
    playerFlag = true;
}


void GameModel::updateGameMap(int row, int col)
{
    if (playerFlag)
        gameMapVec[row][col] = 1;//���������
    else
        gameMapVec[row][col] = -1;//�Է������

    //����
    playerFlag = !playerFlag;
}


void GameModel::actionByPerson(int row, int col)
{
    updateGameMap(row, col);
}


pair<int, int> GameModel::actionByAI()//��չ�ڵ㣨�ֲ������㷨/̰���㷨��//̽��һ��
{
    //naive�汾��ֱ�Ӳ���������������
//    while(true){
//        srand((unsigned)time(NULL));
//        int clickRow, clickCol;
//        clickRow = rand()%kBoardSizeNum; //��¼���ӵ�
//        clickCol = rand()%kBoardSizeNum;
//        if(clickCol!=0&&clickRow!=0){
//            updateGameMap(clickRow, clickCol);
//            pair<int,int> tempReturn(clickRow,clickCol);
//            return tempReturn;
//            break;
//        }
//    }

    //��ǰ̽��һ���汾
    priority_queue<state, vector<state>, CompareScore> scoreQue;//����ѡ�����
    for (int row = 1; row < kBoardSizeNum; row++){
        for (int col = 1; col < kBoardSizeNum; col++)
        {
            //̽���ǿյ��ܱߵĵ㣬��ȫ��ɨ��
            if (gameMapVec[row][col]!=0)
            {
                //������Χ�˸�����
                for (int y = -1; y <= 1; y++){//���£���row���仯
                    //qDebug()<<"before forth loop, the index of third loop: "<<y;
                    for (int x = -1; x <= 1; x++)//���ң���col���仯
                    {//��9�ַ�����ϣ�ԭ���겻�㣬����8�ַ������
                        //(row+y>0 && row+y<kBoardSizeNum && col+x>0 && col+x<kBoardSizeNum)?qDebug()<<"ok to loop":qDebug()<<"cannot loop";
                        if(row+y>0 && row+y<kBoardSizeNum && col+x>0 && col+x<kBoardSizeNum){
                            if ((!(y == 0 && x == 0)) && gameMapVec[row+y][col+x]==0)
                            {   //��ʼ̽��
                                qDebug()<<"Before score: Dot("<<row+y<<","<<col+x<<"):"<<gameMapVec[row+y][col+x];
                                qDebug()<<"-------------start expend----------------";
                                gameMapVec[row+y][col+x] = -1;//�ڵ�ǰ���״̬�½�����չ���º���Ԥ�⣩
                                state tempState;
                                tempState.dot = make_pair(row+y, col+x);
                                tempState.totalScore = double(calculateBoardScore(Black)) / double(calculateBoardScore(White))*1.75;//����Ȩ��
                                scoreQue.push(tempState);
                                qDebug()<<"The highest score: "<<scoreQue.top().totalScore;
                                gameMapVec[row+y][col+x] = 0;//�������֮�󣬻ָ����״̬
                            }
                            else break;
                        }
                    }
                    //qDebug()<<"***************forth Loop finish*****************  "<<"the index of third loop: "<<y;
                }
                //qDebug()<<"***************third Loop finish*****************";
            }
        }
    }
    //ѡ�����Žڵ�����
    qDebug()<<"***************Loop finish*****************";
    state decideState = scoreQue.top();
    priority_queue<state, vector<state>, CompareScore> doClearQue;
    swap(doClearQue, scoreQue);
    updateGameMap(decideState.dot.first, decideState.dot.second);
    return decideState.dot;
}


pair<int, int> GameModel::actionByAI2()//minMax
{
    //MinMax�㷨�汾
    //Ŀǰ�Ѿ����Լ����ÿ��״̬�ĵ÷֣�����ͨ����֦��չ��BOT(MAXscore)->Person(MINscore)->BOT(MAXscore)->Person(MINscore)->����
    qDebug()<<"-------------next judge step----------------";
    int player = Black;//��ʾ����
    int max = -Infi;
    pair<int,int> maxScorePoint(-1, -1);//��ʼ��
    for (int row = 1; row < kBoardSizeNum; row++)
    {
        for (int col = 1; col < kBoardSizeNum; col++)
        {
            //̽���ǿյ��ܱߵĵ㣬��ȫ��ɨ��
            if (gameMapVec[row][col]!=0)
            {
                //������Χ�˸�����
                for (int y = -1; y <= 1; y++)
                {//���£���row���仯
                    for (int x = -1; x <= 1; x++)//���ң���col���仯
                    {//��9�ַ�����ϣ�ԭ���겻�㣬����8�ַ������
                        if(row+y>0 && row+y<kBoardSizeNum && col+x>0 && col+x<kBoardSizeNum){
                            if (!(y == 0 && x == 0) && gameMapVec[row+y][col+x]==0)
                            {   //��ʼ̽��
                                //qDebug()<<"-------------start expend----------------";
                                gameMapVec[row+y][col+x] = player;//�ڵ�ǰ���״̬�½�����չ���º���Ԥ�⣩
                                pair<int,int> tempPair(row+y, col+x);
                                //int temp = -calculateBoardScore(-player);//����ʹ����������С��״̬
                                int temp = minValue(tempPair, -player/*White*/, -Infi/*alph*/, Infi/*beta*/, 0);//wrong!!!
                                qDebug("%d", temp);
                                if(max < temp){
                                    max = temp;
                                    maxScorePoint = tempPair;//ʹ��make_pair��ʽ��������
                                }
                                gameMapVec[row+y][col+x] = 0;//�������֮�󣬻ָ����״̬
                            }
                            else break;
                        }
                    }
                }
            }
        }
    }
    //ѡ�����Žڵ�����
    //qDebug("%d %d",maxScorePoint.first, maxScorePoint.second);
    qDebug()<<"the highest score: "<<max;
    updateGameMap(maxScorePoint.first, maxScorePoint.second);
    return maxScorePoint;
}


int GameModel::maxValue(pair<int,int> nextDot, int player, int alph, int beta, int curDepth)
{
    if(isWin(nextDot.first, nextDot.second) && gameMapVec[nextDot.first][nextDot.second]==-1)
        return 1000;//BOT win
    if(isWin(nextDot.first, nextDot.second) && gameMapVec[nextDot.first][nextDot.second]==1)
        return -1000;//Person win
    if(isDeadGame())
        return 0;//Dead game
    if(curDepth>=3)
        return calculateBoardScore(Black)-calculateBoardScore(White)*2;//̽����ȴ���3��ֱ�Ӽ����ʱ����÷ֲ����أ����̼Ʒְ���BOT���Ƽǣ�

    //to-do ��������δ�����̽����Ȳ�����3����չ��ǰ��֣��ݹ�����ֵ
    int max = -Infi;
    for (int row = 1; row < kBoardSizeNum; row++)
    {
        for (int col = 1; col < kBoardSizeNum; col++)
        {
            //̽���ǿյ��ܱߵĵ㣬��ȫ��ɨ��
            if (gameMapVec[row][col]!=0)
            {
                //������Χ�˸�����
                for (int y = -1; y <= 1; y++)
                {//���£���row���仯
                    for (int x = -1; x <= 1; x++)//���ң���col���仯
                    {//��9�ַ�����ϣ�ԭ���겻�㣬����8�ַ������
                        if(row+y>0 && row+y<kBoardSizeNum && col+x>0 && col+x<kBoardSizeNum){
                            if (!(y == 0 && x == 0) && gameMapVec[row+y][col+x]==0)
                            {   //��ʼ̽��
                                gameMapVec[row+y][col+x] = player;//�ڵ�ǰ���״̬�½�����չ��Ԥ�⣩
                                pair<int,int> tempPair(row+y, col+x);
                                int temp = minValue(tempPair, -player, alph, beta, ++curDepth);
                                if(max < temp){
                                    max = temp;
                                }
                                if(max>=beta){//max���ں󱲽ڵ��betaֵ,alph��֦
                                    gameMapVec[row+y][col+x] = 0;//�������֮�󣬻ָ����״̬
                                    return max;
                                }
                                if(alph > max){
                                    alph = max;
                                }
                                gameMapVec[row+y][col+x] = 0;//�������֮�󣬻ָ����״̬
                            }
                            else break;
                        }
                    }
                }
            }
        }
    }
    return max;
}


int GameModel::minValue(pair<int,int> nextDot, int player, int alph, int beta, int curDepth)
{
    if(isWin(nextDot.first, nextDot.second) && gameMapVec[nextDot.first][nextDot.second]==-1)
        return 1000;//BOT win
    if(isWin(nextDot.first, nextDot.second) && gameMapVec[nextDot.first][nextDot.second]==1)
        return -1000;//Person win
    if(isDeadGame())
        return 0;//Dead game
    if(curDepth>=3)
        return calculateBoardScore(Black)-calculateBoardScore(White)*2;//̽����ȴ���3��ֱ�Ӽ����ʱ����÷ֲ����أ����̼Ʒְ���BOT���Ƽǣ�PersonΪ�෴����

    //to-do ��������δ�����̽����Ȳ�����3����չ��ǰ��֣��ݹ�����ֵ
    int min = Infi;
    for (int row = 1; row < kBoardSizeNum; row++)
    {
        for (int col = 1; col < kBoardSizeNum; col++)
        {
            //̽���ǿյ��ܱߵĵ㣬��ȫ��ɨ��
            if (gameMapVec[row][col]!=0)
            {
                //������Χ�˸�����
                for (int y = -1; y <= 1; y++)
                {//���£���row���仯
                    for (int x = -1; x <= 1; x++)//���ң���col���仯
                    {//��9�ַ�����ϣ�ԭ���겻�㣬����8�ַ������
                        if(row+y>0 && row+y<kBoardSizeNum && col+x>0 && col+x<kBoardSizeNum){
                            if (!(y == 0 && x == 0) && gameMapVec[row+y][col+x]==0)
                            {   //��ʼ̽��
                                //qDebug()<<"-------------minValue start expend----------------";
                                gameMapVec[row+y][col+x] = player/*White*/;//�ڵ�ǰ���״̬�½�����չ��Ԥ�⣩
                                pair<int,int> tempPair(row+y, col+x);
                                int temp = maxValue(tempPair, -player, alph, beta, ++curDepth);
                                if(min > temp){
                                    min = temp;
                                }
                                if(min<=alph){//minС�ڸ��ڵ��alph,alph��֦
                                    gameMapVec[row+y][col+x] = 0;//�������֮�󣬻ָ����״̬
                                    return min;
                                }
                                if(beta < min){
                                    beta = min;
                                }
                                gameMapVec[row+y][col+x] = 0;//�������֮�󣬻ָ����״̬
                            }
                            else break;
                        }
                    }
                }
            }
        }
    }
    return min;
}


bool GameModel::checkFourinLine(int row, int col)
{
    //ˮƽ����
    for (int i = 0; i < 4; i++)
    {
        if (col - i > 0 && col - i + 3 < kBoardSizeNum &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 1] &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 2] &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 3] &&
                ((col-i-1>0 && gameMapVec[row][col-i-1]==0) || (col-i+4<kBoardSizeNum && gameMapVec[row][col-i+4]==0))){
            /*if((i==0 || i==3))*/return true;
        }

    }
    //��ֱ
    for (int i = 0; i < 4; i++)
    {
        if (row - i > 0 && row - i + 3 < kBoardSizeNum &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 1][col] &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 2][col] &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 3][col] &&
                ((row-i-1>0 && gameMapVec[row-i-1][col]==0) || (row-i+4<kBoardSizeNum && gameMapVec[row-i+4][col]==0))){
            /*if(i==0 || i==3)*/return true;
        }

    }
    //��б"/"
    for (int i = 0; i < 4; i++)
    {
        if (row + i < kBoardSizeNum && row + i - 3 > 0 && col - i > 0 && col - i + 3 < kBoardSizeNum &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 1][col - i + 1] &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 2][col - i + 2] &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 3][col - i + 3] &&
                ((row+i+1<kBoardSizeNum && col-i-1>0 && gameMapVec[row+i+1][col-i-1]==0) ||
                 (row+i-4>0 && col-i+4<kBoardSizeNum && gameMapVec[row+i-4][col-i+4]==0))){
            /*if(i==0 || i==3)*/return true;
        }

    }
    //��б"\"
    for (int i = 0; i < 4; i++)
    {
        if (row - i > 0 && row - i + 3 < kBoardSizeNum && col - i > 0 && col - i + 3 < kBoardSizeNum &&
                gameMapVec[row - i][col - i] == gameMapVec[row - i + 1][col - i + 1] &&
                gameMapVec[row - i][col - i] == gameMapVec[row - i + 2][col - i + 2] &&
                gameMapVec[row - i][col - i] == gameMapVec[row - i + 3][col - i + 3] &&
                ((row-i-1>0 && col-i-1>0 && gameMapVec[row-i-1][col-i-1]==0) ||
                 (row-i+4<kBoardSizeNum && col-i+4<kBoardSizeNum && gameMapVec[row-i+4][col-i+4]==0))){
            /*if(i==0 || i==3)*/return true;
        }

    }
    return false;
}


bool GameModel::checkFourinLine2(int row, int col)
{
    //ˮƽ����
    for (int i = 0; i < 4; i++)
    {
        if (col - i > 0 && col - i + 3 < kBoardSizeNum &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 1] &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 2] &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 3] &&
                ((col-i-1>0 && gameMapVec[row][col-i-1]==0) && (col-i+4<kBoardSizeNum && gameMapVec[row][col-i+4]==0))){
            /*if((i==0 || i==3))*/return true;
        }

    }
    //��ֱ
    for (int i = 0; i < 4; i++)
    {
        if (row - i > 0 && row - i + 3 < kBoardSizeNum &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 1][col] &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 2][col] &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 3][col] &&
                ((row-i-1>0 && gameMapVec[row-i-1][col]==0) && (row-i+4<kBoardSizeNum && gameMapVec[row-i+4][col]==0))){
            /*if(i==0 || i==3)*/return true;
        }

    }
    //��б"/"
    for (int i = 0; i < 4; i++)
    {
        if (row + i < kBoardSizeNum && row + i - 3 > 0 && col - i > 0 && col - i + 3 < kBoardSizeNum &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 1][col - i + 1] &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 2][col - i + 2] &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 3][col - i + 3] &&
                ((row+i+1<kBoardSizeNum && col-i-1>0 && gameMapVec[row+i+1][col-i-1]==0) &&
                 (row+i-4>0 && col-i+4<kBoardSizeNum && gameMapVec[row+i-4][col-i+4]==0))){
            /*if(i==0 || i==3)*/return true;
        }

    }
    //��б"\"
    for (int i = 0; i < 4; i++)
    {
        if (row - i > 0 && row - i + 3 < kBoardSizeNum && col - i > 0 && col - i + 3 < kBoardSizeNum &&
                gameMapVec[row - i][col - i] == gameMapVec[row - i + 1][col - i + 1] &&
                gameMapVec[row - i][col - i] == gameMapVec[row - i + 2][col - i + 2] &&
                gameMapVec[row - i][col - i] == gameMapVec[row - i + 3][col - i + 3] &&
                ((row-i-1>0 && col-i-1>0 && gameMapVec[row-i-1][col-i-1]==0) &&
                 (row-i+4<kBoardSizeNum && col-i+4<kBoardSizeNum && gameMapVec[row-i+4][col-i+4]==0))){
            /*if(i==0 || i==3)*/return true;
        }

    }
    return false;
}



int GameModel::calculateLineScore(vector<Piece> temp, int dotType2Cal)
{//1->�������ӣ�-1->AI�����ӣ�0->�ո�
    int aiScore = 0;//���Է���personScore, aiScore, aiScore/personScore, �ȷ���aiScore
    int length = 0;//��¼Ŀ���������ӳ���
    int iter = 0;
    while(iter<int(temp.size())){
        if(temp[iter].curDot==0||temp[iter].curDot==-dotType2Cal){
            if(temp[iter].lastDot==dotType2Cal){
                int tempLength = temp[iter-1].curLength;//iter-1�õ���һ������λ��
                //�ж����������Ƿ��пգ���Ӧ�ӷ�
                if(iter-1-tempLength > 0 && temp[iter-1-tempLength].curDot==0 && temp[iter].curDot==0){
                    if(tempLength==4 || tempLength==3)
                        aiScore+=(length*10+50);//���˿����ģ���ӷ�
                    else
                        aiScore+=(length*10+10);
                }
                else if((iter-1-tempLength > 0 && temp[iter-1-tempLength].curDot==0 && temp[iter].curDot==-dotType2Cal) ||
                        (iter-1-tempLength > 0 && temp[iter-1-tempLength].curDot==-dotType2Cal && temp[iter].curDot==0)){
                    if(tempLength==4)
                        aiScore+=(length*10+50);//һ�˿����ģ���ӷ�
                    else
                        aiScore+=(length*10);
                }
                //�ж��Ƿ������һ���ո�����ӳ��ȳ���4
                if(iter-1-tempLength-1 > 0){//iter-1-(*(iter-1)).curLength-1�õ�����һ�����Ӽ��һ����λ�ĺ���λ��
                    if(temp[iter-1-tempLength-1].curDot==dotType2Cal &&
                            temp[iter-1-tempLength].curDot==0 &&//�жϼ�����Ƿ�Ϊ��λ
                            temp[iter-1-tempLength-1].curLength + tempLength>=4){
                        aiScore+=50;
                    }
                }

                length = 0;
            }
            else{++iter;continue;}
        }
        else{
            if(temp[iter].lastDot==dotType2Cal){
                ++length;
                temp[iter].curLength+=length;
                if(iter==(int)temp.size()-1){
                    int tempLength = temp[iter].curLength;//iter�õ��������λ��
                    //�ж�����ǰ���Ƿ��пգ���Ӧ�ӷ�
                    if(iter-tempLength > 0 && temp[iter-tempLength].curDot==0){//iter-(*(iter)).curLength�õ�����һ����������ķǺ���λ��
                        if(tempLength==4)
                            aiScore+=(length*10+50);//һ�˿����ģ���ӷ�
                        else
                            aiScore+=(length*10);
                    }
                    //�ж��Ƿ������һ���ո�����ӳ��ȳ���4
                    if(iter-1-tempLength > 0){//iter-(*(iter)).curLength-1�õ�����һ�����Ӽ��һ����λ�ĺ���λ��
                        if(temp[iter-tempLength-1].curDot==dotType2Cal &&
                                temp[iter-tempLength].curDot==0 &&//�жϼ�����Ƿ�Ϊ��λ
                                temp[iter-tempLength-1].curLength + tempLength>=4){
                            aiScore+=50;
                        }
                    }
                    length = 0;
                }
            }
            else{
                temp[iter].curLength+=length;
                length = 0;
            }
        }
        ++iter;
    }
    return aiScore;
}


int GameModel::calculateBoardScore(int dotType/* = -1*/)//��������
{
    int boardScore = 0;
    //��������
    vector<Piece> temp;//�洢��������
    //����ˮƽ��������
    for(int thisRow=1; thisRow<kBoardSizeNum; thisRow++){
        for(int thisCol=1; thisCol<kBoardSizeNum; thisCol++){
            if(temp.empty()){//��Ԫ�����⴦��
                temp.push_back(Piece(-2, gameMapVec[thisRow][thisCol], 1));
            }
            else
                temp.push_back(Piece(gameMapVec[thisRow][thisCol-1], gameMapVec[thisRow][thisCol], 1));
        }
        boardScore+=calculateLineScore(temp, dotType);
        temp.clear();
    }

    //������ֱ��������
    for(int thisCol=1; thisCol<kBoardSizeNum; thisCol++){
        for(int thisRow=1; thisRow<kBoardSizeNum; thisRow++){
            if(temp.empty()){//��Ԫ�����⴦��
                temp.push_back(Piece(-2, gameMapVec[thisRow][thisCol], 1));
            }
            else
                temp.push_back(Piece(gameMapVec[thisRow-1][thisCol], gameMapVec[thisRow][thisCol], 1));
        }
        boardScore+=calculateLineScore(temp, dotType);
        temp.clear();
    }

    //��������-������������
    for(int thisRow=5; thisRow<kBoardSizeNum; thisRow++){
        int tempCol = 1, tempRow = thisRow;
        while(tempCol<kBoardSizeNum && tempRow>0){
            if(temp.empty()){//��Ԫ�����⴦��
                temp.push_back(Piece(-2, gameMapVec[tempRow][tempCol], 1));
            }
            else
                temp.push_back(Piece(gameMapVec[tempRow+1][tempCol-1], gameMapVec[tempRow][tempCol], 1));
            ++tempCol; --tempRow;
        }
        boardScore+=calculateLineScore(temp, dotType);
        temp.clear();
    }
    for(int thisCol=2; thisCol<=10; thisCol++){
        int tempCol = thisCol, tempRow = 14;
        while(tempCol<kBoardSizeNum && tempRow>1){//���Խ����Ѿ������
            if(temp.empty()){//��Ԫ�����⴦��
                temp.push_back(Piece(-2, gameMapVec[tempRow][tempCol], 1));
            }
            else
                temp.push_back(Piece(gameMapVec[tempRow+1][tempCol-1], gameMapVec[tempRow][tempCol], 1));
            ++tempCol; --tempRow;
        }
        boardScore+=calculateLineScore(temp, dotType);
        temp.clear();
    }

    //��������-������������
    for(int thisRow=10; thisRow>0; thisRow--){
        int tempCol = 1, tempRow = thisRow;
        while(tempCol<kBoardSizeNum && tempRow<kBoardSizeNum){
            if(temp.empty()){//��Ԫ�����⴦��
                temp.push_back(Piece(-2, gameMapVec[tempRow][tempCol], 1));
            }
            else
                temp.push_back(Piece(gameMapVec[tempRow-1][tempCol-1], gameMapVec[tempRow][tempCol], 1));
            ++tempCol; ++tempRow;
        }
        boardScore+=calculateLineScore(temp, dotType);
        temp.clear();
    }
    for(int thisCol=2; thisCol<=10; thisCol++){
        int tempCol = thisCol, tempRow = 1;
        while(tempCol<kBoardSizeNum-1 && tempRow>1){//���Խ����Ѿ������
            if(temp.empty()){//��Ԫ�����⴦��
                temp.push_back(Piece(-2, gameMapVec[tempRow][tempCol], 1));
            }
            else
                temp.push_back(Piece(gameMapVec[tempRow-1][tempCol-1], gameMapVec[tempRow][tempCol], 1));
            ++tempCol; ++tempRow;
        }
        boardScore+=calculateLineScore(temp, dotType);
        temp.clear();
    }

    //�������
    return boardScore;
}


bool GameModel::isWin(int row, int col)
{
    //����б���������ÿ��������ݵ�ǰ���ӱ���5������
    //ˮƽ����
    for (int i = 0; i < 5; i++)
    {
        //����5��������ƥ��4����
        if (col - i > 0 && col - i + 4 < kBoardSizeNum &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 1] &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 2] &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 3] &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 4])
            return true;
    }

    //��ֱ����(��������4��)
    for (int i = 0; i < 5; i++)
    {
        if (row - i > 0 && row - i + 4 < kBoardSizeNum &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 1][col] &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 2][col] &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 3][col] &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 4][col])
            return true;
    }

    //��б����
    for (int i = 0; i < 5; i++)
    {
        if (row + i < kBoardSizeNum && row + i - 4 > 0 && col - i > 0 && col - i + 4 < kBoardSizeNum &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 1][col - i + 1] &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 2][col - i + 2] &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 3][col - i + 3] &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 4][col - i + 4])
            return true;
    }

    //��б����
    for (int i = 0; i < 5; i++)
    {
        if (row - i > 0 && row - i + 4 < kBoardSizeNum && col - i > 0 && col - i + 4 < kBoardSizeNum &&
                gameMapVec[row - i][col - i] == gameMapVec[row - i + 1][col - i + 1] &&
                gameMapVec[row - i][col - i] == gameMapVec[row - i + 2][col - i + 2] &&
                gameMapVec[row - i][col - i] == gameMapVec[row - i + 3][col - i + 3] &&
                gameMapVec[row - i][col - i] == gameMapVec[row - i + 4][col - i + 4])
            return true;
    }

    return false;
}


bool GameModel::isDeadGame()
{
    //���пո�ȫ������
    for (int i = 1; i < kBoardSizeNum; i++)
        for (int j = 1; j < kBoardSizeNum; j++)
        {
            if (!(gameMapVec[i][j] == 1 || gameMapVec[i][j] == -1))
                return false;
        }
    return true;
}

