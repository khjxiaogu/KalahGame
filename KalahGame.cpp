//宝石棋游戏代码
//
#include <iostream>
#include <ctime>
#include <iomanip>
using namespace std;
//定义棋子初始值
#define INITIAL_CHESS 4
//定义棋洞数量
#define COUNT_HOLE 6
//洞类
class Pit
{
public:
    Pit(short position) :position(position) {}
    Pit* next;//链表指示下一个棋洞
    Pit* opposite;//指示对面的棋洞
    short count;//棋子数量
    short position;//棋洞位置
};
//通用棋盘
class KalahBoard {
protected:
    Pit** pits = new Pit*[12];//全部游戏洞
    Pit* score;//玩家自己的积分洞
    Pit* score2;//对方玩家的积分洞
    const char* name;//玩家的名字
public:
    KalahBoard(const char* name):name(name) {
        /*
             11-10- 9- 8- 7- 6\
       (13)sc2|               sc(12)
              0- 1- 2- 3- 4- 5/
        */
        for (int i = 0; i < COUNT_HOLE*2; i++)
            pits[i] = new Pit(i);
        for (int i = 0; i < COUNT_HOLE*2-1; i++)//按照棋子摆放顺序依次链接所有棋洞
            pits[i]->next = pits[i+1];
        pits[COUNT_HOLE*2-1]->next = pits[0];//首尾相连
        score = new Pit(COUNT_HOLE*2);
        pits[COUNT_HOLE-1]->next = score;//在链表内插入己方积分洞
        score->next = pits[COUNT_HOLE];
        score2 = new Pit(COUNT_HOLE*2+1);
        for (int i = 0; i < COUNT_HOLE; i++) {//指派各个棋洞对面的棋洞
            pits[i]->opposite = pits[COUNT_HOLE * 2 - 1 - i];
            pits[COUNT_HOLE * 2 - 1 - i]->opposite = pits[i];
        }
        reset();
    }
    virtual ~KalahBoard() {
        for (int i = 0; i < COUNT_HOLE*2; i++) {
            delete pits[i];
        }
        delete[] pits;
        delete score;
        delete score2;
    }
    /*
    *获取名字
    */
    const char* getName() const{
        return name;
    }
    /*
    *要求开始输入，返回1-7表示输入值，返回0表示请求了退出
    */
    virtual int input() = 0;
    /*
    *检查棋子格是否可以被选中
    */
    bool movable(int pit) {
        return pits[pit - 1]->count != 0;
    }
    /*
    *移动棋子
    *如果移动完成了，返回0
    *如果玩家需要继续操作，返回1
    */
    int move(int pit) {
        Pit* p = pits[pit - 1];
        int count = p->count;
        p->count = 0;
        //棋子移动
        while(count--)
        {
            p = p->next;
            p->count++;
        }
        //吃棋子判定
        if (p->count == 1 && p->position < COUNT_HOLE && p->position >= 0&&p->opposite->count>0)//如果最后放下的是己方洞
        {
            p->count = 0;//取走自己身上的棋子
            score->count += p->opposite->count + 1;//和对面的棋子一起放到己方洞
            p->opposite->count = 0;//取走对面的棋子
        }
        else if (p->position == COUNT_HOLE * 2)//最后放下的是己方积分洞，要求继续操作
            return 1;
        return 0;
    }
    void reset() {//初始化游戏设定
        for (int i = 0; i < COUNT_HOLE*2; i++) {
            pits[i]->count = INITIAL_CHESS;
        }
        score->count = 0;
        score2->count = 0;
    }
    /*
    *检查是否应该结束游戏
    */
    bool canOperate() {
        if (score->count > COUNT_HOLE * INITIAL_CHESS || score2->count > COUNT_HOLE * INITIAL_CHESS)//其中一位玩家持有一半以上棋子了
            return 0;
        int opable = COUNT_HOLE;
        for (int i = 0; i < COUNT_HOLE; i++) {
            if (pits[i]->count == 0)
                opable--;
        }
        if (opable == 0)return 0;
        return 1;
    }
    /*
    *保存棋盘游戏状态到数组
    *数组至少有COUNT_HOLE*2+2个内存空间
    */
    void save(short*data) {
        for (int i = 0; i < COUNT_HOLE*2; i++)
            data[i] = pits[i]->count;
        data[COUNT_HOLE * 2] = score->count;
        data[COUNT_HOLE * 2+1] = score2->count;
    }
    /*
    *从数组恢复棋盘游戏状态
    *数组至少有14个内存空间
    */
    void load(short*data) {
        for (int i = 0; i < COUNT_HOLE * 2; i++)
             pits[i]->count= data[i];
        score->count= data[COUNT_HOLE * 2];
        score2->count= data[COUNT_HOLE * 2 + 1];
    }
    /*
    *从对方棋盘的数组恢复当前棋盘的游戏状态
    *数组至少有14个内存空间
    */
    void loadReverse(short* data) {
        for (int i = 0; i < COUNT_HOLE; i++) {//交换双方阵列
            pits[i + COUNT_HOLE]->count = data[i];
        }
        for (int i = COUNT_HOLE; i < COUNT_HOLE*2; i++) {
            pits[i- COUNT_HOLE]->count = data[i];
        }
        score->count = data[COUNT_HOLE*2+1];
        score2->count = data[COUNT_HOLE*2];
    }
    /*
    *检查是否符合胜利条件
    *如果为正数则是胜利
    *0则为平局
    *为负则是输了
    */
    int win() {
        if(!canOperate())
            for (int i = COUNT_HOLE; i < COUNT_HOLE*2; i++) {
                score2->count += pits[i]->count;
            }
        return score->count - score2->count;
    }
    /*
    *检查是否符合胜利条件
    *如果为正数则是胜利
    *0则为平局
    *为负则是输了
    */
    int checkWin() {
        int s2 = score2->count;
        int s1 = score->count;
        if (!canOperate())
            for (int i = COUNT_HOLE; i < COUNT_HOLE * 2; i++) {
                s2 += pits[i]->count;
            }
        return s1 - s2;
    }
    int getScore() {
        return score->count;
    }
    int getScore2() {
        return score2->count;
    }
};
//玩家的棋盘继承通用棋盘
class PlayerKalahBoard :public KalahBoard {
public:
    PlayerKalahBoard(const char*name):KalahBoard(name){}
    virtual int input() {
        int num = 0;
        output();
        while (1) {
            cout <<name<<"，请输入你要选择的棋盘序号(1-"<<COUNT_HOLE<<")，输入"<< (COUNT_HOLE +1)<<"退出：";
            cin >> num;
            cout <<'\x8' << '\x8'<<'\x8' << '\r';
            if (num < 1 || num>COUNT_HOLE+1) cout << "非法的输入！请输入1-7范围内的正整数" << endl;
            else if (num == COUNT_HOLE + 1) {
                cout << "确定要退出吗？退出将强行结算分数。再次输入7表示确定。"; 
                cin >> num;
                if(num== COUNT_HOLE + 1)
                    return 0;
            }
            else if (!movable(num)) cout << "该棋盘中棋子数为0，请重新选择！" << endl;
            else break;
        }
        return num;
    }
    void output() {
        //输出对方玩家基地中棋子数
        cout << "对方基地：";
        for (int i = COUNT_HOLE*2-1; i > COUNT_HOLE-1; i--)
            cout << setw(2) << pits[i]->count<<" ";
        cout << endl<<
            "积分区域：" << setw(2) << score2->count << "              " <<setw(2)<<score->count<< endl
            << "己方基地：";
        //输出当前玩家基地中棋子数
        for (int i = 0; i < COUNT_HOLE; i++)
            cout << setw(2) << pits[i]->count<<" ";
        cout << endl<<"棋洞编号：";
        for (int i = 1; i < COUNT_HOLE+1; i++)//输出格子序号
            cout << setw(2) << i<<" ";
        cout << endl;
    }
};
class AIKalahBoard :public KalahBoard {
public:
    AIKalahBoard() :KalahBoard(0) {};
    virtual int input() { return 0; }
    //计算两回合得分期望最高的值
    int calculateBestResult(int* slot) {
        short data[COUNT_HOLE * 2 + 3];
        short data2[COUNT_HOLE * 2 + 3];
        int pts[COUNT_HOLE] = {0};
        int s2=0;
        this->save(data);
        int origscore = this->getScore();
        if (!canOperate()) {//如果导致立即胜利或者立即失败，加极大期望
            if (checkWin() >= 0)
                return 1000;
            else
                return -1000;
        }
        for (int i = 0; i < COUNT_HOLE; i++) {
            this->load(data);
            if (!this->movable(i+1)) {//此格不能走子，加最大负期望
                pts[i] = -10000;
                continue;
            }

            int nxt = this->move(i+1);
            pts[i] = this->getScore() - origscore;
            if (nxt == 1) {
                pts[i] += this->calculateOrigBestResult(&s2);//如果还能继续下子，继续计算。
            }
            this->save(data2);//计算对方走子最高期望
            this->loadReverse(data2);
            pts[i] -= this->calculateOrigBestResult(&s2);//计算本操作得分综合期望
        }
        *slot = findmax(pts);
        return pts[*slot];
    }
    //计算一回合得分期望最高的值
    int calculateOrigBestResult(int* slot) {
        short data[COUNT_HOLE * 2 + 3];
        int pts[COUNT_HOLE] = {0};
        int s2=0;
        this->save(data);
        int origscore = this->getScore();
        if (!canOperate()) {
            if (checkWin() >= 0)
                return 1000;
            else
                return -1000;
        }
        for (int i = 0; i < COUNT_HOLE; i++) {
            this->load(data);
            if (!this->movable(i+1)) {
                pts[i] = -10000;
                continue;
            }
            int nxt = this->move(i+1);
            pts[i] = this->getScore() - origscore;
            if (nxt == 1) {
                pts[i] += this->calculateOrigBestResult(&s2);
            }
        }
        *slot = findmax(pts);
        return pts[*slot];
    }
    //计算最大期望值
    int findmax(int a[]) {
        int max = a[COUNT_HOLE - 1];
        int index = COUNT_HOLE - 1;
        for (int i = COUNT_HOLE - 2; i >= 0; i--) {
            if (a[i] > max) {
                max = a[i];
                index = i;
            }
        }
        return index;
    }
};
//电脑的棋盘继承通用棋盘
class ComputerKalahBoard :public KalahBoard
{
    AIKalahBoard ai;
public:
    ComputerKalahBoard(const char* name) :KalahBoard(name) {}
    //采用AI分析电脑走子
    virtual int input() {
        int sl=0;
        short data[COUNT_HOLE * 2 + 3];
        this->save(data);
        ai.load(data);
        ai.calculateBestResult(&sl);
        return sl+1;
    }

};

