#include <utility>//pair类型
#include <time.h>//随机数
#include<stdlib.h>//随机数
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
    //初始棋盘
    gameMapVec.clear();
    for (int i = 0; i < kBoardSizeNum; i++)
    {
        std::vector<int> lineBoard;
        for (int j = 0; j < kBoardSizeNum; j++)
            lineBoard.push_back(0);
        gameMapVec.push_back(lineBoard);
    }
    //己方下为true,对方下为false，此处设置为己方先下
    playerFlag = true;
}


void GameModel::updateGameMap(int row, int col)
{
    if (playerFlag)
        gameMapVec[row][col] = 1;//己方落白子
    else
        gameMapVec[row][col] = -1;//对方落黑子

    //换手
    playerFlag = !playerFlag;
}


void GameModel::actionByPerson(int row, int col)
{
    updateGameMap(row, col);
}


pair<int, int> GameModel::actionByAI()//拓展节点（局部最优算法/贪心算法）//探索一步
{
    //naive版本，直接产生随机数标记坐标
//    while(true){
//        srand((unsigned)time(NULL));
//        int clickRow, clickCol;
//        clickRow = rand()%kBoardSizeNum; //记录落子点
//        clickCol = rand()%kBoardSizeNum;
//        if(clickCol!=0&&clickRow!=0){
//            updateGameMap(clickRow, clickCol);
//            pair<int,int> tempReturn(clickRow,clickCol);
//            return tempReturn;
//            break;
//        }
//    }

    //向前探索一步版本
    priority_queue<state, vector<state>, CompareScore> scoreQue;//最优选择队列
    for (int row = 1; row < kBoardSizeNum; row++){
        for (int col = 1; col < kBoardSizeNum; col++)
        {
            //探索非空点周边的点，不全部扫描
            if (gameMapVec[row][col]!=0)
            {
                //遍历周围八个方向
                for (int y = -1; y <= 1; y++){//上下，对row做变化
                    //qDebug()<<"before forth loop, the index of third loop: "<<y;
                    for (int x = -1; x <= 1; x++)//左右，对col做变化
                    {//共9种方向组合，原坐标不算，还有8种方向组合
                        //(row+y>0 && row+y<kBoardSizeNum && col+x>0 && col+x<kBoardSizeNum)?qDebug()<<"ok to loop":qDebug()<<"cannot loop";
                        if(row+y>0 && row+y<kBoardSizeNum && col+x>0 && col+x<kBoardSizeNum){
                            if ((!(y == 0 && x == 0)) && gameMapVec[row+y][col+x]==0)
                            {   //开始探索
                                qDebug()<<"Before score: Dot("<<row+y<<","<<col+x<<"):"<<gameMapVec[row+y][col+x];
                                qDebug()<<"-------------start expend----------------";
                                gameMapVec[row+y][col+x] = -1;//在当前棋局状态下进行扩展（下黑子预测）
                                state tempState;
                                tempState.dot = make_pair(row+y, col+x);
                                tempState.totalScore = double(calculateBoardScore(Black)) / double(calculateBoardScore(White))*1.75;//调整权重
                                scoreQue.push(tempState);
                                qDebug()<<"The highest score: "<<scoreQue.top().totalScore;
                                gameMapVec[row+y][col+x] = 0;//计算完成之后，恢复棋局状态
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
    //选择最优节点落子
    qDebug()<<"***************Loop finish*****************";
    state decideState = scoreQue.top();
    priority_queue<state, vector<state>, CompareScore> doClearQue;
    swap(doClearQue, scoreQue);
    updateGameMap(decideState.dot.first, decideState.dot.second);
    return decideState.dot;
}


pair<int, int> GameModel::actionByAI2()//minMax
{
    //MinMax算法版本
    //目前已经可以计算出每个状态的得分，可以通过剪枝扩展：BOT(MAXscore)->Person(MINscore)->BOT(MAXscore)->Person(MINscore)->……
    qDebug()<<"-------------next judge step----------------";
    int player = Black;//表示黑子
    int max = -Infi;
    pair<int,int> maxScorePoint(-1, -1);//初始化
    for (int row = 1; row < kBoardSizeNum; row++)
    {
        for (int col = 1; col < kBoardSizeNum; col++)
        {
            //探索非空点周边的点，不全部扫描
            if (gameMapVec[row][col]!=0)
            {
                //遍历周围八个方向
                for (int y = -1; y <= 1; y++)
                {//上下，对row做变化
                    for (int x = -1; x <= 1; x++)//左右，对col做变化
                    {//共9种方向组合，原坐标不算，还有8种方向组合
                        if(row+y>0 && row+y<kBoardSizeNum && col+x>0 && col+x<kBoardSizeNum){
                            if (!(y == 0 && x == 0) && gameMapVec[row+y][col+x]==0)
                            {   //开始探索
                                //qDebug()<<"-------------start expend----------------";
                                gameMapVec[row+y][col+x] = player;//在当前棋局状态下进行扩展（下黑子预测）
                                pair<int,int> tempPair(row+y, col+x);
                                //int temp = -calculateBoardScore(-player);//返回使白子优势最小的状态
                                int temp = minValue(tempPair, -player/*White*/, -Infi/*alph*/, Infi/*beta*/, 0);//wrong!!!
                                qDebug("%d", temp);
                                if(max < temp){
                                    max = temp;
                                    maxScorePoint = tempPair;//使用make_pair方式创建对组
                                }
                                gameMapVec[row+y][col+x] = 0;//计算完成之后，恢复棋局状态
                            }
                            else break;
                        }
                    }
                }
            }
        }
    }
    //选择最优节点落子
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
        return calculateBoardScore(Black)-calculateBoardScore(White)*2;//探索深度大于3，直接计算此时盘面得分并返回（棋盘计分按照BOT优势记）

    //to-do 如果棋局尚未完结且探索深度不超过3，扩展当前棋局，递归计算价值
    int max = -Infi;
    for (int row = 1; row < kBoardSizeNum; row++)
    {
        for (int col = 1; col < kBoardSizeNum; col++)
        {
            //探索非空点周边的点，不全部扫描
            if (gameMapVec[row][col]!=0)
            {
                //遍历周围八个方向
                for (int y = -1; y <= 1; y++)
                {//上下，对row做变化
                    for (int x = -1; x <= 1; x++)//左右，对col做变化
                    {//共9种方向组合，原坐标不算，还有8种方向组合
                        if(row+y>0 && row+y<kBoardSizeNum && col+x>0 && col+x<kBoardSizeNum){
                            if (!(y == 0 && x == 0) && gameMapVec[row+y][col+x]==0)
                            {   //开始探索
                                gameMapVec[row+y][col+x] = player;//在当前棋局状态下进行扩展（预测）
                                pair<int,int> tempPair(row+y, col+x);
                                int temp = minValue(tempPair, -player, alph, beta, ++curDepth);
                                if(max < temp){
                                    max = temp;
                                }
                                if(max>=beta){//max大于后辈节点的beta值,alph剪枝
                                    gameMapVec[row+y][col+x] = 0;//计算完成之后，恢复棋局状态
                                    return max;
                                }
                                if(alph > max){
                                    alph = max;
                                }
                                gameMapVec[row+y][col+x] = 0;//计算完成之后，恢复棋局状态
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
        return calculateBoardScore(Black)-calculateBoardScore(White)*2;//探索深度大于3，直接计算此时盘面得分并返回（棋盘计分按照BOT优势记，Person为相反数）

    //to-do 如果棋局尚未完结且探索深度不超过3，扩展当前棋局，递归计算价值
    int min = Infi;
    for (int row = 1; row < kBoardSizeNum; row++)
    {
        for (int col = 1; col < kBoardSizeNum; col++)
        {
            //探索非空点周边的点，不全部扫描
            if (gameMapVec[row][col]!=0)
            {
                //遍历周围八个方向
                for (int y = -1; y <= 1; y++)
                {//上下，对row做变化
                    for (int x = -1; x <= 1; x++)//左右，对col做变化
                    {//共9种方向组合，原坐标不算，还有8种方向组合
                        if(row+y>0 && row+y<kBoardSizeNum && col+x>0 && col+x<kBoardSizeNum){
                            if (!(y == 0 && x == 0) && gameMapVec[row+y][col+x]==0)
                            {   //开始探索
                                //qDebug()<<"-------------minValue start expend----------------";
                                gameMapVec[row+y][col+x] = player/*White*/;//在当前棋局状态下进行扩展（预测）
                                pair<int,int> tempPair(row+y, col+x);
                                int temp = maxValue(tempPair, -player, alph, beta, ++curDepth);
                                if(min > temp){
                                    min = temp;
                                }
                                if(min<=alph){//min小于父节点的alph,alph剪枝
                                    gameMapVec[row+y][col+x] = 0;//计算完成之后，恢复棋局状态
                                    return min;
                                }
                                if(beta < min){
                                    beta = min;
                                }
                                gameMapVec[row+y][col+x] = 0;//计算完成之后，恢复棋局状态
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
    //水平方向
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
    //竖直
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
    //左斜"/"
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
    //右斜"\"
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
    //水平方向
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
    //竖直
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
    //左斜"/"
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
    //右斜"\"
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
{//1->己方白子，-1->AI方黑子，0->空格
    int aiScore = 0;//可以返回personScore, aiScore, aiScore/personScore, 先返回aiScore
    int length = 0;//记录目标棋子连子长度
    int iter = 0;
    while(iter<int(temp.size())){
        if(temp[iter].curDot==0||temp[iter].curDot==-dotType2Cal){
            if(temp[iter].lastDot==dotType2Cal){
                int tempLength = temp[iter-1].curLength;//iter-1得到上一个黑子位置
                //判断连子两端是否有空，相应加分
                if(iter-1-tempLength > 0 && temp[iter-1-tempLength].curDot==0 && temp[iter].curDot==0){
                    if(tempLength==4 || tempLength==3)
                        aiScore+=(length*10+50);//两端空连四，多加分
                    else
                        aiScore+=(length*10+10);
                }
                else if((iter-1-tempLength > 0 && temp[iter-1-tempLength].curDot==0 && temp[iter].curDot==-dotType2Cal) ||
                        (iter-1-tempLength > 0 && temp[iter-1-tempLength].curDot==-dotType2Cal && temp[iter].curDot==0)){
                    if(tempLength==4)
                        aiScore+=(length*10+50);//一端空连四，多加分
                    else
                        aiScore+=(length*10);
                }
                //判断是否有相见一个空格的连子长度超过4
                if(iter-1-tempLength-1 > 0){//iter-1-(*(iter-1)).curLength-1得到和上一串黑子间隔一个空位的黑子位置
                    if(temp[iter-1-tempLength-1].curDot==dotType2Cal &&
                            temp[iter-1-tempLength].curDot==0 &&//判断间隔的是否为空位
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
                    int tempLength = temp[iter].curLength;//iter得到这个黑子位置
                    //判断连子前端是否有空，相应加分
                    if(iter-tempLength > 0 && temp[iter-tempLength].curDot==0){//iter-(*(iter)).curLength得到和上一串黑子最近的非黑子位置
                        if(tempLength==4)
                            aiScore+=(length*10+50);//一端空连四，多加分
                        else
                            aiScore+=(length*10);
                    }
                    //判断是否有相见一个空格的连子长度超过4
                    if(iter-1-tempLength > 0){//iter-(*(iter)).curLength-1得到和上一串黑子间隔一个空位的黑子位置
                        if(temp[iter-tempLength-1].curDot==dotType2Cal &&
                                temp[iter-tempLength].curDot==0 &&//判断间隔的是否为空位
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


int GameModel::calculateBoardScore(int dotType/* = -1*/)//计算评分
{
    int boardScore = 0;
    //计算评分
    vector<Piece> temp;//存储各行数据
    //计算水平连线评分
    for(int thisRow=1; thisRow<kBoardSizeNum; thisRow++){
        for(int thisCol=1; thisCol<kBoardSizeNum; thisCol++){
            if(temp.empty()){//首元素特殊处理
                temp.push_back(Piece(-2, gameMapVec[thisRow][thisCol], 1));
            }
            else
                temp.push_back(Piece(gameMapVec[thisRow][thisCol-1], gameMapVec[thisRow][thisCol], 1));
        }
        boardScore+=calculateLineScore(temp, dotType);
        temp.clear();
    }

    //计算竖直连线评分
    for(int thisCol=1; thisCol<kBoardSizeNum; thisCol++){
        for(int thisRow=1; thisRow<kBoardSizeNum; thisRow++){
            if(temp.empty()){//首元素特殊处理
                temp.push_back(Piece(-2, gameMapVec[thisRow][thisCol], 1));
            }
            else
                temp.push_back(Piece(gameMapVec[thisRow-1][thisCol], gameMapVec[thisRow][thisCol], 1));
        }
        boardScore+=calculateLineScore(temp, dotType);
        temp.clear();
    }

    //计算左下-右上连线评分
    for(int thisRow=5; thisRow<kBoardSizeNum; thisRow++){
        int tempCol = 1, tempRow = thisRow;
        while(tempCol<kBoardSizeNum && tempRow>0){
            if(temp.empty()){//首元素特殊处理
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
        while(tempCol<kBoardSizeNum && tempRow>1){//主对角线已经计算过
            if(temp.empty()){//首元素特殊处理
                temp.push_back(Piece(-2, gameMapVec[tempRow][tempCol], 1));
            }
            else
                temp.push_back(Piece(gameMapVec[tempRow+1][tempCol-1], gameMapVec[tempRow][tempCol], 1));
            ++tempCol; --tempRow;
        }
        boardScore+=calculateLineScore(temp, dotType);
        temp.clear();
    }

    //计算右下-左上连线评分
    for(int thisRow=10; thisRow>0; thisRow--){
        int tempCol = 1, tempRow = thisRow;
        while(tempCol<kBoardSizeNum && tempRow<kBoardSizeNum){
            if(temp.empty()){//首元素特殊处理
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
        while(tempCol<kBoardSizeNum-1 && tempRow>1){//主对角线已经计算过
            if(temp.empty()){//首元素特殊处理
                temp.push_back(Piece(-2, gameMapVec[tempRow][tempCol], 1));
            }
            else
                temp.push_back(Piece(gameMapVec[tempRow-1][tempCol-1], gameMapVec[tempRow][tempCol], 1));
            ++tempCol; ++tempRow;
        }
        boardScore+=calculateLineScore(temp, dotType);
        temp.clear();
    }

    //计算结束
    return boardScore;
}


bool GameModel::isWin(int row, int col)
{
    //横竖斜四种情况，每种情况根据当前落子遍历5个棋子
    //水平方向
    for (int i = 0; i < 5; i++)
    {
        //往左5个，往右匹配4个子
        if (col - i > 0 && col - i + 4 < kBoardSizeNum &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 1] &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 2] &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 3] &&
                gameMapVec[row][col - i] == gameMapVec[row][col - i + 4])
            return true;
    }

    //竖直方向(上下延伸4个)
    for (int i = 0; i < 5; i++)
    {
        if (row - i > 0 && row - i + 4 < kBoardSizeNum &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 1][col] &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 2][col] &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 3][col] &&
                gameMapVec[row - i][col] == gameMapVec[row - i + 4][col])
            return true;
    }

    //左斜方向
    for (int i = 0; i < 5; i++)
    {
        if (row + i < kBoardSizeNum && row + i - 4 > 0 && col - i > 0 && col - i + 4 < kBoardSizeNum &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 1][col - i + 1] &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 2][col - i + 2] &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 3][col - i + 3] &&
                gameMapVec[row + i][col - i] == gameMapVec[row + i - 4][col - i + 4])
            return true;
    }

    //右斜方向
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
    //所有空格全部填满
    for (int i = 1; i < kBoardSizeNum; i++)
        for (int j = 1; j < kBoardSizeNum; j++)
        {
            if (!(gameMapVec[i][j] == 1 || gameMapVec[i][j] == -1))
                return false;
        }
    return true;
}

