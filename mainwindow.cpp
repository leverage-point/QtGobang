#include<QPainter>
#include<QTimer>
#include<QSound>
#include<QMouseEvent>
#include<QMessageBox>
#include<QMenuBar>
#include<QMenu>
#include<QAction>
#include<QDebug>
#include<math.h>
#include<utility>//pair
#include"mainwindow.h"

//*************************************************

const int kBoardMargin = 30; //棋盘边缘空隙
const int kRadius = 18; //棋子半径
const int kMarkSize = 6; //落子标记边长
const int kBlockSize = 40; //格子的大小
const int kPosDelta = kBlockSize/2; //鼠标点击的模糊距离上限
const int kAIDelay = 700; //AI下棋的思考时间

#define CHESS_ONE_SOUND ":/new/sound/chessone.wav"
#define WIN_SOUND ":/new/sound/win.wav"
#define LOSE_SOUND ":/new/sound/lose.wav"

//*************************************************

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //设置棋盘大小
    setFixedSize(kBoardMargin * 2 + kBlockSize * kBoardSizeNum, kBoardMargin * 2 + kBlockSize * kBoardSizeNum);

    //开启鼠标hover功能
    setMouseTracking(true);

    //添加菜单
    //选择人人对战
    QMenu *gameMenu = menuBar()->addMenu(tr("Game Option"));
    QAction *actionPVP = new QAction("Person VS Person", this);
    connect(actionPVP, SIGNAL(triggered()), this, SLOT(initPVPGame()));
    gameMenu->addAction(actionPVP);

    //选择人机对战
    QAction *actionPVE = new QAction("Person VS Computer(primary)", this);
    connect(actionPVE, SIGNAL(triggered()), this, SLOT(initPVEGame()));
    gameMenu->addAction(actionPVE);

    QAction *actionPVE2 = new QAction("Person VS Computer(advanced)", this);
    connect(actionPVE2, SIGNAL(triggered()), this, SLOT(initPVEGame2()));
    gameMenu->addAction(actionPVE2);

    //返回游戏开始界面
    QAction *back = new QAction("To Start Screen", this);
    connect(back, &QAction::triggered, [=](){
        emit this->chooseScenceBack();
    });
    gameMenu->addAction(back);

    //初始化游戏
    initGame();
}


MainWindow::~MainWindow()
{
    if (game) delete game;
}


void MainWindow::initGame()
{
    //初始化游戏模型
    game = new GameModel;
    initPVPGame();
}


void MainWindow::initPVPGame()
{
    game_type = PERSON;
    game->gameStatus = PLAYING;
    game->startGame(game_type);
    update();
}


void MainWindow::initPVEGame()
{
    game_type = BOT;
    game->gameStatus = PLAYING;
    game->startGame(game_type);
    update();
}


void MainWindow::initPVEGame2()
{
    game_type = BOT2;
    game->gameStatus = PLAYING;
    game->startGame(game_type);
    update();
}


void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QPixmap pix;
    pix.load(":/new/image/background2.png");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);

    //绘制棋盘
    painter.setRenderHint(QPainter::Antialiasing, true); //Indicates that the engine should antialias edges of primitives if possible.

    for (int i = 0; i < kBoardSizeNum + 1; i++)
    {
        painter.drawLine(kBoardMargin + kBlockSize * i, kBoardMargin, kBoardMargin + kBlockSize * i, size().height() - kBoardMargin);
        painter.drawLine(kBoardMargin, kBoardMargin + kBlockSize * i, size().width() - kBoardMargin, kBoardMargin + kBlockSize * i);
    }

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    //绘制落子标记(防止鼠标出框越界)
    if (clickPosRow > 0 && clickPosRow < kBoardSizeNum && clickPosCol > 0 && clickPosCol < kBoardSizeNum &&
        game->gameMapVec[clickPosRow][clickPosCol] == 0)
    {
        if (game->playerFlag)
            brush.setColor(Qt::white);
        else
            brush.setColor(Qt::black);
        painter.setBrush(brush);
        painter.drawRect(kBoardMargin + kBlockSize * clickPosCol - kMarkSize / 2, kBoardMargin + kBlockSize * clickPosRow - kMarkSize / 2, kMarkSize, kMarkSize);
    }

    //判断输赢
    if (clickPosRow > 0 && clickPosRow < kBoardSizeNum &&
        clickPosCol > 0 && clickPosCol < kBoardSizeNum &&
        (game->gameMapVec[clickPosRow][clickPosCol] == 1 ||
            game->gameMapVec[clickPosRow][clickPosCol] == -1))
    {
        if (game->isWin(clickPosRow, clickPosCol) && game->gameStatus == PLAYING)
        {
            game->gameStatus = WIN;
            QSound::play(WIN_SOUND);
            QString str;
            if (game->gameMapVec[clickPosRow][clickPosCol] == 1)
                str = "white player";
            else if (game->gameMapVec[clickPosRow][clickPosCol] == -1)
                str = "black player";
            QMessageBox::StandardButton btnValue = QMessageBox::information(this, "congratulations", str + "win!");

            //重置游戏状态，否则容易死循环
            if (btnValue == QMessageBox::Ok)
            {
                game->startGame(game_type);
                game->gameStatus = PLAYING;
            }
        }
    }

    //绘制棋子
    for (int i = 0; i < kBoardSizeNum; i++){
        for (int j = 0; j < kBoardSizeNum; j++)
        {
            if (game->gameMapVec[i][j] == 1)
            {
                brush.setColor(Qt::white);
                painter.setBrush(brush);
                painter.drawEllipse(kBoardMargin + kBlockSize * j - kRadius, kBoardMargin + kBlockSize * i - kRadius, kRadius * 2, kRadius * 2);
            }
            else if (game->gameMapVec[i][j] == -1)
            {
                brush.setColor(Qt::black);
                painter.setBrush(brush);
                painter.drawEllipse(kBoardMargin + kBlockSize * j - kRadius, kBoardMargin + kBlockSize * i - kRadius, kRadius * 2, kRadius * 2);
            }
        }
    }

    //判断死局（当棋子布满棋盘时）
    if (game->isDeadGame())
    {
        QSound::play(LOSE_SOUND);
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "notice", "dead game!");
        if (btnValue == QMessageBox::Ok)
        {
            game->startGame(game_type);
            game->gameStatus = PLAYING;
        }

    }
}


