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

unsigned int CurrretState = 0;    //当前游戏状态
int current_id;
int current_attack_tower[TOTAL_TOWER];
int MyTowerID[TOTAL_TOWER];    //存储我方塔ID
int N_MyTowerID[TOTAL_TOWER];      //存储未占领塔ID
int SoldierNum[8] = { 0, };           //记录各种士兵的数量


int data_soldier[8][4] = {
{ 0,0,0,0 },
{25,15,100,3},
{35,5,80,3},
{40,10,100,5},
{30,20,200,2},
{30,25,150,3},
{45,15,100,3},
{50,20,120,4}
};

Info* inf; //Info指针


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


class MapUnit {
public:

	int danger;

	TPoint base;
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


class Troop {   //对我方士兵的评估
public:
	Troop() {

	}
	Troop(const TSoldier& t) {
		base = t;
	}
	Troop(const Troop& t) {
		base = t.base;
		attack_possibility = t.attack_possibility;
		defense_need = t.defense_need;
		for (int i = 0; i < 4; i++) {
			way[i] = t.way[i];
			if (i < 2) {
				place[i] = t.place[i];
			}
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

	void move(int x, int y, int length, int way[4]);      //移动方向
	void endplace(int x, int y, int place[2]);      //打塔确定位置

	void evaluate();

	void generate_move();
	void generate_attack();
	void generate();

	TSoldier base;
	
	int attack_possibility;      //攻击可能性
	int defense_need;            //防守需要

	int way[4];       //x方向走左右+距离+y方向走左右+距离
	int place[2];     //目标点

	TPoint attack_target;        //攻击目标
	TPoint defense_target;       //防守方向


	int radius = 5;
	
	

};

class Tower {                //对塔的评估
public:
	Tower(const TowerInfo& t) {
		base = t;

	}
	Tower() {}
	Tower(const Tower& t) {
		danger = t.danger;
		accessible = t.accessible;
		attack_evaluation = attack_evaluation;
		base = t.base;
	}

	void evaluate_danger(vector<Enemy> enemy, vector<Troop> my);        //计算危险性
	void evaluate_accessible();          //计算可接近性
	void evaluate_convenience();

	void evaluate(vector<Enemy> enemy, vector<Troop> my, vector<Tower> tower);   //评估函数
	void generate(vector<Enemy> enemy, vector<Troop> my, vector<Tower> tower);   //激活函数

	bool operator<(const Tower& t) {          //大小代表攻击难度
		return this->attack_evaluation < t.attack_evaluation;
	}

	int danger;               //表示危险程度，正为敌方控制，负为我方控制
	int accessible;           //可接近程度
	int convenience;          //攻塔便利度


	/*******外部修改区*****/
	double attack_evaluation;   //可攻击性评估
	double product_evaluation;   //生产性评估

	bool friend attack_evaluate(const Tower& t1, const Tower& t2) {
		return t1.attack_evaluation < t2.attack_evaluation;
	}
	bool friend product_evaluate(const Tower t1, const Tower& t2) {
		return t1.product_evaluation < t2.product_evaluation;
	}
	/******外部修改区结束***************/

	TowerInfo base;


};

class Properties {  //我方属性
public:

	Properties(Info& info):EnemyEvaluation(4) {  //构造函数
		int my_id = info.myID;
		current_id = my_id;
		
		for (int i = 0; i < 4; i++){               //获取各方属性信息
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
				cout << "我方塔" << MyTowerID[MyTowerID_iter - 1] << endl;

			}
			else {
				N_MyTowerID[N_MyTowerID_iter] = inf->towerInfo[i].id;
				N_MyTowerID_iter++;
				cout << "非我方塔" << N_MyTowerID[N_MyTowerID_iter - 1] << endl;

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
				MyTroop.push_back(inf->soldierInfo[i]);
				//SoldierNum[inf->soldierInfo[i].type]++;  //计数
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

	Decision(){}
	Decision(Properties* p) {
		data = p;
	}


	void generate();

	int AttackWeigh;
	int DefenseWeigh;


	Properties* data;

	
};


/*
基本的四个添加指令的命令
info.myCommandList.addCommand(Produce,aim_tower_id,HeavyArcher);//造兵命令，第二个参数是造兵的塔id，第三个是造的兵种
info.myCommandList.addCommand(Attack,aim_soldier_id,x_position,y_position);//攻击命令，第二个参数是发起攻击的士兵id，第三和第四个参数是目标的x，y位置
info.myCommandList.addCommand(Upgrade,aim_tower_id);//升级命令，第二个参数是欲升级的塔id
info.myCommandList.addCommand(Move,aim_soldier_id,UP,distance);//移动命令，第二个参数是欲移动的士兵id，第三个参数是移动方向，第四个参数是移动距离
*/
void player_ai(Info& info)
{
	Properties MyProperties(info);

	cout << "初始化完成Round" << info.round << endl;
	int n;
	cin >> n;
	//初始化完成
	Decision MyDecision(&MyProperties);
	cout << "Generate" << endl;
	MyDecision.generate();

}

 
void Enemy::evaluate() {

}

bool debug_flag = true;

void Tower::evaluate_danger(vector<Enemy> enemy, vector<Troop> my) {
	int scan[2][2];
	//确定搜索区域
	AreaDecision(3, scan, base.x_position, base.y_position);
	danger = 0;  //初始化


	if (enemy.size() != 0) {
		for (int i = 0; i < TOTAL_TOWER; i++)
		{
			if (inf->towerInfo[i].owner == current_id && debug_flag) {
				inf->myCommandList.addCommand(Produce, i, LightInfantry);
				debug_flag = false;
			}
		}
	}
	
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
	
}

void Tower::evaluate_accessible() {   
	accessible = 0;
	for (int i = 0; i < inf->playerInfo[current_id].tower_num; i++) {
		
		accessible += distance(inf->towerInfo[MyTowerID[i]].position, base.position);
		
	}
}

void Tower::evaluate_convenience() {
	convenience = 0;
	for (int i = 0; i < TOTAL_TOWER - inf->playerInfo[current_id].tower_num; i++) {

		convenience += distance(inf->towerInfo[N_MyTowerID[i]].position, base.position);

	}
}

void Tower::evaluate(vector<Enemy> enemy, vector<Troop> my, vector<Tower> tower) {
	evaluate_danger(enemy, my);
	evaluate_accessible();
	evaluate_convenience();
	cout << "Tower" << base.id << " Accessible" << accessible << " Convenience" << convenience <<" Danger"<< danger << endl;

}

void Tower::generate(vector<Enemy> enemy, vector<Troop> my, vector<Tower> tower) {
	evaluate(enemy, my, tower);
	
	if (base.owner == current_id) {


	}
}


void Troop::move(int x, int y, int length, int way[4]) {
	int a = base.x_position;
	int b = base.y_position;
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

void Troop::endplace(int x, int y, int place[2]) {
	int a = base.x_position;
	int b = base.y_position;
	int min = 0;
	place[0] = x - 1;
	place[1] = y - 2;
	min = distance(a, b, x - 1, y - 2);
	for (int i = 0; i < 5; i = i + 4) {
		for (int j = 0; j < 3; j++) {
			if (distance(a, b, x + j - 1, y + i - 2) < min) {
				place[0] = x + j - 1;
				place[1] = y + i - 2;
				min = distance(a, b, x + j - 1, y + i - 2);
			}
		}
	}
	for (int i = 0; i < 5; i = i + 4) {
		for (int j = 0; j < 3; j++) {
			if (distance(a, b, x + i - 2, y + j - 1) < min) {
				place[0] = x + i - 2;
				place[1] = y + j - 1;
				min = distance(a, b, x + i - 2, y + j - 1);
			}
		}
	}
}


void Troop::evaluate() {
	int scan[2][2];
	int radius = 5;                   //搜索半径
	AreaDecision(radius, scan, base.x_position, base.y_position);
	/*
	evaluation = new MapUnit* [2 * radius + 1];   //动态生成二维数组
	for (int i = 0; i <= 2 * radius; i++) {
		evaluation[i] = new MapUnit[2 * radius + 1];
	}
	*/



}

void Troop::generate_move() {
	int n = current_attack_tower[0];
	int a = base.x_position;
	int b = base.y_position;

	endplace(inf->towerInfo[n].x_position, inf->towerInfo[n].y_position, place);
	move(place[0], place[1], base.move_ability, way);
	if (way[2] != -1 || way[0] != -1) {
		if (way[3] == base.move_ability                             //优先走y方向
			&& inf->pointInfo[a][b + way[3] * (1 - 2 * way[2])].occupied_type == 0
			&& inf->pointInfo[a][b + way[3] * (1 - 2 * way[2])].land != 2)
			inf->myCommandList.addCommand(Move, base.id, way[2], way[3]);
		else if (way[3] < base.move_ability && way[3]>0              //如果移动能力剩余
			&& inf->pointInfo[a][b + way[3] * (1 - 2 * way[2])].occupied_type == 0
			&& inf->pointInfo[a][b + way[3] * (1 - 2 * way[2])].land != 2)
		{
			inf->myCommandList.addCommand(Move, base.id, way[2], way[3]);
			if (way[1] >= base.move_left)                              //转弯之后
				inf->myCommandList.addCommand(Move, base.id, way[0], base.move_left);
			else
				inf->myCommandList.addCommand(Move, base.id, way[0], way[1]);

		}

		else inf->myCommandList.addCommand(Move, base.id, way[0], way[1]);
	}
}

void Troop::generate_attack() {
	int n = current_attack_tower[0];
	int a = base.x_position;
	int b = base.y_position;

	if (base.attackable) {     //打塔
		int z = a - 1, x = a + 1, c = b - 1, v = b + 1;
		if (inf->pointInfo[a][b + 1].occupied_type == 2)
			inf->myCommandList.addCommand(Attack, base.id, a, v);
		if (inf->pointInfo[a][b - 1].occupied_type == 2)
			inf->myCommandList.addCommand(Attack, base.id, a, c);
		if (inf->pointInfo[a + 1][b].occupied_type == 2)
			inf->myCommandList.addCommand(Attack, base.id, x, b);
		if (inf->pointInfo[a - 1][b].occupied_type == 2)
			inf->myCommandList.addCommand(Attack, base.id, z, b);
	}


}

void Troop::generate() {
	generate_move();
	generate_attack();
}

void Decision::generate() {
	//产生Properties中的决策指标



	//选择拆塔目标并激活塔的相关指标
	vector<Tower> unoccupied;           //已占领列表
	vector<Tower> occupied;             //未占领列表
	for (unsigned int i = 0; i < TOTAL_TOWER; i++) {
		data->TowerInf[i].generate(data->EnemyTroop, data->MyTroop, data->TowerInf);
		if (data->TowerInf[i].base.owner != current_id) {
			unoccupied.push_back(data->TowerInf[i]);
		}
		else {
			occupied.push_back(data->TowerInf[i]);
		}
	}
	
	//sort(unoccupied.begin(), unoccupied.end());  
	unsigned int current_attack_tower_iter = 0;     //存储攻击塔的迭代器
	for (; current_attack_tower_iter < unoccupied.size(); current_attack_tower_iter++) {
		current_attack_tower[current_attack_tower_iter] = unoccupied[current_attack_tower_iter].base.id;
	}
	for (; current_attack_tower_iter < TOTAL_TOWER; current_attack_tower_iter++){
		current_attack_tower[current_attack_tower_iter] = -1;
	}

	//外部激活塔造兵相关指标


	//对我方士兵激活并操作

}