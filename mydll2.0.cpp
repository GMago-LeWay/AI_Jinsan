#include<stdio.h>
#include"mydll.h"
#include"definition.h"
#include<cmath>
#include<algorithm>
#include<iostream>
using namespace std;

#define FREE_STATE 0
#define DEFENSE_STATE 1
#define ATTACK_STATE 2
#define TOTAL_TOWER 9
#define NUM_KINDS_SOLDIER 8
#define UPGRADE_POINT 6
#define COMMAND_LENGTH 2
#define DIVISION_OF_SOLDIER_PROPOTION 8

enum State {
	FREE, TOWER_DEFENSE, DEFENSE, ATTACK
};

/************全局储存变量***********/

unsigned int CurrentState = 0;    //当前生产状态，小于NUM_KINDS_SOLDIER时为士兵，等于NUM_KINDS_SOLDIER时为升级塔
int propotion_short[NUM_KINDS_SOLDIER] = { 0,0,1,3,0,0,1,0 };    //初期造兵造兵比例
int propotion_long[NUM_KINDS_SOLDIER] = { 0,0,1,2,0,1,1,1 };    //初期造兵造兵比例

int current_id;
vector<int> current_attack_tower;

bool first_tower_flag = true;
int last_attack_tower;  //记录上一个攻击的塔

vector<int> current_defense_tower;

int MyTowerID[TOTAL_TOWER];    //存储我方塔ID
int N_MyTowerID[TOTAL_TOWER];      //存储未占领塔ID
int SoldierNum[NUM_KINDS_SOLDIER] = { 0, };           //记录各种士兵的数量

static int index[100] = { 0, };      //短期指令执行情况


int n;         //测试flag

int data_soldier[8][6] = {//4视野范围 5攻击范围
	{ 0,0,0,0,0,0 },
{ 25,15,100,3,7,1 },
{ 35,5,80,3,7,2 },
{ 40,10,100,5,11,1 },
{ 30,20,200,2,5,3 },
{ 30,25,150,3,7,1 },
{ 45,15,100,3,7,3 },
{ 50,20,120,4,9,1 }
};

int watch[4][2][2] = {
	{ { 13,41 } ,{ 41,36 } },
{ { 41,36 },{ 36,7 } },
{ { 36,7 },{ 7,13 } },
{ { 7,13 },{ 13,41 } }
};//侦查点
int hide[4][2] = {
	{ 21,25 },
{ 27,25 },
{ 24,28 },
{ 24,23 }
};//埋伏点

Info* inf;

/*************全局储存变量结束**********/



/********基础功能函数**********************/

int min(int a, int b) {
	int c = 0;
	if (a > b) c = b;
	else c = a;
	return c;
}

double length(int a[], int dimension) {
	double sum = 0;
	for (int i = 0; i < dimension; i++)
	{
		sum += (a[i] * a[i]);
	}
	return sqrt(sum);
}

double mycos(int a[], int b[], int dimension) {
	double DotProduct = 0;
	double lengthA = length(a, dimension);
	double lengthB = length(b, dimension);
	for (int i = 0; i < dimension; i++) {
		DotProduct += (a[i] * b[i]);
	}
	return DotProduct / (lengthA*lengthB);
}
int max(int a, int b) {
	int c = 0;
	if (a > b) c = a;
	else c = b;
	return c;
}


void setzero(int a[4])
{
	for (int i = 0; i < 4; i++) {
		a[i] = -1;
	}

}

//判断地图是否越界
int flag(int a, int b) {
	int fl = 0;
	if (a <= 49 && a >= 0 && b >= 0 && b <= 49)
		fl = 1;
	return fl;
}

void AreaDecision(int radius, int scan_point[][2], int center_x, int center_y) { //确定搜索区域
	scan_point[0][0] = (center_x - radius >= 0 ? center_x - radius : 0);
	scan_point[0][1] = (center_y - radius >= 0 ? center_y - radius : 0);
	scan_point[1][0] = (scan_point[0][0] + 2 * radius <= 49 ? scan_point[0][0] + 2 * radius : 49);
	scan_point[1][1] = (scan_point[0][1] + 2 * radius <= 49 ? scan_point[0][1] + 2 * radius : 49);

}
//距离计算
int distance(int a, int b, int x, int y) {
	return abs(a - x) + abs(b - y);
}