/** 显示规则函数 */
void show_rule();
/*主持游戏*/
void host_game(KalahBoard* players[2]) {
    bool state = rand() %2;//决定先手
    short data[COUNT_HOLE * 2 + 3];//同步游戏数据用数组
    int op;//玩家操作
    while (1) {
        int mstate;
        while (1) {
            if (!players[state]->canOperate()) {
                mstate = 2;
                break;
            }
            op = players[state]->input();
            if (op == 0) {
                mstate = 2;
                break;
            }
            mstate = players[state]->move(op);
            cout << players[state]->getName() << " 移动了洞 " << op << endl;
            if (mstate != 1)
                break;
           // system("cls");
        }
        if (mstate == 2)
            break;
        players[state]->save(data);
        //system("cls");
        
        state = !state;
        players[state]->loadReverse(data);
    }
    int result = players[state]->win();
    cout << players[state]->getName() << "得分：" << players[state]->getScore()<<endl;
    cout << players[!state]->getName() << "得分：" << players[state]->getScore2() << endl;
    if (result>0)
        cout << "恭喜"<< players[state]->getName()<<"游戏胜利" << endl;
    else if (result<0)
        cout << "恭喜"<< players[!state]->getName() <<"游戏胜利" << endl;
    else
        cout << "你们打成了平局" << endl;
    cout << "游戏结束..." << endl;
    system("pause");
    system("cls");
}
/** 单人游戏函数 */
void game_single() {
    KalahBoard* players[2]{new PlayerKalahBoard("玩家"),new ComputerKalahBoard("电脑")};
    host_game(players);
    //回收内存
    delete players[0];
    delete players[1];
};
/** 双人游戏函数 */
void game_double() {
    KalahBoard* players[2]{ new PlayerKalahBoard("玩家1"),new PlayerKalahBoard("玩家2") };
    host_game(players);
    //回收内存
    delete players[0];
    delete players[1];
};
//定义显示规则函数
void show_rule()
{
    system("cls");
    cout << endl << "\t\t游戏玩法" << endl << endl
        << "这个游戏的目的是将棋子收集到自己的基地（右侧的数即表示你基地中的棋子数）" << endl
        << "游戏时您和您的对手必须遵循这样的规则来移动棋子：" << endl
        << "1.您只可以移动自己那边的棋子" << endl
        << "2.每次移动时，您要把一个洞内的所有棋子拿起, 并以逆时针方向把棋子分发到下一个洞中" << endl
        << "3.若您移动的最后一颗棋子掉到了您的基地上，您可以再次移动" << endl
        << "4.若您的移动的最后一颗棋子掉到了自己那边的一个没有棋子的洞内，" << endl
        << " 而对面的洞有棋子时，您就可以把两个洞内的棋子都吃掉。" << endl << endl;
    system("pause");
    system("cls");
}

//主函数
int main()
{
    //定义选择变量
    int option;
    do
    {
        cout << "0.单人游戏 \n1.双人对战 \n2.游戏玩法\n3.退出游戏 "<<endl
            <<"请输入选项前面的数字并按ENTER键继续"<<endl;
        cin >> option;
        system("cls");
        switch (option)
        {
        case 0: game_single(); break;
        case 1: game_double(); break;
        case 2: show_rule(); break;
        }

    } while (option != 3);

    return 0;

}