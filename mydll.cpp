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

unsigned int CurrretState = 0;    //��ǰ��Ϸ״̬
int current_id;
int current_attack_tower[TOTAL_TOWER];
int MyTowerID[TOTAL_TOWER];    //�洢�ҷ���ID
int N_MyTowerID[TOTAL_TOWER];      //�洢δռ����ID
int SoldierNum[8] = { 0, };           //��¼����ʿ��������


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

Info* inf; //Infoָ��


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


class MapUnit {
public:

	int danger;

	TPoint base;
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


class Troop {   //���ҷ�ʿ��������
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

	void move(int x, int y, int length, int way[4]);      //�ƶ�����
	void endplace(int x, int y, int place[2]);      //����ȷ��λ��

	void evaluate();

	void generate_move();
	void generate_attack();
	void generate();

	TSoldier base;
	
	int attack_possibility;      //����������
	int defense_need;            //������Ҫ

	int way[4];       //x����������+����+y����������+����
	int place[2];     //Ŀ���

	TPoint attack_target;        //����Ŀ��
	TPoint defense_target;       //���ط���


	int radius = 5;
	
	

};

class Tower {                //����������
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

	void evaluate_danger(vector<Enemy> enemy, vector<Troop> my);        //����Σ����
	void evaluate_accessible();          //����ɽӽ���
	void evaluate_convenience();

	void evaluate(vector<Enemy> enemy, vector<Troop> my, vector<Tower> tower);   //��������
	void generate(vector<Enemy> enemy, vector<Troop> my, vector<Tower> tower);   //�����

	bool operator<(const Tower& t) {          //��С�������Ѷ�
		return this->attack_evaluation < t.attack_evaluation;
	}

	int danger;               //��ʾΣ�ճ̶ȣ���Ϊ�з����ƣ���Ϊ�ҷ�����
	int accessible;           //�ɽӽ��̶�
	int convenience;          //����������


	/*******�ⲿ�޸���*****/
	double attack_evaluation;   //�ɹ���������
	double product_evaluation;   //����������

	bool friend attack_evaluate(const Tower& t1, const Tower& t2) {
		return t1.attack_evaluation < t2.attack_evaluation;
	}
	bool friend product_evaluate(const Tower t1, const Tower& t2) {
		return t1.product_evaluation < t2.product_evaluation;
	}
	/******�ⲿ�޸�������***************/

	TowerInfo base;


};

class Properties {  //�ҷ�����
public:

	Properties(Info& info):EnemyEvaluation(4) {  //���캯��
		int my_id = info.myID;
		current_id = my_id;
		
		for (int i = 0; i < 4; i++){               //��ȡ����������Ϣ
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
				cout << "�ҷ���" << MyTowerID[MyTowerID_iter - 1] << endl;

			}
			else {
				N_MyTowerID[N_MyTowerID_iter] = inf->towerInfo[i].id;
				N_MyTowerID_iter++;
				cout << "���ҷ���" << N_MyTowerID[N_MyTowerID_iter - 1] << endl;

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
				MyTroop.push_back(inf->soldierInfo[i]);
				//SoldierNum[inf->soldierInfo[i].type]++;  //����
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
�������ĸ����ָ�������
info.myCommandList.addCommand(Produce,aim_tower_id,HeavyArcher);//�������ڶ����������������id������������ı���
info.myCommandList.addCommand(Attack,aim_soldier_id,x_position,y_position);//��������ڶ��������Ƿ��𹥻���ʿ��id�������͵��ĸ�������Ŀ���x��yλ��
info.myCommandList.addCommand(Upgrade,aim_tower_id);//��������ڶ�������������������id
info.myCommandList.addCommand(Move,aim_soldier_id,UP,distance);//�ƶ�����ڶ������������ƶ���ʿ��id���������������ƶ����򣬵��ĸ��������ƶ�����
*/
void player_ai(Info& info)
{
	Properties MyProperties(info);

	cout << "��ʼ�����Round" << info.round << endl;
	int n;
	cin >> n;
	//��ʼ�����
	Decision MyDecision(&MyProperties);
	cout << "Generate" << endl;
	MyDecision.generate();

}

 
void Enemy::evaluate() {

}

bool debug_flag = true;

void Tower::evaluate_danger(vector<Enemy> enemy, vector<Troop> my) {
	int scan[2][2];
	//ȷ����������
	AreaDecision(3, scan, base.x_position, base.y_position);
	danger = 0;  //��ʼ��


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
	int radius = 5;                   //�����뾶
	AreaDecision(radius, scan, base.x_position, base.y_position);
	/*
	evaluation = new MapUnit* [2 * radius + 1];   //��̬���ɶ�ά����
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
		if (way[3] == base.move_ability                             //������y����
			&& inf->pointInfo[a][b + way[3] * (1 - 2 * way[2])].occupied_type == 0
			&& inf->pointInfo[a][b + way[3] * (1 - 2 * way[2])].land != 2)
			inf->myCommandList.addCommand(Move, base.id, way[2], way[3]);
		else if (way[3] < base.move_ability && way[3]>0              //����ƶ�����ʣ��
			&& inf->pointInfo[a][b + way[3] * (1 - 2 * way[2])].occupied_type == 0
			&& inf->pointInfo[a][b + way[3] * (1 - 2 * way[2])].land != 2)
		{
			inf->myCommandList.addCommand(Move, base.id, way[2], way[3]);
			if (way[1] >= base.move_left)                              //ת��֮��
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

	if (base.attackable) {     //����
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
	//����Properties�еľ���ָ��



	//ѡ�����Ŀ�겢�����������ָ��
	vector<Tower> unoccupied;           //��ռ���б�
	vector<Tower> occupied;             //δռ���б�
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
	unsigned int current_attack_tower_iter = 0;     //�洢�������ĵ�����
	for (; current_attack_tower_iter < unoccupied.size(); current_attack_tower_iter++) {
		current_attack_tower[current_attack_tower_iter] = unoccupied[current_attack_tower_iter].base.id;
	}
	for (; current_attack_tower_iter < TOTAL_TOWER; current_attack_tower_iter++){
		current_attack_tower[current_attack_tower_iter] = -1;
	}

	//�ⲿ������������ָ��


	//���ҷ�ʿ���������

}