int distance(TPoint p1, TPoint p2) {
	return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

//计算前缀平均值
double Average(double CurrentAverage, int CurrentNumber, int NewData) {
	double NewSum = CurrentAverage * CurrentNumber + NewData;
	return NewSum / (CurrentNumber + 1);
}

int Ability_TSoldierType(TSoldier s) {  //对某个场上士兵的评估
	return 10 * data_soldier[s.type][0] + 5 * data_soldier[s.type][1] + 2 * s.blood + 5 * data_soldier[s.type][3];
}


/*********基础功能函数结束*********/

/*********类的定义**********/

class Pair {

public:
	Pair(int index, int value) :index(index), value(value) {}
	Pair() {}
	int index;
	int value;

};

bool value(const Pair& p1, const Pair& p2) {
	return p1.value < p2.value;
}

class MapUnit {
public:
	MapUnit() {}
	MapUnit(TPoint t) {
		x = t.x;
		y = t.y;
	}
	MapUnit(int x, int y) :x(x), y(y) {

	}
	MapUnit(const MapUnit& map) {
		x = map.x;
		y = map.y;
	}

	bool operator==(const MapUnit& mapunit) {
		return x == mapunit.x && y == mapunit.y;
	}

	int x;
	int y;

};

class Map {
public:
	Map() {}
	Map(TPoint center, int radius = 7) {
		int scan[2][2];
		AreaDecision(radius, scan, center.x, center.y);
	}
	Map(const Map& m) {

	}

	void initialize() {}
	void route(int a, int b, int x, int y) {}

	void Dijistra() {}

	vector<MapUnit> map;
	vector<MapUnit> result;

};

class Enemy {   //对攻击目标的评估
public:
	Enemy() {

	}
	Enemy(const TSoldier& t) {
		base = t;
	}

	Enemy(const Enemy& e) {
		danger = e.danger;
		attack_possibility = e.attack_possibility;
		value = e.value;
		base = e.base;
	}

	void evaluate();     //评估

	int danger;   //危险程度
	int attack_possibility;  //攻击可能性
	int value;   //攻击价值

	TSoldier base;

};


class Troop {   //士兵
public:
	Troop() {

	}
	Troop(const TSoldier& t, unsigned int index_soldierinfo) {
		base = t;
		index = index_soldierinfo;
		view_length = data_soldier[base.type][4];
		length = inf->soldierInfo[index].move_left;  //行动力
		duty = FREE;

		point[0] = base.x_position - (view_length - 1) / 2;
		point[1] = base.y_position - (view_length - 1) / 2;

		for (unsigned int i = 0; i < COMMAND_LENGTH; i++) {
			state[i] = false;
		}
		for (int i = 0; i < 4; i++) {
			delta[i] = 0;
		}
		place[0] = base.x_position;
		place[1] = base.y_position;

		current_position[0] = base.x_position;
		current_position[1] = base.y_position;

	}
	Troop(const Troop& t) {
		base = t.base;
		index = t.index;

		length = t.length;
		view_length = t.view_length;
		duty = t.duty;

		tag = t.tag;
		signal = t.signal;
		attack_possibility = t.attack_possibility;
		defense_need = t.defense_need;

		for (int i = 0; i < 4; i++) {
			way[i] = t.way[i];
			delta[i] = t.delta[i];
			if (i < 2) {
				place[i] = t.place[i];
				current_position[i] = t.current_position[i];
				point[i] = t.point[i];
			}
		}

		for (int i = 0; i < 11; i++) {
			for (int j = 0; j < 11; j++) {
				detail[i][j] = t.detail[i][j];
			}
		}

		for (unsigned int i = 0; i < t.mytroop.size(); i++) {
			mytroop.push_back(t.mytroop[i]);
		}
		for (unsigned int i = 0; i < t.enemy.size(); i++) {
			enemy.push_back(t.enemy[i]);
		}
		for (unsigned int i = 0; i < COMMAND_LENGTH; i++) {
			state[i] = t.state[i];
		}

		attack_target = t.attack_target;
		defense_target = t.defense_target;

	}
	~Troop()
	{
		/*for (int i = 0; i < 2*+radius+1; i++)
		{
		delete[] evaluation[i];
		}
		delete[] evaluation;
		*/
	}

	void move(int length);      //移动方向，length步长
	void endplace(int x, int y);      //
	void endplace(TPoint t);
	void attackplace(int x, int y);
	void attackplace(TPoint t);
	void search(int length, int type);   //搜索地形，length视野，type地形
	void gettower();
	void go();

	void step_go(int x, int y);
	void map_go(Map&);

	void investigation();  //视野内侦查

	void evaluate();  //总评估
	void clean();
	void clean(int a);

	bool change_duty(State s);
	void march();
	void attack();    //四种指令
	void defense(int towerid);

	void act();    //总行动



	TSoldier base;
	unsigned int index;

	int view_length;         //视野边长
	int length;              //行动力

	State duty;

	int attack_possibility;      //攻击可能性
	int defense_need;            //防守需要
	int tag = 0;                   //遍历标签
	int signal;             //我方敌方数量差

	int way[4];       //x方向走左右+距离+y方向走左右+距离
	int place[2];     //目标点
	int delta[4];      //坐标更新量

	int current_position[2];

	int detail[11][11]; //记录每个点的占据情况
	int point[2];        //视野左下角坐标

	vector<Troop> mytroop;
	vector<Enemy> enemy;

	TPoint attack_target;        //攻击目标
	TPoint defense_target;       //防守方向

	bool state[COMMAND_LENGTH];

	int radius = 5;



};

class Tower {                //对塔的评估
public:
	Tower(const TowerInfo& t) {
		base = t;
		sum_accessible = 0;
		sum_convenience = 0;
		sum_danger = 0;
		sum_troop_convenience = 0;

		length = 2 * t.level + 5;
		point[0] = base.x_position - (length - 1) / 2;
		point[1] = base.y_position - (length - 1) / 2;

		//获取周围视野
		for (int i = 0; i < length; i++) {
			for (int j = 0; j < length; j++) {
				if (flag(point[0] + i, point[1] + j))
					detail[i][j] = inf->pointInfo[point[0] + i][point[1] + j].occupied_type;
				else
					detail[i][j] = -1;
				if (detail[i][j] == 1) {
					int soldier_id = inf->pointInfo[point[0] + i][point[1] + j].soldier;
					//遍历寻找所占士兵
					for (unsigned int k = 0; k < inf->soldierInfo.size(); k++) {
						if (inf->soldierInfo[k].id == soldier_id) {  //找到士兵

							if (inf->soldierInfo[k].owner == current_id) {
								//cout << "开始push自己的士兵" << endl;
								this->mytroop.push_back(Troop(inf->soldierInfo[k], k));
								//cout << "结束push自己的士兵" << endl;
							}
							else {
								this->enemy.push_back(Enemy(inf->soldierInfo[k]));
							}

						}
					}
				}
			}
		}
		/*cout << "塔" << base.id << " 拥有者" << base.owner << " 我方士兵数量" << mytroop.size() << " 敌方士兵数量" << enemy.size();
		cout << " 我方士兵";
		for (unsigned int i = 0; i < mytroop.size(); i++) {
		cout << mytroop[i].base.id << " ";
		}

		cout << " 敌方士兵";
		for (unsigned int i = 0; i < enemy.size(); i++) {
		cout << enemy[i].base.id << " ";
		}
		cout << endl;*/

	}
	Tower() {}
	Tower(const Tower& t) {
		danger = t.danger;
		accessible = t.accessible;
		convenience = t.convenience;
		troop_convenience = t.troop_convenience;

		attack_evaluation = t.attack_evaluation;
		product_evaluation = t.product_evaluation;
	

		for (int i = 0; i < 11; i++) {
			for (int j = 0; j < 11; j++) {
				detail[i][j] = t.detail[i][j];
			}
		}
		for (int i = 0; i < 2; i++) {
			point[i] = t.point[i];
		}
		for (unsigned int i = 0; i < t.mytroop.size(); i++) {
			mytroop.push_back(t.mytroop[i]);
		}
		for (unsigned int i = 0; i < t.enemy.size(); i++) {
			enemy.push_back(t.enemy[i]);
		}

		length = t.length;
		base = t.base;
	}

	void evaluate_danger(vector<Enemy> enemy, vector<Troop> my);        //计算危险性
	void evaluate_accessible();          //计算可接近性
	void evaluate_convenience();
	void evaluate_troop_convenience(vector<Troop> my);      //用到我方士兵距离之和评价的便利度
	void evaluate_all(vector<Enemy> enemy, vector<Troop> my);   //评估函数

	unsigned int basicneed();
	unsigned int is_dangerous();

	void defense(vector<Troop>& total_troop);
	void evaluate(vector<Enemy> enemy, vector<Troop> my);   //两种指令

	void static_generate();         //利用静态变量的激活函数

	void act();   //行动

	static int sum_accessible;
	static int sum_convenience;
	static int sum_danger;
	static int sum_troop_convenience;

	int detail[11][11];
	int point[2];
	int length;                     //塔的视野
	vector<Troop> mytroop;
	vector<Enemy> enemy;

	int danger;               //表示危险程度，正为敌方控制，负为我方控制
	int accessible;           //可接近程度
	int convenience;          //攻塔便利度
	int troop_convenience;         //攻塔便利度，用士兵距离之和衡量

	int soldierneed = 0;

								   /*******外部修改区*****/
	double attack_evaluation;   //可攻击性评估，越小越容易
	double product_evaluation;   //生产性评估，越小越方便


								 /******外部修改区结束***************/

	TowerInfo base;


};

bool attack_evaluate(const Tower& t1, const Tower& t2) {
	return t1.attack_evaluation < t2.attack_evaluation;
}
bool product_evaluate(const Tower t1, const Tower& t2) {
	return t1.product_evaluation < t2.product_evaluation;
}

bool high_danger(const Tower& t1, const Tower& t2) {
	return t1.danger > t2.danger;
}

class Properties {  //我方属性
public:

	Properties(Info& info) :EnemyEvaluation(4) {  //构造函数
												  //清空当前士兵
		for (int i = 0; i < NUM_KINDS_SOLDIER; i++) {
			SoldierNum[i] = 0;
		}

		int my_id = info.myID;
		current_id = my_id;

		for (int i = 0; i < 4; i++) {               //获取各方属性信息
			if (info.playerInfo[i].id == my_id) {
				attack = info.playerInfo[i].population * 10 + info.playerInfo[i].tower_num * 50 + info.playerInfo[i].resource * 1;
				EnemyEvaluation[i] = -1;
			}
			else {
				EnemyEvaluation[i] = info.playerInfo[i].population * 10 + info.playerInfo[i].tower_num * 50 + info.playerInfo[i].resource * 1;
			}
		}

		//指针指向正确
		inf = &info;

		//获取我方塔与非我方塔信息
		int MyTowerID_iter = 0;
		int N_MyTowerID_iter = 0;
		for (int i = 0; i < TOTAL_TOWER; i++) {
			if (inf->towerInfo[i].owner == my_id) {
				MyTowerID[MyTowerID_iter] = inf->towerInfo[i].id;  //将MytowerID填入我方塔id
				MyTowerID_iter++;
				//cout << "我方塔" << MyTowerID[MyTowerID_iter - 1] << endl;

			}
			else {
				N_MyTowerID[N_MyTowerID_iter] = inf->towerInfo[i].id;
				N_MyTowerID_iter++;
				//cout << "非我方塔" << N_MyTowerID[N_MyTowerID_iter - 1] << endl;

			}
			TowerInf.push_back(inf->towerInfo[i]);      //信息复制

		}
		while (MyTowerID_iter<TOTAL_TOWER)                 //空位用-1填充
		{
			MyTowerID[MyTowerID_iter] = -1;
			MyTowerID_iter++;
		}
		while (N_MyTowerID_iter<TOTAL_TOWER)
		{
			N_MyTowerID[N_MyTowerID_iter] = -1;
			N_MyTowerID_iter++;
		}

		//获取我方与敌方士兵信息

		for (unsigned int i = 0; i < inf->soldierInfo.size(); i++) {
			if (inf->soldierInfo[i].owner == my_id) {
				MyTroop.push_back(Troop(inf->soldierInfo[i], i));
				SoldierNum[inf->soldierInfo[i].type]++;  //计数
			}
			else {
				EnemyTroop.push_back(inf->soldierInfo[i]);
			}
		}

	}


	double attack;       //攻击力
	double defense;          //防御力
	int swift;         //移动能力
	int demolition;       //拆塔能力

	vector<double> EnemyEvaluation;    //敌人评估值
	vector<Tower> TowerInf;
	vector<Troop> MyTroop;
	vector<Enemy> EnemyTroop;


};


class Decision {      //决策
public:

	Decision() {}
	Decision(Properties* p) {
		data = p;
	}

	int findorigin(int soldierid);  //通过士兵id找到我的士兵的下标
	bool in_list(int id, vector<int> v);         //判断id是否在列表v中

	void shortterm();
	int get_duty_num(State s);   //获取s状态的士兵总数
	void setfree(State s);

	void analyse_tower();
	void analyse_troop();

	void command_tower();   //指挥塔
	void command_troop();   //指挥兵

	void clean();
	void product();
	void defense();
	void attack();    //三种命令方式


	void command();    //总命令

	int AttackWeigh;
	int DefenseWeigh;

	vector<Tower> unoccupied;           //已占领列表
	vector<Tower> occupied;             //未占领列表


	Properties* data;


};


/*************类的静态变量的初始化***************/
int Tower::sum_accessible = 0;
int Tower::sum_convenience = 0;
int Tower::sum_danger = 0;
int Tower::sum_troop_convenience = 0;

/**************类静态变量初始化完毕****************/

/*
基本的四个添加指令的命令
info.myCommandList.addCommand(Produce,aim_tower_id,HeavyArcher);//造兵命令，第二个参数是造兵的塔id，第三个是造的兵种
info.myCommandList.addCommand(Attack,aim_soldier_id,x_position,y_position);//攻击命令，第二个参数是发起攻击的士兵id，第三和第四个参数是目标的x，y位置
info.myCommandList.addCommand(Upgrade,aim_tower_id);//升级命令，第二个参数是欲升级的塔id
info.myCommandList.addCommand(Move,aim_soldier_id,UP,distance);//移动命令，第二个参数是欲移动的士兵id，第三个参数是移动方向，第四个参数是移动距离
*/
void player_ai(Info& info)
{
	//cout << "Round " << info.round << " start." << endl << endl;

	Properties MyProperties(info);

	//cout << "初始化完成Round" << info.round << endl;

	//cin >> n;
	//初始化完成
	Decision MyDecision(&MyProperties);
	MyDecision.command();
	/*
	cout << "Attacksoldier" << AttackSoldier.size() << " DefenseSoldier" << DefenseSoldier.size() << endl;
	for (unsigned int i = 0; i < AttackSoldier.size(); i++) {
		cout << AttackSoldier[i] << " ";
	}
	cout << endl;
	for (unsigned int i = 0; i < DefenseSoldier.size(); i++) {
		cout << DefenseSoldier[i] << " ";
	}
	cout << endl;
	*/
	cout << "Round " << info.round << " end." << endl << endl;
	//cin >> n;
	
}

/***********Enemy成员函数定义************/

void Enemy::evaluate() {

}

/*********Enemy成员函数定义结束*******/


/*************Tower成员函数定义*****************************/

void Tower::evaluate_danger(vector<Enemy> enemy, vector<Troop> my) {
	int scan[2][2];
	//确定搜索区域
	AreaDecision(3, scan, base.x_position, base.y_position);
	danger = 1;  //初始化


	for (unsigned int i = 0; i < enemy.size(); i++) {
		if (enemy[i].base.x_position >= scan[0][0] && enemy[i].base.x_position <= scan[1][0]
			&& enemy[i].base.y_position >= scan[0][1] && enemy[i].base.y_position <= scan[1][1]) {  //如果敌人在范围内
			danger += Ability_TSoldierType(enemy[i].base);

		}
	}
	for (unsigned int i = 0; i < my.size(); i++) {
		if (my[i].base.x_position >= scan[0][0] && my[i].base.x_position <= scan[1][0]
			&& my[i].base.y_position >= scan[0][1] && my[i].base.y_position <= scan[1][1]) {  //如果我方在范围内
			danger -= Ability_TSoldierType(my[i].base);

		}
	}

	Tower::sum_danger += abs(danger);


}

void Tower::evaluate_accessible() {
	accessible = 0;
	for (int i = 0; i < inf->playerInfo[current_id].tower_num; i++) {

		accessible += distance(inf->towerInfo[MyTowerID[i]].position, base.position);

	}
	Tower::sum_accessible += accessible;
	//cout << Tower::sum_accessible << " " << accessible << "sum_accessible plused" << endl;
}

void Tower::evaluate_convenience() {
	convenience = 0;
	for (int i = 0; i < TOTAL_TOWER - inf->playerInfo[current_id].tower_num; i++) {

		convenience += distance(inf->towerInfo[N_MyTowerID[i]].position, base.position);


	}
	Tower::sum_convenience += convenience;
	//cout << Tower::sum_convenience << " " << convenience << "sum_convenience plused" << endl;
}

void Tower::evaluate_troop_convenience(vector<Troop> t) {
	troop_convenience = 0;
	for (unsigned int i = 0; i < t.size(); i++) {

		troop_convenience += distance(t[i].base.position, base.position);

	}
	Tower::sum_troop_convenience += troop_convenience;
}

void Tower::evaluate_all(vector<Enemy> enemy, vector<Troop> my) {
	evaluate_danger(enemy, my);
	evaluate_accessible();
	evaluate_convenience();
	evaluate_troop_convenience(my);
	//cout << "Tower" << base.id << " Accessible" << accessible << " Convenience" << convenience <<" Danger"<< danger << endl;

}

unsigned int Tower::basicneed() {
	int need = 0;
	if (inf->round < 35) {
		if (enemy.size()) {
			need = enemy.size() + 1;
		}
		else {
			need = 0;
		}
	}
	else {
		if (enemy.size()) {
			need = enemy.size() + 2;
		}
		else {
			if (base.id == 0 || base.id == 6 || base.id == 4 || base.id == 2) {
				need = 0;
			}
			else if (base.id == 1 || base.id == 7 || base.id == 5 || base.id == 3) {
				need = 1;
			}
			else {
				need = 2;
			}
		}
	}
	if (need <= 0)
		return 0;
	else
		return need;
}

unsigned int Tower::is_dangerous() {  //返回需要士兵数量
	int need = 0;
	if (inf->round < 35) {
		if (enemy.size()) {
			need = enemy.size() - mytroop.size() + 1;
		}
		else {
			need = 0;
		}
	}
	else {
		if (enemy.size()) {
			need = enemy.size() - mytroop.size() + 2;
		}
		else {
			if (base.id == 0 || base.id == 6 || base.id == 4 || base.id == 2) {
				need = 0;
			}
			else if (base.id == 1 || base.id == 7 || base.id == 5 || base.id == 3) {
				need = 1 - mytroop.size();
			}
			else {
				need = 2 - mytroop.size();
			}
		}
	}
	if (need <= 0)
		return 0;
	else
		return need;

}

void Tower::defense(vector<Troop>& total_troop) {

	if (this->enemy.size() != 0) {
		int k = 0;
		for (unsigned int i = 0; i < mytroop.size(); i++) {
			if (enemy[k].base.blood <= 0) k++;   //按顺序攻击

			if (distance(mytroop[i].base.position, enemy[k].base.position) > mytroop[i].base.range) {
				mytroop[i].attackplace(enemy[k].base.position);
				if (mytroop[i].base.x_position >= point[0] &&
					mytroop[i].base.y_position >= point[1] &&
					mytroop[i].base.x_position <= point[0] + length &&
					mytroop[i].base.y_position <= point[1] + length) {
					mytroop[i].go();
					mytroop[i].clean();
					for (unsigned int j = 0; j < total_troop.size(); j++) {
						if (total_troop[j].base.id == mytroop[i].base.id) {
							total_troop[j].duty = TOWER_DEFENSE;
							break;
						}
					}
				}
			}
			if (distance(mytroop[i].base.position, enemy[k].base.position) <= mytroop[i].base.range)
				inf->myCommandList.addCommand(Attack, mytroop[i].base.id, enemy[k].base.x_position, enemy[k].base.y_position);

		}
	}

}

void Tower::evaluate(vector<Enemy> enemy, vector<Troop> my) {
	evaluate_all(enemy, my);

	if (base.owner == current_id) {


	}
}

void Tower::act() {}

void Tower::static_generate() {
	double normalizer_accessible = Tower::sum_accessible / TOTAL_TOWER;
	double normalizer_danger = Tower::sum_danger / TOTAL_TOWER;
	double normalizer_convenience = Tower::sum_convenience / TOTAL_TOWER;
	double normalizer_troop_convenience = Tower::sum_troop_convenience / TOTAL_TOWER;
	double normalizer_blood = 300;

	//cout <<"正则化因子"<< normalizer_accessible << " " << normalizer_danger << " " << normalizer_convenience;

	double weigh_accessible = 1;
	double weigh_danger = 2;
	double weigh_convenience = 2;
	double weigh_troop_convenience = 2;
	double weigh_blood = 2;


	attack_evaluation = weigh_accessible * accessible / normalizer_accessible + weigh_danger * danger / normalizer_danger + weigh_troop_convenience * troop_convenience / normalizer_troop_convenience + weigh_blood * base.blood / normalizer_blood;
	product_evaluation = weigh_convenience * convenience / normalizer_convenience;

	//cout << "塔" << base.id << "攻击评估:" << attack_evaluation << endl;

}

/*************Tower成员函数定义结束*****************************/


/*************Troop成员函数定义*************/


void Troop::move(int length) {
	int a = base.x_position;
	int b = base.y_position;

	int x = place[0];
	int y = place[1];

	setzero(way);
	if (a != x) {
		if (a < x) way[0] = 3;
		if (a > x) way[0] = 2;
		if (abs(x - a) <length) way[1] = abs(x - a);
		if (abs(x - a) >= length) way[1] = length;
	}
	if (b != y) {
		if (b < y) way[2] = 0;
		if (b > y) way[2] = 1;
		if (abs(y - b) <length) way[3] = abs(y - b);
		if (abs(y - b) >= length) way[3] = length;
	}


}

void Troop::endplace(int x, int y) {
	int a = base.x_position;
	int b = base.y_position;
	int d = distance(a, b, x, y);
	if (x == 24 && y == 25 && d > 5) {
		place[0] = 24; place[1] = 25;
	}

	else {
		int min = 10000;
		place[0] = x - 1;
		place[1] = y - 2;
		for (int i = 0; i < 5; i = i + 4) {
			for (int j = 0; j < 3; j++) {
				if (distance(a, b, x + j - 1, y + i - 2) < min) {
					if (distance(a, b, x + j - 1, y + i - 2) > 0 && inf->pointInfo[x + j - 1][y + i - 2].occupied_type != 0) { ; }
					else {
						place[0] = x + j - 1;
						place[1] = y + i - 2;
						min = distance(a, b, x + j - 1, y + i - 2);
					}
				}
			}
		}
		for (int i = 0; i < 5; i = i + 4) {
			for (int j = 0; j < 3; j++) {
				if (distance(a, b, x + i - 2, y + j - 1) < min) {
					if (distance(a, b, x + i - 2, y + j - 1) > 0 && inf->pointInfo[x + i - 2][y + j - 1].occupied_type != 0) { ; }
					else {
						place[0] = x + i - 2;
						place[1] = y + j - 1;
						min = distance(a, b, x + i - 2, y + j - 1);
					}
				}
			}
		}
	}
}

void Troop::endplace(TPoint t) {
	endplace(t.x, t.y);
}

void Troop::attackplace(int x, int y) {
	int a = base.x_position;
	int b = base.y_position;

	length = base.range;

	int min = 1000000;
	place[0] = a; place[1] = b;
	if (flag(x + length, y) && distance(a, b, x + length, y) < min
		&& inf->pointInfo[x + length][y].occupied_type == 0
		&& inf->pointInfo[x + length][y].land != 2) {
		place[0] = x + length; place[1] = y;
		min = distance(a, b, x + length, y);
	}
	if (flag(x - length, y) && distance(a, b, x - length, y) < min
		&& inf->pointInfo[x - length][y].occupied_type == 0
		&& inf->pointInfo[x - length][y].land != 2) {
		place[0] = x - length; place[1] = y;
		min = distance(a, b, x - length, y);
	}
	if (flag(x, y + length) && distance(a, b, x, y + length) < min
		&& inf->pointInfo[x][y + length].occupied_type == 0
		&& inf->pointInfo[x][y + length].land != 2) {
		place[0] = x; place[1] = y + length;
		min = distance(a, b, x, y + length);
	}
	if (flag(x, y - length) && distance(a, b, x, y - length) < min
		&& inf->pointInfo[x][y - length].occupied_type == 0
		&& inf->pointInfo[x][y - length].land != 2) {
		place[0] = x; place[1] = y - length;
		min = distance(a, b, x, y - length);
	}

}

void Troop::attackplace(TPoint t) {
	attackplace(t.x, t.y);
}

void Troop::search(int length, int type) {
	int a = base.x_position;
	int b = base.y_position;

	int min = 100000, c = a - (length - 1) / 2, d = b - (length - 1) / 2;
	place[0] = a; place[1] = b;
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < length; j++) {
			if (flag(c + i, d + j)) {
				if (inf->pointInfo[c + i][d + j].land == type && distance(a, b, c + i, d + j) < min) {
					min = distance(a, b, c + i, d + j);
					place[0] = c + i; place[1] = d + j;
				}
			}
		}
	}
}

