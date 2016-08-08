#include<stdio.h>
#include<iostream>
#include<string.h>
using namespace std;

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
//未初期化である事に注意
Point::Point(){}
Point::~Point(){}

enum stonecolor{
	green,
	black,
	white,
};
stonecolor board[10][10];
stonecolor turn;
void init(){
	turn = black;
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
stonecolor get_enemy(stonecolor a){
	if(a == black){
		return white;
	} else{
		return black;
	}
}
void draw(){
	cout << "  ";
	for(int x = 1; x <= 8; ++x){
		cout << x;
	}
	cout << endl;
	for(int y = 1; y <= 8; ++y){
		cout << y << " ";
		for(int x = 1; x <= 8; ++x){
			if(board[y][x] == green){
				cout << " ";
			} else{
				cout << board[y][x];
			}
		}
		cout << endl;
	}
}
void show_score(){
	int white_score = 0;
	int black_score = 0;
	for(int y = 1; y <= 8; ++y){
		for(int x = 1; x <= 8; ++x){
			switch(board[y][x]){
				case white:
					white_score++;
					break;
				case black:
					black_score++;
					break;
				default:
					break;
			}
		}
	}
	cout << "黒" << black_score << " " << "白" << white_score << endl;
}
void change(){
	if(turn == black){
		turn = white;
	} else{
		turn = black;
	}
}

Point search_sand_pair(Point start_pos, Point direc){
	//本当にこれでコピーされる?
	Point search_pos = start_pos + direc;
	if(board[search_pos.y][search_pos.x] == turn || board[search_pos.y][search_pos.x] == green){
		return start_pos;
		//returnでいいのか?
		//相手がいなかった事を検索開始位置を返すことで示す
	}

	do{
		search_pos += direc;
		if(board[search_pos.y][search_pos.x] == green){
			return start_pos;
			//returnでいいのか?
			//相手がいなかった事を検索開始位置を返すことで示す
		}
	} while(board[search_pos.y][search_pos.x] == get_enemy(turn));
	return search_pos - direc;//ゴールの一歩手前までが裏返す所!
}
bool reverse(Point start_pos, Point direc){
	Point reverse_pos = start_pos;
	Point goal = search_sand_pair(start_pos, direc);
	if(goal == start_pos){
		return false;
	}
	do{
		reverse_pos += direc;
		board[reverse_pos.y][reverse_pos.x] = turn;
	} while(!(reverse_pos == goal));
	return true;
}
bool reverse_alldirec(Point fing){
	bool ok = false;
	for(int y = -1; y <= 1; ++y){
		for(int x = -1; x <= 1; ++x){
			if(!(y == 0 && x == 0)){
				if(reverse(fing, Point(x, y))){
					ok = true;
				}
			}
		}
	}
	return ok;
}
int main(){
	init();
	while(true){
		draw();
		show_score();
		cout << turn << " turn" << endl;

		Point fing_pos;
		cin >> fing_pos.x;
		cin >> fing_pos.y;
		//スペース無しで入力出来ると便利

		if(reverse_alldirec(fing_pos)){
			board[fing_pos.y][fing_pos.x] = turn;
			change();
		} else{
			cout << "そこにはおけません!!!!!" << endl;
		}
	}
	return 0;
}