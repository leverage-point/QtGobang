#ifndef AIGAMESTATE_H
#define AIGAMESTATE_H

#include<queue>//使用优先队列
using namespace std;

class AIGameState
{
public:
    AIGameState();
    void calculateScore();

    priority_queue<int> score;
    int totalScore;
};

#endif // AIGAMESTATE_H