void Troop::investigation() {
	//获取周围视野
	for (int i = 0; i < view_length; i++) {
		for (int j = 0; j < view_length; j++) {
			if (flag(point[0] + i, point[1] + j))
				detail[i][j] = inf->pointInfo[point[0] + i][point[1] + j].occupied_type;
			else
				detail[i][j] = -1;
			if (detail[i][j] == 1) {
				int soldier_id = inf->pointInfo[point[0] + i][point[1] + j].soldier;
				//遍历寻找所占士兵
				for (unsigned int k = 0; k < inf->soldierInfo.size(); k++) {
					if (inf->soldierInfo[k].id == soldier_id) {  //找到士兵

						if (inf->soldierInfo[k].owner == current_id) {
							this->mytroop.push_back(Troop(inf->soldierInfo[k], k));

						}
						else {
							this->enemy.push_back(Enemy(inf->soldierInfo[k]));
						}

					}
				}
			}
		}
	}


}

void Troop::evaluate() {
	investigation();
}

void Troop::gettower() {

	if (base.attackable) {
		int x = base.x_position - base.range, y = base.y_position - base.range;
		for (int i = 0; i < 2 * base.range + 1; i++)
			for (int j = 0; j < 2 * base.range + 1; j++)
			{
				int d = distance(base.x_position, base.y_position, x + i, y + j);
				if (inf->pointInfo[x + i][y + j].occupied_type == 2 && d <= base.range && base.attackable)
					inf->myCommandList.addCommand(Attack, base.id, x + i, y + j);
			}
		/*int z = current_p[0] - 1, x = current_p[0] + 1, c = current_p[1] - 1, v = current_p[1] + 1;
		if (inf->pointInfo[current_p[0]][current_p[1] + 1].occupied_type == 2)
		inf->myCommandList.addCommand(Attack, id, current_p[0], v);
		if (inf->pointInfo[current_p[0]][current_p[1] - 1].occupied_type == 2)
		inf->myCommandList.addCommand(Attack, id, current_p[0], c);
		if (inf->pointInfo[current_p[0] + 1][current_p[1]].occupied_type == 2)
		inf->myCommandList.addCommand(Attack, id, x, current_p[1]);
		if (inf->pointInfo[current_p[0] - 1][current_p[1]].occupied_type == 2)
		inf->myCommandList.addCommand(Attack, id, z, current_p[1]);*/
	}
}

