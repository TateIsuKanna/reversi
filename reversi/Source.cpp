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
Point::Point(int x, int y){
	this->x = x;
	this->y = y;
}

Point::Point(){}
Point::~Point(){}

bool reverse_alldirec(Point, bool, stonecolor);
Point search_sand_pair(Point, Point, stonecolor);

//HACK:ここに置くのはちょっと・・・
int rating[10][10] = {{},{0,30,-12,0,-1},
{0,-12,-15,-3,-3},
{0,0,-3,0,-1},
{0,-1,-3,-1,-1}};

class reversi_AI{
public:
	reversi_AI();
	~reversi_AI();
	Point static think(){
		//ratingの初期化
		for(int y = 1; y <= 8; ++y){
			for(int x = 1; x <= 8; ++x){
				rating[y][9 - x] = rating[y][x];
				rating[9 - y][x] = rating[y][x];
				rating[9 - y][9 - x] = rating[y][x];
			}
		}

		Point max_pos;
		int max = -999999999;//HACK:
		for(int y = 1; y <= 8; ++y){
			for(int x = 1; x <= 8; ++x){
				int rate = rate_alldirec(Point(x, y), current_turn);
				if(rate > -9999){
					if(rate > max){
						max = rate;
						max_pos = Point(x, y);
					}
				}
			}
		}

		return max_pos;
	}

private:
	int static rate_reverse(Point start_pos, Point direc, stonecolor turn){
		Point reverse_pos = start_pos;
		Point goal = search_sand_pair(start_pos, direc, turn);
		if(goal == start_pos){
			//HACK:絶対やめた方が良い!
			return -9999;
		}
		int rate_sum = 0;
		do{
			rate_sum += rating[reverse_pos.y][reverse_pos.x];
			reverse_pos += direc;
		} while(!(reverse_pos == goal));
		rate_sum -= rating[reverse_pos.y][reverse_pos.x];//ゴールの分のスコア減
		return rate_sum;
	}
	int static rate_alldirec(Point fing, stonecolor turn){
		//入力範囲チェック
		if(!((1 <= fing.x && fing.x <= 8) && (1 <= fing.y && fing.y <= 8))){
			//HACK:絶対やめた方が良い!
			return -9999;
		}
		//入力チェック既においてあるか．もうちょっと良い方法あるかも．
		if(!(board[fing.y][fing.x] == green)){
			//HACK:絶対やめた方が良い!
			return -9999;
		}
		int rate_sum = 0;
		bool ok = false;
		for(int y = -1; y <= 1; ++y){
			for(int x = -1; x <= 1; ++x){
				if(!(y == 0 && x == 0)){
					int tmp = rate_reverse(fing, Point(x, y), turn);
					if(tmp > -9999){
						rate_sum += tmp;
						ok = true;
					}
				}
			}
		}
		if(!ok){
			//HACK:絶対やめた方が良い!
			return -9999;
		}
		return rate_sum;
	}

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
		cout << "$の勝ち" << endl;
	} else if(get_score(white) > get_score(black)){
		cout << "0の勝ち" << endl;
	} else{
		cout << "引き分け" << endl;
	}
}
void show_turn(){
	switch(current_turn){
		case black:
			cout << "$のturn" << endl;
			break;
		case white:
			cout << "0のturn" << endl;
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

Point search_sand_pair(Point start_pos, Point direc, stonecolor turn){
	Point search_pos = start_pos;
	do{
		search_pos += direc;
		if(board[search_pos.y][search_pos.x] == green){
			return start_pos;
			//相手がいなかった事を検索開始位置を返すことで示す
		}
	} while(board[search_pos.y][search_pos.x] == get_enemy_color(turn));
	return search_pos - direc;//ゴールの一歩手前までが裏返す所!
}
bool reverse(Point start_pos, Point direc, bool just_check, stonecolor turn){
	Point reverse_pos = start_pos;
	Point goal = search_sand_pair(start_pos, direc, turn);
	if(goal == start_pos){
		return false;
	}
	do{
		reverse_pos += direc;
		if(!just_check){
			board[reverse_pos.y][reverse_pos.x] = turn;
		}
	} while(!(reverse_pos == goal));
	return true;
}
bool reverse_alldirec(Point fing, bool just_check, stonecolor turn){
	//入力範囲チェック
	if(!((1 <= fing.x && fing.x <= 8) && (1 <= fing.y && fing.y <= 8))){
		return false;
	}
	//入力チェック既においてあるか．
	if(!(board[fing.y][fing.x] == green)){
		return false;
	}
	bool isok = false;
	for(int y = -1; y <= 1; ++y){
		for(int x = -1; x <= 1; ++x){
			//if(!(y == 0 && x == 0)){
			if(reverse(fing, Point(x, y), just_check, turn)){
				isok = true;
			}
			//}
		}
	}
	return isok;
}

bool is_need_pass(stonecolor player){
	for(int y = 1; y <= 8; ++y){
		for(int x = 1; x <= 8; ++x){
			if(reverse_alldirec(Point(x, y), true, player)){
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
		cout << "先手は? 1:入力 2:AI" << endl;
		int first_player;
		cin >> first_player;
		while(true){
			draw();
			if(is_end()){
				break;
			}
			if(is_need_pass(current_turn)){
				cout << "パス" << endl;
				change_player();
			}
			show_turn();
			Point fing_pos;
			if(current_turn == first_player){
				int temp;
				cin >> temp;
				fing_pos = Point(temp / 10, temp % 10);
			} else{
				fing_pos = reversi_AI::think();
				cout << fing_pos.x << " " << fing_pos.y << "に置いたよ" << endl << endl;
			}
			if(reverse_alldirec(fing_pos, false, current_turn)){
				board[fing_pos.y][fing_pos.x] = current_turn;
				change_player();
			} else{
				cout << "[[[そこにはおけません!!!!!]]]" << endl;
			}
			show_score();
		}
		cout << "終了" << endl;
		cout << endl;
		show_winner();
		cout << "-----" << endl;
	}
	return 0;
}