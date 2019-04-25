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

/************ȫ�ִ������***********/

unsigned int CurrentState = 0;    //��ǰ����״̬��С��NUM_KINDS_SOLDIERʱΪʿ��������NUM_KINDS_SOLDIERʱΪ������
int propotion_short[NUM_KINDS_SOLDIER] = { 0,0,1,3,0,0,1,0 };    //��������������
int propotion_long[NUM_KINDS_SOLDIER] = { 0,0,1,2,0,1,1,1 };    //��������������

int current_id;
vector<int> current_attack_tower;

bool first_tower_flag = true;
int last_attack_tower;  //��¼��һ����������

vector<int> current_defense_tower;

int MyTowerID[TOTAL_TOWER];    //�洢�ҷ���ID
int N_MyTowerID[TOTAL_TOWER];      //�洢δռ����ID
int SoldierNum[NUM_KINDS_SOLDIER] = { 0, };           //��¼����ʿ��������

static int index[100] = { 0, };      //����ָ��ִ�����


int n;         //����flag

int data_soldier[8][6] = {//4��Ұ��Χ 5������Χ
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
};//����
int hide[4][2] = {
	{ 21,25 },
{ 27,25 },
{ 24,28 },
{ 24,23 }
};//�����

Info* inf;

/*************ȫ�ִ����������**********/



/********�������ܺ���**********************/

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

//�жϵ�ͼ�Ƿ�Խ��
int flag(int a, int b) {
	int fl = 0;
	if (a <= 49 && a >= 0 && b >= 0 && b <= 49)
		fl = 1;
	return fl;
}

void AreaDecision(int radius, int scan_point[][2], int center_x, int center_y) { //ȷ����������
	scan_point[0][0] = (center_x - radius >= 0 ? center_x - radius : 0);
	scan_point[0][1] = (center_y - radius >= 0 ? center_y - radius : 0);
	scan_point[1][0] = (scan_point[0][0] + 2 * radius <= 49 ? scan_point[0][0] + 2 * radius : 49);
	scan_point[1][1] = (scan_point[0][1] + 2 * radius <= 49 ? scan_point[0][1] + 2 * radius : 49);

}
//�������
int distance(int a, int b, int x, int y) {
	return abs(a - x) + abs(b - y);
}