void Troop::go() {
	//int apojo;
	//cin >> apojo;
	move(base.move_left);
	//cout << "Go 1" << endl;
	if (way[2] != -1 || way[0] != -1 && base.move_left<6) {
		if (way[0] != -1 && way[2] != -1)
		{
			int x = 0, y = 0, m = 0;
			int left = base.move_left;
			if (way[1] + way[3] > left) {
				for (int len = left; len > 0; len--) {
					for (int i = 0; i < len + 1; i++) {
						if (m == 0) {
							int a = base.x_position + i * (2 * way[0] - 5), b = base.y_position + (len - i) * (1 - 2 * way[2]);
							if (flag(a, b) != 0) {
								if (inf->pointInfo[a][b].occupied_type == 0 && inf->pointInfo[a][b].land != 2) {
									if (base.type != 3 && base.type != 7 && i <= way[1] && len - i <= way[3]) { x = i; y = len - i; m = 1; }
									else {
										if (inf->pointInfo[a][b].land != 3 && i <= way[1] && len - i <= way[3]) {
											x = i; y = len - i; m = 1;
										}
									}
								}
							}
						}
					}
				}
			}
			if (way[1] + way[3] <= left) { x = way[1], y = way[3]; }
			if (x != 0 || y != 0)
			{
				if (inf->pointInfo[base.x_position + x * (2 * way[0] - 5)][base.y_position].occupied_type == 0) {
					inf->myCommandList.addCommand(Move, base.id, way[0], x); delta[way[0]] = x;
					inf->myCommandList.addCommand(Move, base.id, way[2], y); delta[way[2]] = y;
					//cout << "id" << " " << base.id << " " << "up down" << endl;

				}
				else {
					inf->myCommandList.addCommand(Move, base.id, way[2], y); delta[way[2]] = y;
					inf->myCommandList.addCommand(Move, base.id, way[0], x); delta[way[0]] = x;
					//cout << "id" << " " << base.id << " " << "down up" << endl;
				}
			}
			else {
				inf->myCommandList.addCommand(Move, base.id, way[0], way[1]); delta[way[0]] = way[1];
				//cout << "id" <<" "<< base.id  << " " << "only ver" << endl;
			}
		}


		else if (way[2] == -1) {
			int fla = 0;
			for (int i = 1; i < way[1] + 1; i++) {
				int x = base.x_position + i * (2 * way[0] - 5), y = base.y_position;
				if (inf->pointInfo[x][y].occupied_type == 0 && inf->pointInfo[x][y].land != 2)
					fla = i;
			}
			if (fla != 0) {
				inf->myCommandList.addCommand(Move, base.id, way[0], fla);
				//cout << "id" << " " << base.id << " " << "need hor" << endl;
				delta[way[0]] = fla;
			}
			else {
				int fle = 0;
				for (int i = -base.move_left; i <= base.move_left; i++) {
					int x = base.x_position + 2 * way[0] - 5, y = base.y_position + i;
					if (inf->pointInfo[x][y].occupied_type == 0 && inf->pointInfo[x][y].land != 2) {
						if (i < 0) {
							inf->myCommandList.addCommand(Move, base.id, 1, -i);
							//cout << "id" << " " << base.id << " " << "extr down" << endl;
							; delta[1] = -i;
							for (int j = 0; j <= base.move_left + i; j++)
							{
								inf->myCommandList.addCommand(Move, base.id, way[0], 1);
								//cout << "id" << " " << base.id << " " << "ver one" << endl;
								delta[way[0]] ++;
							}
							fle = 1;
						}
						else {
							inf->myCommandList.addCommand(Move, base.id, 0, i);
							//cout << "id" << " " << base.id << " " << "extr up" << endl;
							delta[0] = i;
							for (int j = 0; j <= base.move_left - i; j++) {
								inf->myCommandList.addCommand(Move, base.id, way[0], 1);
								//cout << "id" << " " << base.id << " " << "ver one" << endl;
								delta[way[0]]++;
							}
							fle = 1;
						}
						break;
					}
				}
				if (fle == 0) {
					inf->myCommandList.addCommand(Move, base.id, way[0], way[1]);
					//cout << "id" << " " << base.id << " " << "ver run" << endl;
					delta[way[0]] = way[1];
				}
			}
		}
		else if (way[0] = -1) {
			int laf = 0;
			for (int i = 1; i <= way[3]; i++) {
				int x = base.x_position, y = base.y_position + i * (1 - 2 * way[2]);
				if (inf->pointInfo[x][y].occupied_type == 0 && inf->pointInfo[x][y].land != 2)  laf = i;
			}
			if (laf != 0) {
				inf->myCommandList.addCommand(Move, base.id, way[2], laf);
				//cout << "id" << " " << base.id << " " << "need hor" << endl;
				delta[way[2]] = laf;
			}
			else {
				int  lafq = 0;
				for (int i = -base.move_left; i <= base.move_left; i++) {
					int x = base.x_position + i, y = base.y_position + 1 - 2 * way[2];
					if (inf->pointInfo[x][y].occupied_type == 0 && inf->pointInfo[x][y].land != 2) {
						if (i < 0) {
							inf->myCommandList.addCommand(Move, base.id, 2, -i);
							//cout << "id" << " " << base.id << " " << "extr left" << endl;
							delta[2] = -i;
							for (int j = 0; j < base.move_left + i; j++)
							{
								inf->myCommandList.addCommand(Move, base.id, way[2], 1);
								//cout << "id" << " " << base.id << " " << "hor one" << endl;
								delta[way[2]]++;
							}
							lafq = 1;
						}
						else {
							inf->myCommandList.addCommand(Move, base.id, 3, i);
							//cout << "id" << " " << base.id << " " << "extr right" << endl;
							delta[3] = i;
							for (int j = 0; j < base.move_left - i; j++) {
								inf->myCommandList.addCommand(Move, base.id, way[2], 1);
								//cout << "id" << " " << base.id << " " << "hor one" << endl;
								delta[way[2]]++;
							}

							lafq = 1;
						}
						break;
					}
				}
				if (lafq == 0) {
					inf->myCommandList.addCommand(Move, base.id, way[2], way[3]);
					//cout << "id" << " " << base.id << " " << "hor run" << endl;
					delta[way[2]] = way[3];
				}
			}
		}
	}
	clean(1);
}

