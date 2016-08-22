//TODO:任意の絵を出せたらとっても楽しい


#include<stdio.h>
#include<iostream>
#include<memory.h>
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

bool reverse_alldirec(Point, bool, stonecolor, stonecolor[10][10]);
Point search_sand_pair(Point, Point, stonecolor, stonecolor[10][10]);
stonecolor get_enemy_color(stonecolor);

const int think_depth = 6;

//HACK:ここに置くのはちょっと・・・
int rating[10][10] = {{},{0,30,-12,0,-1},
{0,-12,-15,-3,-3},
{0,0,-3,0,-1},
{0,-1,-3,-1,-1}};


//一応動くけど思ったほど強くない
class reversi_minimaxAI{
public:
	Point static think(){
		Point good_pos;
		int tesu = 0;
		cout << "思考中...";
		minimax(board, current_turn, think_depth, &good_pos, &tesu);
		cout << endl << tesu << "通り考えました" << endl;
		return good_pos;
	}
	int static rate(stonecolor board_status[10][10], stonecolor turn){
		int rate_sum = 0;
		int blank = 0;
			for(int y = 1; y <= 8; ++y){
				for(int x = 1; x <= 8; ++x){
					switch(board_status[y][x]){
						case black:
							if(turn == black){
								rate_sum += rating[y][x];
							} else{
								rate_sum -= rating[y][x];
							}
							break;
						case white:
							if(turn == white){
								rate_sum += rating[y][x];
							} else{
								rate_sum -= rating[y][x];
							}
							break;
						default:
							//blank++;
							break;
					}
				}
			}
		if(blank < think_depth){
			for(int y = 1; y <= 8; ++y){
				for(int x = 1; x <= 8; ++x){
					if(board_status[y][x] == turn){
						rate_sum += 2;
					}
				}
			}
		}
		return rate_sum;
	}
	int static minimax(stonecolor board_status[10][10], stonecolor turn, int depth, Point *good_pos, int *yosotesu){
		if(depth == 0){
			return rate(board_status, turn);
		}
		if(turn != current_turn){
			int max = INT_MIN;
			bool ok = false;
			for(int y = 1; y <= 8; ++y){
				for(int x = 1; x <= 8; ++x){
					stonecolor new_board[10][10];
					for(int ty = 0; ty < 10; ++ty){
						for(int tx = 0; tx < 10; ++tx){
							new_board[ty][tx] = board_status[ty][tx];
						}
					}
					if(reverse_alldirec(Point(x, y), false, turn, new_board)){
						new_board[y][x] = turn;//TODO:これが必要なのはreverse_alldirecの仕様がおかしい
						(*yosotesu)++;
						ok = true;
						int score = minimax(new_board, get_enemy_color(turn), depth - 1, good_pos, yosotesu);
						if(score > max){
							max = score;
							if(depth == think_depth){
								*good_pos = Point(x, y);
							}
						}
					}
				}
			}
			if(!ok){
				return rate(board_status, turn);
			}
			return max;
		} else{
			bool ok = false;
			int min = INT_MAX;
			for(int y = 1; y <= 8; ++y){
				for(int x = 1; x <= 8; ++x){
					stonecolor new_board[10][10];
					for(int ty = 0; ty < 10; ++ty){
						for(int tx = 0; tx < 10; ++tx){
							new_board[ty][tx] = board_status[ty][tx];
						}
					}
					if(reverse_alldirec(Point(x, y), false, turn, new_board)){
						new_board[y][x] = turn;//TODO:これが必要なのはreverse_alldirecの仕様がおかしい
						(*yosotesu)++;
						ok = true;
						int score = minimax(new_board, get_enemy_color(turn), depth - 1, good_pos, yosotesu);
						if(score < min){
							min = score;
							if(depth == think_depth){
								*good_pos = Point(x, y);
							}
						}
					}
				}
			}
			if(!ok){
				return rate(board_status, turn);
			}
			return min;
		}
	}
};

class reversi_AI{
public:
	Point static think(){
		Point max_pos;
		int max = INT_MIN;
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
		Point goal = search_sand_pair(start_pos, direc, turn, board);
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

Point search_sand_pair(Point start_pos, Point direc, stonecolor turn, stonecolor dist_board[10][10]){
	Point search_pos = start_pos;
	do{
		search_pos += direc;
		if(dist_board[search_pos.y][search_pos.x] == green){
			return start_pos;
			//相手がいなかった事を検索開始位置を返すことで示す
		}
	} while(dist_board[search_pos.y][search_pos.x] == get_enemy_color(turn));
	return search_pos - direc;//ゴールの一歩手前までが裏返す所!
}
bool reverse(Point start_pos, Point direc, bool just_check, stonecolor turn, stonecolor dist_board[10][10]){
	Point reverse_pos = start_pos;
	Point goal = search_sand_pair(start_pos, direc, turn, dist_board);
	if(goal == start_pos){
		return false;
	}
	do{
		reverse_pos += direc;
		if(!just_check){
			dist_board[reverse_pos.y][reverse_pos.x] = turn;
		}
	} while(!(reverse_pos == goal));
	return true;
}
bool reverse_alldirec(Point fing, bool just_check, stonecolor turn, stonecolor dist_board[10][10]){
	//範囲チェック
	if(!((1 <= fing.x && fing.x <= 8) && (1 <= fing.y && fing.y <= 8))){
		return false;
	}
	//既においてあるか．
	if(!(dist_board[fing.y][fing.x] == green)){
		return false;
	}
	bool isok = false;
	for(int y = -1; y <= 1; ++y){
		for(int x = -1; x <= 1; ++x){
			if(reverse(fing, Point(x, y), just_check, turn, dist_board)){
				isok = true;
			}
		}
	}
	return isok;
}

bool is_need_pass(stonecolor player){
	for(int y = 1; y <= 8; ++y){
		for(int x = 1; x <= 8; ++x){
			if(reverse_alldirec(Point(x, y), true, player, board)){
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
	//ratingの初期化．鏡写し
	for(int y = 1; y <= 8; ++y){
		for(int x = 1; x <= 8; ++x){
			rating[y][9 - x] = rating[y][x];
			rating[9 - y][x] = rating[y][x];
			rating[9 - y][9 - x] = rating[y][x];
		}
	}
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
				//temp = rand() % 89;
				fing_pos = Point(temp / 10, temp % 10);
				//fing_pos = reversi_AI::think();
				//fing_pos = reversi_minimaxAI::think();
			} else{
				fing_pos = reversi_AI::think();
				//fing_pos = reversi_minimaxAI::think();
				cout << fing_pos.x << " " << fing_pos.y << "に置いたよ" << endl << endl;
			}
			if(reverse_alldirec(fing_pos, false, current_turn, board)){
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
