#include<stdio.h>
#include<iostream>
using namespace std;

enum stonecolor{
	green,
	black,
	white,
};
stonecolor board[10][10];
stonecolor current_turn;
class Point{
public:
	Point(int, int);
	Point();
	~Point();
	Point operator -(Point obj){
		Point temp(x, y);
		temp.x -= obj.x;
		temp.y -= obj.y;
		return temp;
	}
	Point operator +(Point obj){
		Point temp(x, y);
		temp.x += obj.x;
		temp.y += obj.y;
		return temp;
	}
	void operator +=(Point obj){
		x += obj.x;
		y += obj.y;
	}
	bool operator ==(Point obj){
		if(x == obj.x && y == obj.y){
			return true;
		} else{
			return false;
		}
	}
	int x;
	int y;

private:

};
Point::Point(int xv, int yv){
	x = xv;
	y = yv;
}
//���������ł��鎖�ɒ���
Point::Point(){}
Point::~Point(){}

int reverse_alldirec(Point, bool, stonecolor);

class reversi_AI{
public:
	reversi_AI();
	~reversi_AI();
	Point static think(){
		int rating[10][10] = {{},{0,30,-12,0,-1},
		{0,-12,-15,-3,-3},
		{0,0,-3,0,-1},
		{0,-1,-3,-1,-1}};
		for(int y = 1; y <= 8; ++y){
			for(int x = 1; x <= 8; ++x){
				rating[y][9 - x] = rating[y][x];
				rating[9 - y][x] = rating[y][x];
				rating[9 - y][9 - x] = rating[y][x];
			}
		}

		Point max_pos;
		int max = -1000;//HACK:
		for(int y = 1; y <= 8; ++y){
			for(int x = 1; x <= 8; ++x){
				if(reverse_alldirec(Point(x, y), true, current_turn) > 0){
					if(rating[y][x] > max){
						max = rating[y][x];
						max_pos = Point(x, y);
					}
				}
			}
		}

		return max_pos;
	}

private:
};
reversi_AI::reversi_AI(){}
reversi_AI::~reversi_AI(){}

void init(){
	current_turn = black;
	for(int y = 0; y < 10; ++y){
		for(int x = 0; x < 10; ++x){
			board[y][x] = green;
		}
	}
	board[4][4] = white;
	board[4][5] = black;
	board[5][4] = black;
	board[5][5] = white;
}
stonecolor get_enemy_color(stonecolor a){
	if(a == black){
		return white;
	} else{
		return black;
	}
}
void draw(){
	cout << "  ";
	for(int x = 1; x <= 8; ++x){
		cout << x << "|";
	}
	cout << endl;
	for(int y = 1; y <= 8; ++y){
		cout << y << " ";
		for(int x = 1; x <= 8; ++x){
			switch(board[y][x]){
				case white:
					cout << "0|";
					break;
				case black:
					cout << "$|";
					break;
				default:
					cout << " |";
					break;
			}
		}
		cout << endl;
	}
}
int get_score(stonecolor color){
	int score = 0;
	for(int y = 1; y <= 8; ++y){
		for(int x = 1; x <= 8; ++x){
			if(board[y][x] == color){
				++score;
			}
		}
	}
	return score;
}
void show_score(){
	cout << "$:" << get_score(black) << " " << "0:" << get_score(white) << endl;
}
void show_winner(){
	if(get_score(black) > get_score(white)){
		cout << "$�̏���" << endl;
	} else if(get_score(white) > get_score(black)){
		cout << "0�̏���" << endl;
	} else{
		cout << "��������" << endl;
	}
}
void show_turn(){
	switch(current_turn){
		case black:
			cout << "$��turn" << endl;
			break;
		case white:
			cout << "0��turn" << endl;
			break;
		default:
			break;
	}
}
void change_player(){
	if(current_turn == black){
		current_turn = white;
	} else{
		current_turn = black;
	}
}

//TODO:search_sand_pair��reverse�܂Ƃ߂ꂻ������Ȃ�?
Point search_sand_pair(Point start_pos, Point direc, stonecolor turn){
	//Point search_pos = start_pos + direc;
	//if(board[search_pos.y][search_pos.x] == turn || board[search_pos.y][search_pos.x] == green){
	//	return start_pos;
	//	//�Ƃ�Ȃ��������������J�n�ʒu��Ԃ����ƂŎ���
	//}
	Point search_pos = start_pos;
	do{
		search_pos += direc;
		if(board[search_pos.y][search_pos.x] == green){
			return start_pos;
			//���肪���Ȃ��������������J�n�ʒu��Ԃ����ƂŎ���
		}
	} while(board[search_pos.y][search_pos.x] == get_enemy_color(turn));
	return search_pos - direc;//�S�[���̈����O�܂ł����Ԃ���!
}
int reverse(Point start_pos, Point direc, bool just_check, stonecolor turn){
	Point reverse_pos = start_pos;
	Point goal = search_sand_pair(start_pos, direc, turn);
	if(goal == start_pos){
		return 0;
	}
	int reverse_sum = 0;
	do{
		reverse_pos += direc;
		if(!just_check){
			board[reverse_pos.y][reverse_pos.x] = turn;
		}
		reverse_sum++;
	} while(!(reverse_pos == goal));
	return reverse_sum;
}
int reverse_alldirec(Point fing, bool just_check, stonecolor turn){
	//���͔͈̓`�F�b�N
	if(!((1 <= fing.x && fing.x <= 8) && (1 <= fing.y && fing.y <= 8))){
		return 0;
	}
	if(!(board[fing.y][fing.x] == green)){
		return 0;
	}

	int reverse_sum = 0;
	for(int y = -1; y <= 1; ++y){
		for(int x = -1; x <= 1; ++x){
			if(!(y == 0 && x == 0)){
				reverse_sum += reverse(fing, Point(x, y), just_check, turn);
			}
		}
	}
	return reverse_sum;
}

bool is_need_pass(stonecolor player){
	for(int y = 1; y <= 8; ++y){
		for(int x = 1; x <= 8; ++x){
			if(reverse_alldirec(Point(x, y), true, player) > 0){
				return false;
			}
		}
	}
	return true;
}
bool is_end(){
	if(is_need_pass(white) && is_need_pass(black)){
		return true;
	}
	return false;
}

int main(){
	while(true){
		init();
		cout << "����?" << endl;
		int first_player;
		//HACK:���l�Ŏ󂯂Ƃ�������Ă�
		cin >> first_player;
		while(true){
			draw();
			if(is_end()){
				break;
			}
			if(is_need_pass(current_turn)){
				cout << "�p�X" << endl;
				change_player();
			}
			show_turn();
			Point fing_pos;
			if(current_turn == first_player){
				//HACK:������҂�HACK
				int temp;
				cin >> temp;
				fing_pos = Point(temp / 10, temp % 10);
			} else{
				fing_pos = reversi_AI::think();
				cout << fing_pos.x << " " << fing_pos.y << "�ɒu������" << endl << endl;
			}
			if(reverse_alldirec(fing_pos, false, current_turn) > 0){
				board[fing_pos.y][fing_pos.x] = current_turn;
				change_player();
			} else{
				cout << "[[[�����ɂ͂����܂���!!!!!]]]" << endl;
			}
			show_score();
		}
		cout << "�I��" << endl;
		cout << endl;
		show_winner();
		cout << "-----" << endl;
	}
	return 0;
}