void Troop::step_go(int x, int y) {
	if (distance(x, y, current_position[0], current_position[1]) != 1) {  //发生错误
		return;
	}
	if (current_position[0] != x) {
		int delta_x = x - current_position[0];
		int direction = (delta_x + 5) / 2;                //-1,1->2LEFT, 3RIGHT
		inf->myCommandList.addCommand(Move, base.id, direction, 1);
		current_position[0] = x;
	}
	else {
		int delta_y = y - current_position[1];
		int direction = (delta_y - 1) / (-2);                //-1,1->1DOWN, 0UP
		inf->myCommandList.addCommand(Move, base.id, direction, 1);
		current_position[1] = y;
	}
}

void Troop::map_go(Map& m) {

	for (unsigned int i = 1; i < m.result.size() && i <= base.move_left; i++) {
		step_go(m.result[i].x, m.result[i].y);

	}
}

void Troop::clean() {
	if (base.attackable) {
		int x = base.x_position - base.range, y = base.y_position - base.range;
		for (int i = 0; i < 2 * base.range + 1; i++)
			for (int j = 0; j < 2 * base.range + 1; j++)
			{
				{
					if (flag(x + i, y + j) != 0) {
						if (inf->pointInfo[x + i][y + j].occupied_type == 1) {
							int belong = 0;
							for (unsigned int k = 0; k < mytroop.size(); k++)
								if (inf->pointInfo[x + i][y + j].soldier == mytroop[k].base.id) {
									belong = 1;
								}
							if (belong == 0)
								inf->myCommandList.addCommand(Attack, base.id, x + i, y + j);
						}
						if (inf->pointInfo[x + i][y + j].occupied_type == 2 && base.attackable)
							inf->myCommandList.addCommand(Attack, base.id, x + i, y + j);

					}
				}
			}
	}
}