void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //通过鼠标的hover确定落子的标记
    int x = event->x();
    int y = event->y();

    //棋盘边缘不能落子
    if (x >= kBoardMargin + kBlockSize / 2 && x < size().width() - kBoardMargin &&
            y >= kBoardMargin + kBlockSize / 2 && y < size().height()- kBoardMargin)
    {
        //获取最近的左上角的点(leftTopPosX,leftTopPosY)
        int col = x / kBlockSize;
        int row = y / kBlockSize;

        int leftTopPosX = kBoardMargin + kBlockSize * col;
        int leftTopPosY = kBoardMargin + kBlockSize * row;

        //根据距离算出合适的点击位置,一共四个点，根据半径距离选最近的
        clickPosRow = -1; //初始化最终的值
        clickPosCol = -1;//初始化最终的值
        int len = 0;

        //确定一个误差在范围内的点，且只可能确定一个出来
        len = sqrt((x - leftTopPosX) * (x - leftTopPosX) + (y - leftTopPosY) * (y - leftTopPosY));
        if (len < kPosDelta)
        {
            clickPosRow = row;
            clickPosCol = col;
        }
        len = sqrt((x - leftTopPosX - kBlockSize) * (x - leftTopPosX - kBlockSize) + (y - leftTopPosY) * (y - leftTopPosY));
        if (len < kPosDelta)
        {
            clickPosRow = row;
            clickPosCol = col + 1;
        }
        len = sqrt((x - leftTopPosX) * (x - leftTopPosX) + (y - leftTopPosY - kBlockSize) * (y - leftTopPosY - kBlockSize));
        if (len < kPosDelta)
        {
            clickPosRow = row + 1;
            clickPosCol = col;
        }
        len = sqrt((x - leftTopPosX - kBlockSize) * (x - leftTopPosX - kBlockSize) + (y - leftTopPosY - kBlockSize) * (y - leftTopPosY - kBlockSize));
        if (len < kPosDelta)
        {
            clickPosRow = row + 1;
            clickPosCol = col + 1;
        }
    }
    //qDebug()<<clickPosRow<<" "<<clickPosCol;
    //储存坐标后重绘
    update();
}


void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    //人下棋，并且不能抢机器的棋
    if (!((game_type == BOT || game_type ==BOT2) && !game->playerFlag))//除人机模式且轮到BOT的情况外的所有情况
    {
        chessOneByPerson();
        //如果是人机模式，需要调用AI下棋
        if (game->gameType == BOT && !game->playerFlag)
        {
            //用定时器做一个延迟
            QTimer::singleShot(kAIDelay, this, SLOT(chessOneByAI()));
        }
        if (game->gameType == BOT2 && !game->playerFlag)
        {
            //用定时器做一个延迟
            QTimer::singleShot(kAIDelay, this, SLOT(chessOneByAI2()));
        }
    }
}


void MainWindow::chessOneByPerson()
{
    //根据当前存储的坐标下子
    //只有有效点击才下子，并且该处没有子，clickPosRow和clickPosCol的初值为-1
    if (clickPosRow != -1 && clickPosCol != -1 && game->gameMapVec[clickPosRow][clickPosCol] == 0)
    {
        game->actionByPerson(clickPosRow, clickPosCol);
        QSound::play(CHESS_ONE_SOUND);
        //重绘
        update();
    }
}


void MainWindow::chessOneByAI()
{
    pair<int,int> temp(-1,-1);
    temp = game->actionByAI();
    clickPosRow = temp.first;
    clickPosCol = temp.second;
    QSound::play(CHESS_ONE_SOUND);
    update();
}


void MainWindow::chessOneByAI2()
{
    pair<int,int> temp(-1,-1);
    temp = game->actionByAI2();
    clickPosRow = temp.first;
    clickPosCol = temp.second;
    QSound::play(CHESS_ONE_SOUND);
    update();
}