int distance(TPoint p1, TPoint p2) {
	return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

//����ǰ׺ƽ��ֵ
double Average(double CurrentAverage, int CurrentNumber, int NewData) {
	double NewSum = CurrentAverage * CurrentNumber + NewData;
	return NewSum / (CurrentNumber + 1);
}

int Ability_TSoldierType(TSoldier s) {  //��ĳ������ʿ��������
	return 10 * data_soldier[s.type][0] + 5 * data_soldier[s.type][1] + 2 * s.blood + 5 * data_soldier[s.type][3];
}


/*********�������ܺ�������*********/

/*********��Ķ���**********/

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

class Enemy {   //�Թ���Ŀ�������
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

	void evaluate();     //����

	int danger;   //Σ�ճ̶�
	int attack_possibility;  //����������
	int value;   //������ֵ

	TSoldier base;

};


class Troop {   //ʿ��
public:
	Troop() {

	}
	Troop(const TSoldier& t, unsigned int index_soldierinfo) {
		base = t;
		index = index_soldierinfo;
		view_length = data_soldier[base.type][4];
		length = inf->soldierInfo[index].move_left;  //�ж���
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

	void move(int length);      //�ƶ�����length����
	void endplace(int x, int y);      //
	void endplace(TPoint t);
	void attackplace(int x, int y);
	void attackplace(TPoint t);
	void search(int length, int type);   //�������Σ�length��Ұ��type����
	void gettower();
	void go();

	void step_go(int x, int y);
	void map_go(Map&);

	void investigation();  //��Ұ�����

	void evaluate();  //������
	void clean();
	void clean(int a);

	bool change_duty(State s);
	void march();
	void attack();    //����ָ��
	void defense(int towerid);

	void act();    //���ж�



	TSoldier base;
	unsigned int index;

	int view_length;         //��Ұ�߳�
	int length;              //�ж���

	State duty;

	int attack_possibility;      //����������
	int defense_need;            //������Ҫ
	int tag = 0;                   //������ǩ
	int signal;             //�ҷ��з�������

	int way[4];       //x����������+����+y����������+����
	int place[2];     //Ŀ���
	int delta[4];      //���������

	int current_position[2];

	int detail[11][11]; //��¼ÿ�����ռ�����
	int point[2];        //��Ұ���½�����

	vector<Troop> mytroop;
	vector<Enemy> enemy;

	TPoint attack_target;        //����Ŀ��
	TPoint defense_target;       //���ط���

	bool state[COMMAND_LENGTH];

	int radius = 5;



};

class Tower {                //����������
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

		//��ȡ��Χ��Ұ
		for (int i = 0; i < length; i++) {
			for (int j = 0; j < length; j++) {
				if (flag(point[0] + i, point[1] + j))
					detail[i][j] = inf->pointInfo[point[0] + i][point[1] + j].occupied_type;
				else
					detail[i][j] = -1;
				if (detail[i][j] == 1) {
					int soldier_id = inf->pointInfo[point[0] + i][point[1] + j].soldier;
					//����Ѱ����ռʿ��
					for (unsigned int k = 0; k < inf->soldierInfo.size(); k++) {
						if (inf->soldierInfo[k].id == soldier_id) {  //�ҵ�ʿ��

							if (inf->soldierInfo[k].owner == current_id) {
								//cout << "��ʼpush�Լ���ʿ��" << endl;
								this->mytroop.push_back(Troop(inf->soldierInfo[k], k));
								//cout << "����push�Լ���ʿ��" << endl;
							}
							else {
								this->enemy.push_back(Enemy(inf->soldierInfo[k]));
							}

						}
					}
				}
			}
		}
		/*cout << "��" << base.id << " ӵ����" << base.owner << " �ҷ�ʿ������" << mytroop.size() << " �з�ʿ������" << enemy.size();
		cout << " �ҷ�ʿ��";
		for (unsigned int i = 0; i < mytroop.size(); i++) {
		cout << mytroop[i].base.id << " ";
		}

		cout << " �з�ʿ��";
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

	void evaluate_danger(vector<Enemy> enemy, vector<Troop> my);        //����Σ����
	void evaluate_accessible();          //����ɽӽ���
	void evaluate_convenience();
	void evaluate_troop_convenience(vector<Troop> my);      //�õ��ҷ�ʿ������֮�����۵ı�����
	void evaluate_all(vector<Enemy> enemy, vector<Troop> my);   //��������

	unsigned int basicneed();
	unsigned int is_dangerous();

	void defense(vector<Troop>& total_troop);
	void evaluate(vector<Enemy> enemy, vector<Troop> my);   //����ָ��

	void static_generate();         //���þ�̬�����ļ����

	void act();   //�ж�

	static int sum_accessible;
	static int sum_convenience;
	static int sum_danger;
	static int sum_troop_convenience;

	int detail[11][11];
	int point[2];
	int length;                     //������Ұ
	vector<Troop> mytroop;
	vector<Enemy> enemy;

	int danger;               //��ʾΣ�ճ̶ȣ���Ϊ�з����ƣ���Ϊ�ҷ�����
	int accessible;           //�ɽӽ��̶�
	int convenience;          //����������
	int troop_convenience;         //���������ȣ���ʿ������֮�ͺ���

	int soldierneed = 0;

								   /*******�ⲿ�޸���*****/
	double attack_evaluation;   //�ɹ�����������ԽСԽ����
	double product_evaluation;   //������������ԽСԽ����


								 /******�ⲿ�޸�������***************/

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

class Properties {  //�ҷ�����
public:

	Properties(Info& info) :EnemyEvaluation(4) {  //���캯��
												  //��յ�ǰʿ��
		for (int i = 0; i < NUM_KINDS_SOLDIER; i++) {
			SoldierNum[i] = 0;
		}

		int my_id = info.myID;
		current_id = my_id;

		for (int i = 0; i < 4; i++) {               //��ȡ����������Ϣ
			if (info.playerInfo[i].id == my_id) {
				attack = info.playerInfo[i].population * 10 + info.playerInfo[i].tower_num * 50 + info.playerInfo[i].resource * 1;
				EnemyEvaluation[i] = -1;
			}
			else {
				EnemyEvaluation[i] = info.playerInfo[i].population * 10 + info.playerInfo[i].tower_num * 50 + info.playerInfo[i].resource * 1;
			}
		}

		//ָ��ָ����ȷ
		inf = &info;

		//��ȡ�ҷ�������ҷ�����Ϣ
		int MyTowerID_iter = 0;
		int N_MyTowerID_iter = 0;
		for (int i = 0; i < TOTAL_TOWER; i++) {
			if (inf->towerInfo[i].owner == my_id) {
				MyTowerID[MyTowerID_iter] = inf->towerInfo[i].id;  //��MytowerID�����ҷ���id
				MyTowerID_iter++;
				//cout << "�ҷ���" << MyTowerID[MyTowerID_iter - 1] << endl;

			}
			else {
				N_MyTowerID[N_MyTowerID_iter] = inf->towerInfo[i].id;
				N_MyTowerID_iter++;
				//cout << "���ҷ���" << N_MyTowerID[N_MyTowerID_iter - 1] << endl;

			}
			TowerInf.push_back(inf->towerInfo[i]);      //��Ϣ����

		}
		while (MyTowerID_iter<TOTAL_TOWER)                 //��λ��-1���
		{
			MyTowerID[MyTowerID_iter] = -1;
			MyTowerID_iter++;
		}
		while (N_MyTowerID_iter<TOTAL_TOWER)
		{
			N_MyTowerID[N_MyTowerID_iter] = -1;
			N_MyTowerID_iter++;
		}

		//��ȡ�ҷ���з�ʿ����Ϣ

		for (unsigned int i = 0; i < inf->soldierInfo.size(); i++) {
			if (inf->soldierInfo[i].owner == my_id) {
				MyTroop.push_back(Troop(inf->soldierInfo[i], i));
				SoldierNum[inf->soldierInfo[i].type]++;  //����
			}
			else {
				EnemyTroop.push_back(inf->soldierInfo[i]);
			}
		}

	}


	double attack;       //������
	double defense;          //������
	int swift;         //�ƶ�����
	int demolition;       //��������

	vector<double> EnemyEvaluation;    //��������ֵ
	vector<Tower> TowerInf;
	vector<Troop> MyTroop;
	vector<Enemy> EnemyTroop;


};


class Decision {      //����
public:

	Decision() {}
	Decision(Properties* p) {
		data = p;
	}

	int findorigin(int soldierid);  //ͨ��ʿ��id�ҵ��ҵ�ʿ�����±�
	bool in_list(int id, vector<int> v);         //�ж�id�Ƿ����б�v��

	void shortterm();
	int get_duty_num(State s);   //��ȡs״̬��ʿ������
	void setfree(State s);

	void analyse_tower();
	void analyse_troop();

	void command_tower();   //ָ����
	void command_troop();   //ָ�ӱ�

	void clean();
	void product();
	void defense();
	void attack();    //�������ʽ


	void command();    //������

	int AttackWeigh;
	int DefenseWeigh;

	vector<Tower> unoccupied;           //��ռ���б�
	vector<Tower> occupied;             //δռ���б�


	Properties* data;


};


/*************��ľ�̬�����ĳ�ʼ��***************/
int Tower::sum_accessible = 0;
int Tower::sum_convenience = 0;
int Tower::sum_danger = 0;
int Tower::sum_troop_convenience = 0;

/**************�ྲ̬������ʼ�����****************/

/*
�������ĸ����ָ�������
info.myCommandList.addCommand(Produce,aim_tower_id,HeavyArcher);//�������ڶ����������������id������������ı���
info.myCommandList.addCommand(Attack,aim_soldier_id,x_position,y_position);//��������ڶ��������Ƿ��𹥻���ʿ��id�������͵��ĸ�������Ŀ���x��yλ��
info.myCommandList.addCommand(Upgrade,aim_tower_id);//��������ڶ�������������������id
info.myCommandList.addCommand(Move,aim_soldier_id,UP,distance);//�ƶ�����ڶ������������ƶ���ʿ��id���������������ƶ����򣬵��ĸ��������ƶ�����
*/
void player_ai(Info& info)
{
	//cout << "Round " << info.round << " start." << endl << endl;

	Properties MyProperties(info);

	//cout << "��ʼ�����Round" << info.round << endl;

	//cin >> n;
	//��ʼ�����
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

/***********Enemy��Ա��������************/

void Enemy::evaluate() {

}

/*********Enemy��Ա�����������*******/


/*************Tower��Ա��������*****************************/

void Tower::evaluate_danger(vector<Enemy> enemy, vector<Troop> my) {
	int scan[2][2];
	//ȷ����������
	AreaDecision(3, scan, base.x_position, base.y_position);
	danger = 1;  //��ʼ��


	for (unsigned int i = 0; i < enemy.size(); i++) {
		if (enemy[i].base.x_position >= scan[0][0] && enemy[i].base.x_position <= scan[1][0]
			&& enemy[i].base.y_position >= scan[0][1] && enemy[i].base.y_position <= scan[1][1]) {  //��������ڷ�Χ��
			danger += Ability_TSoldierType(enemy[i].base);

		}
	}
	for (unsigned int i = 0; i < my.size(); i++) {
		if (my[i].base.x_position >= scan[0][0] && my[i].base.x_position <= scan[1][0]
			&& my[i].base.y_position >= scan[0][1] && my[i].base.y_position <= scan[1][1]) {  //����ҷ��ڷ�Χ��
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

unsigned int Tower::is_dangerous() {  //������Ҫʿ������
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
			if (enemy[k].base.blood <= 0) k++;   //��˳�򹥻�

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

	//cout <<"��������"<< normalizer_accessible << " " << normalizer_danger << " " << normalizer_convenience;

	double weigh_accessible = 1;
	double weigh_danger = 2;
	double weigh_convenience = 2;
	double weigh_troop_convenience = 2;
	double weigh_blood = 2;


	attack_evaluation = weigh_accessible * accessible / normalizer_accessible + weigh_danger * danger / normalizer_danger + weigh_troop_convenience * troop_convenience / normalizer_troop_convenience + weigh_blood * base.blood / normalizer_blood;
	product_evaluation = weigh_convenience * convenience / normalizer_convenience;

	//cout << "��" << base.id << "��������:" << attack_evaluation << endl;

}

/*************Tower��Ա�����������*****************************/


/*************Troop��Ա��������*************/


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
	//��ȡ��Χ��Ұ
	for (int i = 0; i < view_length; i++) {
		for (int j = 0; j < view_length; j++) {
			if (flag(point[0] + i, point[1] + j))
				detail[i][j] = inf->pointInfo[point[0] + i][point[1] + j].occupied_type;
			else
				detail[i][j] = -1;
			if (detail[i][j] == 1) {
				int soldier_id = inf->pointInfo[point[0] + i][point[1] + j].soldier;
				//����Ѱ����ռʿ��
				for (unsigned int k = 0; k < inf->soldierInfo.size(); k++) {
					if (inf->soldierInfo[k].id == soldier_id) {  //�ҵ�ʿ��

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
	if (distance(x, y, current_position[0], current_position[1]) != 1) {  //��������
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
		//cout << "ʿ��" << base.id << "����ռ�ã����ܽ��лط�" << endl;
	}
	//cout << "Check 4.X.X.end" << endl;
}

void Troop::act() {
	evaluate();

}

/*************Troop��Ա�����������*************/



/**************Decision��Ա��������*******************/

int Decision::findorigin(int soldierid) {  //ͨ��ʿ��id�ҵ��ҵ�ʿ�����±�
	int returnid = -1;                             //Խ�羯��
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
	if (get_duty_num(DEFENSE) > 0) {  //�ж��Ƿ�ﵽĿ��
		for (unsigned int i = 0; i < occupied.size(); i++) {
			if (inf->round <= 50) {
				if (occupied[i].mytroop.size() - occupied[i].enemy.size() < 0) {
					flag = false;
				}
			}
			else {
				if (occupied[i].mytroop.size() - occupied[i].enemy.size() < 1) { //�ҷ�ʿ���ȵз�ʿ����һ��
					flag = false;
				}
			}
		}
		if (flag) {  //����Ѿ���ȫ��Ĩȥ���з������
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

	if (current_defense_tower.size()) {    //�������Ҫ�ط��ı���

		if (get_duty_num(DEFENSE)) {
			for (unsigned int i = 0; i < data->MyTroop.size(); i++) {
				//cout << "Check 4.1.1." << i << endl;
				data->MyTroop[i].defense(current_defense_tower[0]);             //��ʱֻ���Ȼط�һ����
				//cout << "��ǰ���лط�ʿ��" << findorigin(DefenseSoldier[0]) << endl;
				//cout << "�ܻط�ʿ��" << DefenseSoldier.size() << endl;
			}
		}

		//cout << "Check 4.5" << endl;

		//bool find_free_soldier_flag = false;
		bool safe_flag = false;

		vector<Pair> id_distance;   //index����ʿ��id��value�������
		for (unsigned int i = 0; i < data->MyTroop.size(); i++) { //�ҵ���ɢʿ������ط�,������id_distance��
			if (data->MyTroop[i].duty == FREE) {
				id_distance.push_back(Pair(data->MyTroop[i].base.id,
					distance(data->MyTroop[i].base.position, data->TowerInf[current_defense_tower[0]].base.position)));

				//find_free_soldier_flag = true;
				//cout << "��ɢʿ��" << MyTroop[i].base.id << "�����ط�" << endl;

			}
		}
		sort(id_distance.begin(), id_distance.end(), value);

		//cout << "Check 4.6" << endl;

		for (unsigned int i = 0; i < id_distance.size() && get_duty_num(DEFENSE)<sum_soldier_need; i++) {
			data->MyTroop[findorigin(id_distance[i].index)].duty = DEFENSE;  //����ʿ������

			data->MyTroop[findorigin(id_distance[i].index)].defense(current_defense_tower[0]);     //��ʱֻ����һ����
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

	if (last_attack_tower != current_attack_tower[0]) {  //��������˹���Ŀ�꣬���������Ѿ������ˣ�֤����Ҫ���¹滮

														 //cout << "��������������չ���ʿ���б�" << endl;
		setfree(ATTACK);

		last_attack_tower = current_attack_tower[0];   //�����¸�ֵ
	}

	///cout << endl;
	///cout << "�ҷ�ʿ������" << inf->playerInfo[current_id].soldier_num << " ����������������������" << inf->playerInfo[current_id].tower_num + 1 + DefenseSoldier.size() << endl;

	bool attack_flag = true;

	if (attack_flag) {
			


		//ȷ����tag���ҵ�����ʿ�����й���

		for (unsigned int i = 0; i < data->MyTroop.size(); i++) { //�ҵ���ɢʿ�����乥��
			if (data->MyTroop[i].duty == FREE) {
				data->MyTroop[i].duty = ATTACK;
				data->MyTroop[i].attack();
				///cout << "��ɢʿ��" << i << "��������" << endl;
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

	//���������������в������㣬���������κζ���

	for (unsigned int i = 0; i < TOTAL_TOWER; i++) { //ǳ����
		data->TowerInf[i].evaluate(data->EnemyTroop, data->MyTroop);
	}
	for (unsigned int i = 0; i < TOTAL_TOWER; i++) {       //���
		data->TowerInf[i].static_generate();
	}

	//������������ָ��.....


	//���������֮�󣬽����������µ��б������������Ʊ��ľ����Լ����ľ���

	for (unsigned int i = 0; i < TOTAL_TOWER; i++) {       //������ֿ�
		if (data->TowerInf[i].base.owner != current_id) {
			unoccupied.push_back(data->TowerInf[i]);
		}
		else {
			occupied.push_back(data->TowerInf[i]);
		}
	}

	sort(unoccupied.begin(), unoccupied.end(), attack_evaluate);
	//�������

	///cout << "��ʼ����" << endl;
	for (unsigned int i = 0; i < unoccupied.size(); i++) {
		//cout << unoccupied[i].base.id << " ";
	}

	//ѡ��������

	unsigned int current_attack_tower_iter = 0;     //�洢�������ĵ�����
	for (; current_attack_tower_iter < unoccupied.size(); current_attack_tower_iter++) {
		current_attack_tower.push_back(unoccupied[current_attack_tower_iter].base.id);
	}


}

void Decision::analyse_troop() {
	//����ʿ�������ж��γ�ʼ��
	for (unsigned int i = 0; i < data->MyTroop.size(); i++) {
		data->MyTroop[i].evaluate();
	}

}


void Decision::product() { //������������generate�������
	int Soldier_resourse[8] = { 0,40,40,50,70,55,60,70 };
	int* propotion;
	if (data->MyTroop.size() < DIVISION_OF_SOLDIER_PROPOTION)
		propotion = propotion_short;
	else
		propotion = propotion_long;

	if (true) {
		double cos_cost[NUM_KINDS_SOLDIER] = { 0 };
		for (int i = 1; i < NUM_KINDS_SOLDIER; i++) {  //�ж���Ŀ��ļн�
			int temp_Decision[NUM_KINDS_SOLDIER] = { 0 };
			for (int j = 0; j < NUM_KINDS_SOLDIER; j++)    //��������
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
			if (temp_cost < mincost) {       //��¼���ۺ�����Сֵ���±�
				mincost = temp_cost;
				argmin = i;
			}
		}


		sort(occupied.begin(), occupied.end(), product_evaluate);

		if (CurrentState != NUM_KINDS_SOLDIER) {       //ȷ�����
			CurrentState = argmin;
			//�����Դ������ʼ���
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

	//������

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
			CurrentState = NUM_KINDS_SOLDIER;  //��������״̬
		}
		if (CurrentState == NUM_KINDS_SOLDIER) {
			for (unsigned int i = 0; i < occupied.size(); i++) {
				if (occupied[i].base.level < 3) {
					if (inf->playerInfo[current_id].resource >= Tower_resource[occupied[i].base.level]) {  //ֻҪ����һ����������������ô������
						inf->myCommandList.addCommand(Upgrade, occupied[i].base.id);

						///cout << "��Ϊ5�ı�������ʼ������" << endl;

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
		//�е�����������

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

	//��������
	if (index[0] == 0) {
		inf->myCommandList.addCommand(Produce, occupied[0].base.id, LightKnight); index[0] = 1;
	}//����0����1�������
	bool available = !occupied[0].base.recruiting
		&& inf->playerInfo[current_id].population < inf->playerInfo[current_id].max_population;

	//cout << "Round" << inf->round << " Available " << available << endl;


	if (index[0] == 1 && available && index[1] == 0 && inf->round != 0) {
		//cout << "׼����ڶ��������" << endl;
		inf->myCommandList.addCommand(Produce, occupied[0].base.id, LightKnight); index[1] = 1;

	}//����1����1�������

	if (inf->playerInfo[current_id].resource >= 80 && index[2] == 0 && index[1] == 1) {
		inf->myCommandList.addCommand(Upgrade, occupied[0].base.id); index[2] = 1;
	}//����2����1����

	 //******************************//
	if (occupied[0].base.level == 1 && index[3] == 1 && inf->playerInfo[current_id].resource >= 70 && index[4] == 0) {
		inf->myCommandList.addCommand(Produce, occupied[0].base.id, HeavyKnight);  index[4] = 1;
	}//����4����1�������


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


	}//����6���ƶ������1ռ��2

	if (index[6] == 1 && index[7] == 0) {
		if (inf->playerInfo[current_id].tower_num != 2) data->MyTroop[0].gettower();
		if (inf->playerInfo[current_id].tower_num == 2) index[7] = 1;   //����ڶ�����
	}//����7�������1����2

	if (inf->playerInfo[current_id].soldier_num>1 && index[11] == 0) {
		int n = occupied[0].base.id + 1;
		data->MyTroop[1].endplace(inf->towerInfo[n].x_position, inf->towerInfo[n].y_position);
		data->MyTroop[1].go();
		second_troop = data->MyTroop[1].base.id;
		if (distance(inf->soldierInfo[second_troop].x_position, inf->soldierInfo[second_troop].y_position, data->MyTroop[1].place[0], data->MyTroop[1].place[1]) == 0)
			index[11] = 1;
	}//����6���ƶ������2ռ��2

	if (index[11] == 1 && index[12] == 0) {
		if (inf->playerInfo[current_id].tower_num != 2) data->MyTroop[1].gettower();
		if (inf->playerInfo[current_id].tower_num == 2) index[12] = 1;
	}//����7�������2����2

}

void Decision::command() {
	//����Properties�еľ���ָ��
	for (unsigned int i = 0; current_attack_tower.size() > 0; i++) {
		current_attack_tower.pop_back();
	}
	for (unsigned int i = 0; current_defense_tower.size() > 0; i++) {
		current_defense_tower.pop_back();
	}

	analyse_tower();
	analyse_troop();
	//cout << "check 1" << endl;
	command_tower(); //��������ָ��	
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

	command_troop();  //�������ָ��
	product();


}



/**************Decision��Ա�����������*******************/