void Troop::clean(int a) {
	if (base.attackable) {
		int x = base.x_position - base.range - base.move_left, y = base.y_position - base.range - base.move_left;
		for (int i = 0; i < 2 * (base.range + base.move_left) + 1; i++)
			for (int j = 0; j < 2 * (base.range + base.move_left) + 1; j++)
			{
				if (flag(x + i, y + j) != 0) {
					if (inf->pointInfo[x + i][y + j].occupied_type == 1) {
						int belong = 0;
						for (unsigned int k = 0; k<mytroop.size(); k++)
							if (inf->pointInfo[x + i][y + j].soldier == mytroop[k].base.id) {
								belong = 1;
							}
						if (belong == 0)
							inf->myCommandList.addCommand(Attack, base.id, x + i, y + j);
					}
					if (inf->pointInfo[x + i][y + j].occupied_type == 2 && base.attackable)
						inf->myCommandList.addCommand(Attack, base.id, x + i, y + j);
				}
			}
	}
}

bool Troop::change_duty(State s) {
	if (duty == FREE) {
		duty = s;
		return true;
	}
	else{
		return false;
	}
}

void Troop::march() {

}

void Troop::attack() {
	if (current_attack_tower.size()) {
		endplace(inf->towerInfo[current_attack_tower[0]].position);
		int d = distance(base.x_position, base.y_position, place[0], place[1]);
		/*if (base.x_position == place[0] && base.y_position == place[1]) {
		gettower();
		}*/

		if (d < base.range) gettower();
		else go();
	}
}

void Troop::defense(int towerid) {
	//cout << "Check 4.X.X.start" << endl;
	if (duty == DEFENSE) {
		//cout << "Check 4.X.X.1" << endl;
		endplace(inf->towerInfo[towerid].position);
		//cout << "Check 4.X.X.2" << endl;
		go();
		//cout << "Check 4.X.X.3" << endl;
	}
	else {
		//cout << "士兵" << base.id << "被塔占用，不能进行回防" << endl;
	}
	//cout << "Check 4.X.X.end" << endl;
}

void Troop::act() {
	evaluate();

}

/*************Troop成员函数定义结束*************/



/**************Decision成员函数定义*******************/

int Decision::findorigin(int soldierid) {  //通过士兵id找到我的士兵的下标
	int returnid = -1;                             //越界警告
	for (unsigned int i = 0; i < data->MyTroop.size(); i++) {
		if (data->MyTroop[i].base.id == soldierid)
		{
			returnid = i;
			break;
		}
	}

	return returnid;
}

bool Decision::in_list(int id, vector<int> v) {
	bool return_flag = false;
	for (unsigned int i = 0; i < v.size(); i++) {
		if (id == v[i]) {
			return_flag = true;
			break;
		}
	}
	return return_flag;
}

int Decision::get_duty_num(State s) {
	int sum = 0;
	for (unsigned int i = 0; i < data->MyTroop.size(); i++) {
		if (data->MyTroop[i].duty == s) {
			sum++;
		}
	}
	return sum;
}

void Decision::setfree(State s) {
	for (unsigned int i = 0; i < data->MyTroop.size(); i++) {
		if (data->MyTroop[i].duty == s) {
			data->MyTroop[i].duty = FREE;
		}
	}
}


void Decision::defense() {

	//cout << "Check 4.1" << endl;
	bool flag = true;
	if (get_duty_num(DEFENSE) > 0) {  //判断是否达到目标
		for (unsigned int i = 0; i < occupied.size(); i++) {
			if (inf->round <= 50) {
				if (occupied[i].mytroop.size() - occupied[i].enemy.size() < 0) {
					flag = false;
				}
			}
			else {
				if (occupied[i].mytroop.size() - occupied[i].enemy.size() < 1) { //我方士兵比敌方士兵多一个
					flag = false;
				}
			}
		}
		if (flag) {  //如果已经安全，抹去所有防御标记
			setfree(DEFENSE);
		}
	}

	//cout << "Check 4.2" << endl;

	vector<Tower> defense_list;
	int sum_soldier_need = 0;
	for (int i = 0; i < TOTAL_TOWER; i++)
	{
		if (data->TowerInf[i].base.owner == current_id) {
			if (data->TowerInf[i].is_dangerous()) {  
				defense_list.push_back(data->TowerInf[i]);
				defense_list[defense_list.size() - 1].soldierneed = data->TowerInf[i].is_dangerous();
				sum_soldier_need += data->TowerInf[i].is_dangerous();
			}
		}
	}

	//cout << "Check 4.3" << endl;

	sort(defense_list.begin(), defense_list.end(), high_danger);
	for (unsigned int i = 0; i < defense_list.size(); i++) {
		current_defense_tower.push_back(defense_list[i].base.id);
	}
	
	//cout << "Check 4.4" << endl;

	if (current_defense_tower.size()) {    //如果有需要回防的兵塔

		if (get_duty_num(DEFENSE)) {
			for (unsigned int i = 0; i < data->MyTroop.size(); i++) {
				//cout << "Check 4.1.1." << i << endl;
				data->MyTroop[i].defense(current_defense_tower[0]);             //暂时只优先回防一个塔
				//cout << "当前既有回防士兵" << findorigin(DefenseSoldier[0]) << endl;
				//cout << "总回防士兵" << DefenseSoldier.size() << endl;
			}
		}

		//cout << "Check 4.5" << endl;

		//bool find_free_soldier_flag = false;
		bool safe_flag = false;

		vector<Pair> id_distance;   //index储存士兵id，value储存距离
		for (unsigned int i = 0; i < data->MyTroop.size(); i++) { //找到零散士兵让其回防,储存在id_distance内
			if (data->MyTroop[i].duty == FREE) {
				id_distance.push_back(Pair(data->MyTroop[i].base.id,
					distance(data->MyTroop[i].base.position, data->TowerInf[current_defense_tower[0]].base.position)));

				//find_free_soldier_flag = true;
				//cout << "零散士兵" << MyTroop[i].base.id << "出发回防" << endl;

			}
		}
		sort(id_distance.begin(), id_distance.end(), value);

		//cout << "Check 4.6" << endl;

		for (unsigned int i = 0; i < id_distance.size() && get_duty_num(DEFENSE)<sum_soldier_need; i++) {
			data->MyTroop[findorigin(id_distance[i].index)].duty = DEFENSE;  //自由士兵防守

			data->MyTroop[findorigin(id_distance[i].index)].defense(current_defense_tower[0]);     //暂时只防守一座塔
			if (get_duty_num(DEFENSE) >= sum_soldier_need) {
				safe_flag = true;
				break;
			}
		}

		//cout << "Check 4.7" << endl;


	


	}
}

void Decision::attack() {


	if (current_attack_tower.size() == 0)
		return;

	if (last_attack_tower != current_attack_tower[0]) {  //如果更换了攻击目标，可能是塔已经被攻克，证明需要重新规划

														 //cout << "更换攻击塔，清空攻击士兵列表" << endl;
		setfree(ATTACK);

		last_attack_tower = current_attack_tower[0];   //被重新赋值
	}

	///cout << endl;
	///cout << "我方士兵数量" << inf->playerInfo[current_id].soldier_num << " 数量超过此数量启动攻击" << inf->playerInfo[current_id].tower_num + 1 + DefenseSoldier.size() << endl;

	bool attack_flag = true;

	if (attack_flag) {
			


		//确定好tag后，找到自由士兵进行攻击

		for (unsigned int i = 0; i < data->MyTroop.size(); i++) { //找到零散士兵让其攻击
			if (data->MyTroop[i].duty == FREE) {
				data->MyTroop[i].duty = ATTACK;
				data->MyTroop[i].attack();
				///cout << "零散士兵" << i << "出发攻击" << endl;
			}
		}

	}

}

void Decision::clean() {
	for (unsigned int i = 0; i < data->MyTroop.size(); i++) {
		data->MyTroop[i].clean();
	}
}

void Decision::analyse_tower() {

	//激活评估函数进行参数计算，但不进行任何动作

	for (unsigned int i = 0; i < TOTAL_TOWER; i++) { //浅激活
		data->TowerInf[i].evaluate(data->EnemyTroop, data->MyTroop);
	}
	for (unsigned int i = 0; i < TOTAL_TOWER; i++) {       //深激活
		data->TowerInf[i].static_generate();
	}

	//激活塔造兵相关指标.....


	//参数计算好之后，将塔储存在新的列表里用于塔控制兵的决策以及塔的决策

	for (unsigned int i = 0; i < TOTAL_TOWER; i++) {       //将结果分开
		if (data->TowerInf[i].base.owner != current_id) {
			unoccupied.push_back(data->TowerInf[i]);
		}
		else {
			occupied.push_back(data->TowerInf[i]);
		}
	}

	sort(unoccupied.begin(), unoccupied.end(), attack_evaluate);
	//排序结束

	///cout << "开始排序" << endl;
	for (unsigned int i = 0; i < unoccupied.size(); i++) {
		//cout << unoccupied[i].base.id << " ";
	}

	//选出攻击塔

	unsigned int current_attack_tower_iter = 0;     //存储攻击塔的迭代器
	for (; current_attack_tower_iter < unoccupied.size(); current_attack_tower_iter++) {
		current_attack_tower.push_back(unoccupied[current_attack_tower_iter].base.id);
	}


}

void Decision::analyse_troop() {
	//激活士兵，进行二次初始化
	for (unsigned int i = 0; i < data->MyTroop.size(); i++) {
		data->MyTroop[i].evaluate();
	}

}


void Decision::product() { //生产，需利用generate后的数据
	int Soldier_resourse[8] = { 0,40,40,50,70,55,60,70 };
	int* propotion;
	if (data->MyTroop.size() < DIVISION_OF_SOLDIER_PROPOTION)
		propotion = propotion_short;
	else
		propotion = propotion_long;

	if (true) {
		double cos_cost[NUM_KINDS_SOLDIER] = { 0 };
		for (int i = 1; i < NUM_KINDS_SOLDIER; i++) {  //判断与目标的夹角
			int temp_Decision[NUM_KINDS_SOLDIER] = { 0 };
			for (int j = 0; j < NUM_KINDS_SOLDIER; j++)    //复制数组
			{
				temp_Decision[j] = SoldierNum[j];
				if (j == i) {
					temp_Decision[j]++;
				}
			}
			cos_cost[i] = mycos(temp_Decision, propotion, NUM_KINDS_SOLDIER);
		}
		double mincost = 1;
		int argmin = 0;
		for (int i = 1; i < NUM_KINDS_SOLDIER; i++)
		{
			double temp_cost = 1 - cos_cost[i];
			if (temp_cost < mincost) {       //记录代价函数最小值及下标
				mincost = temp_cost;
				argmin = i;
			}
		}


		sort(occupied.begin(), occupied.end(), product_evaluate);

		if (CurrentState != NUM_KINDS_SOLDIER) {       //确定造兵
			CurrentState = argmin;
			//如果资源允许，开始造兵
			//cout << CurrentState << endl;
			for (unsigned int i = 0; i < occupied.size(); i++) {
				if (occupied[i].base.recruiting == false) {
					inf->myCommandList.addCommand(Produce, occupied[i].base.id, CurrentState);
					break;
				}
			}
			CurrentState = 0;

		}
	}

	//升级塔

	int upgrade_signal[UPGRADE_POINT][2] = {
		{ 6,0 },{ 9,0 },{ 12,0 },{ 15,0 },{ 20,0 },{ 100,0 }
	};
	int upgrade_at_soldiernum;


	int SoldierSum = 0;
	int Tower_resource[3] = { 0,80,150 };
	if (inf->round > 20) {
		for (int i = 1; i < NUM_KINDS_SOLDIER; i++) {
			SoldierSum += SoldierNum[i];
		}
		bool FullFlag = true;
		for (unsigned int i = 0; i < occupied.size(); i++) {
			if (occupied[i].base.level < 3) {
				FullFlag = false;
			}
		}


		for (int i = 0; i < UPGRADE_POINT; i++) {
			if (upgrade_signal[i][1] == 0) {
				upgrade_at_soldiernum = upgrade_signal[i][0];
				break;
			}
		}

		if (SoldierSum == upgrade_at_soldiernum && !FullFlag) {
			CurrentState = NUM_KINDS_SOLDIER;  //置于升级状态
		}
		if (CurrentState == NUM_KINDS_SOLDIER) {
			for (unsigned int i = 0; i < occupied.size(); i++) {
				if (occupied[i].base.level < 3) {
					if (inf->playerInfo[current_id].resource >= Tower_resource[occupied[i].base.level]) {  //只要存在一个塔可以升级，那么就升级
						inf->myCommandList.addCommand(Upgrade, occupied[i].base.id);

						///cout << "兵为5的倍数，开始升级塔" << endl;

						for (int i = 0; i < UPGRADE_POINT; i++) {
							if (upgrade_signal[i][1] == 0) {
								upgrade_signal[i][0] = 1;
							}
						}

						CurrentState = 0;
					}
				}
			}
		}
	}

}

void Decision::command_tower() {
	for (unsigned int i = 0; i < occupied.size(); i++)
	{
		int need = occupied[i].basicneed();
		for (unsigned int j = 0; j < occupied[i].mytroop.size() && j < need; j++) {
			data->MyTroop[findorigin(occupied[i].mytroop[j].base.id)].duty = TOWER_DEFENSE;
		}
	}
	for (unsigned int i = 0; i < occupied.size(); i++)
	{
		//有敌人启动防御

		occupied[i].defense(data->MyTroop);


	}


}

void Decision::command_troop() {
	for (unsigned int i = 0; i < data->MyTroop.size(); i++) {
		State type = data->MyTroop[i].duty;
		if (type == ATTACK)
			data->MyTroop[i].attack();
		else if(type == DEFENSE)
			if(current_defense_tower.size())
				data->MyTroop[i].defense(current_defense_tower[0]);
	}
}

void Decision::shortterm() {

	//操作塔：
	if (index[0] == 0) {
		inf->myCommandList.addCommand(Produce, occupied[0].base.id, LightKnight); index[0] = 1;
	}//操作0：塔1造轻骑兵
	bool available = !occupied[0].base.recruiting
		&& inf->playerInfo[current_id].population < inf->playerInfo[current_id].max_population;

	//cout << "Round" << inf->round << " Available " << available << endl;


	if (index[0] == 1 && available && index[1] == 0 && inf->round != 0) {
		//cout << "准备造第二个轻骑兵" << endl;
		inf->myCommandList.addCommand(Produce, occupied[0].base.id, LightKnight); index[1] = 1;

	}//操作1：塔1造轻骑兵

	if (inf->playerInfo[current_id].resource >= 80 && index[2] == 0 && index[1] == 1) {
		inf->myCommandList.addCommand(Upgrade, occupied[0].base.id); index[2] = 1;
	}//操作2：塔1升级

	 //******************************//
	if (occupied[0].base.level == 1 && index[3] == 1 && inf->playerInfo[current_id].resource >= 70 && index[4] == 0) {
		inf->myCommandList.addCommand(Produce, occupied[0].base.id, HeavyKnight);  index[4] = 1;
	}//操作4：塔1造重骑兵


	int first_troop;
	int second_troop;

	if (index[1] == 1 && index[6] == 0) {
		int n = occupied[0].base.id + 1;
		if (data->MyTroop.size() > 0) {
			first_troop = data->MyTroop[0].base.id;
			data->MyTroop[0].endplace(inf->towerInfo[n].x_position, inf->towerInfo[n].y_position);
			data->MyTroop[0].go();
			if (distance(
				data->MyTroop[0].base.x_position,
				data->MyTroop[0].base.y_position,
				data->MyTroop[0].place[0],
				data->MyTroop[0].place[1])
				== 0)
				index[6] = 1;
		}


	}//操作6：移动轻骑兵1占塔2

	if (index[6] == 1 && index[7] == 0) {
		if (inf->playerInfo[current_id].tower_num != 2) data->MyTroop[0].gettower();
		if (inf->playerInfo[current_id].tower_num == 2) index[7] = 1;   //造出第二个塔
	}//操作7：轻骑兵1打塔2

	if (inf->playerInfo[current_id].soldier_num>1 && index[11] == 0) {
		int n = occupied[0].base.id + 1;
		data->MyTroop[1].endplace(inf->towerInfo[n].x_position, inf->towerInfo[n].y_position);
		data->MyTroop[1].go();
		second_troop = data->MyTroop[1].base.id;
		if (distance(inf->soldierInfo[second_troop].x_position, inf->soldierInfo[second_troop].y_position, data->MyTroop[1].place[0], data->MyTroop[1].place[1]) == 0)
			index[11] = 1;
	}//操作6：移动轻骑兵2占塔2

	if (index[11] == 1 && index[12] == 0) {
		if (inf->playerInfo[current_id].tower_num != 2) data->MyTroop[1].gettower();
		if (inf->playerInfo[current_id].tower_num == 2) index[12] = 1;
	}//操作7：轻骑兵2打塔2

}

void Decision::command() {
	//产生Properties中的决策指标
	for (unsigned int i = 0; current_attack_tower.size() > 0; i++) {
		current_attack_tower.pop_back();
	}
	for (unsigned int i = 0; current_defense_tower.size() > 0; i++) {
		current_defense_tower.pop_back();
	}

	analyse_tower();
	analyse_troop();
	//cout << "check 1" << endl;
	command_tower(); //向塔传达指令	
	//cout << "check 2" << endl;

	//cout << "check 3" << endl;
	clean();


	bool short_term_flag = index[12];

	if (inf->round > 20) {


		if (first_tower_flag) {
			last_attack_tower = current_attack_tower[0];
			first_tower_flag = false;
		}

		//cout << "check 4" << endl;
		defense();
		//cout << "check 5" << endl;
		attack();
		//cout << "check 6" << endl;
		clean();

	}
	else {
		shortterm();
	}

	command_troop();  //向兵传达指令
	product();


}



/**************Decision成员函数定义结束*******************/
