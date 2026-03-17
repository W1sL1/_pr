#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_N 24
typedef int my_bool; //error 
#define true 1
#define false 0
typedef struct {int x;int y;} Point;
//size  +fig  fig alredy
int N; int L; int K;       
//orig fig            //mask of boat: true - busy cell, false - free cell
Point initial[MAX_N]; my_bool board[MAX_N][MAX_N];
//(all fig: orig + new)   
Point solution[MAX_N];   int solution_count = 0;  FILE* output_file;
//knight=horse 
const int knight_dx[] = { -2, -1, 1, 2, 2, 1, -1, -2 };
const int knight_dy[] = { 1, 2, 2, 1, -1, -2, -2, -1 };
//queen=ferz 
const int queen_dx[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
const int queen_dy[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
//chek point on boat 
my_bool is_valid(int x, int y) {
    return x >= 0 && x < N && y >= 0 && y < N;
}
//from (x, y)
void attack_from(int x, int y, my_bool temp_board[MAX_N][MAX_N], my_bool add) {
    //knight attack
    for (int i = 0; i < 8; i++) {
        int nx = x + knight_dx[i]; int ny = y + knight_dy[i];
        if (is_valid(nx, ny)) temp_board[nx][ny] = temp_board[nx][ny] || add;
    }
    //queen attack
    for (int dir = 0; dir < 8; dir++) {
        for (int step = 1; step < N; step++) {
            int nx = x + queen_dx[dir] * step; int ny = y + queen_dy[dir] * step;
            if (!is_valid(nx, ny)) break;
            temp_board[nx][ny] = temp_board[nx][ny] || add;
        }
    }
    //this cell is busy too 
    temp_board[x][y] = temp_board[x][y] || add;
}
void init_board() {
    //clean boat
    memset(board, 0, sizeof(board));
    //add orig fig
    for (int i = 0; i < K; i++) {
        int x = initial[i].x; int y = initial[i].y;
        if (board[x][y]) {printf("Error: Initial pieces attack each other!\n");exit(1);}
        //add fig and attack
        attack_from(x, y, board, true);
    }
}
//check
my_bool can_place(int x, int y) {
    return !board[x][y]; 
}
//recurs
void place_pieces(int placed, int start_x, int start_y) {
    if (placed == L) {
        for (int i = 0; i < K + L; i++) {
            fprintf(output_file, "(%d,%d)", solution[i].x, solution[i].y);
            if (i < K + L - 1) fprintf(output_file, " ");
        }
        fprintf(output_file, "\n"); solution_count++;
        return;
    }
    //begin (start_x, start_y), enum all cells
    for (int x = start_x; x < N; x++) {
        int y_start = (x == start_x) ? start_y : 0;
        for (int y = y_start; y < N; y++) {
            //check
            if (can_place(x, y)) {
                //temp copy of boat 
                my_bool temp_board[MAX_N][MAX_N];
                memcpy(temp_board, board, sizeof(board));
                //plus new fig 
                attack_from(x, y, board, true);
                //save fig in solution 
                solution[K + placed].x = x;
                solution[K + placed].y = y;
                // recurse add next figs 
                if (y + 1 < N) place_pieces(placed + 1, x, y + 1);
                else place_pieces(placed + 1, x + 1, 0);
                //hill boat (restore)
                memcpy(board, temp_board, sizeof(board));
            }
        }
    }
}
int main() {
    FILE* input_file = fopen("input.txt", "r");
    if (!input_file) {printf("Error: Cannot open input.txt\n");return 1;}
    fscanf(input_file, "%d %d %d", &N, &L, &K);
    if (N < 1 || N > MAX_N || K < 0 || K > N * N || L < 0 || L > N * N - K) {
        printf("Error: Invalid input parameters\n");
        fclose(input_file); return 1;
    }
    //read orig fig
    for (int i = 0; i < K; i++) {
        fscanf(input_file, "%d %d", &initial[i].x, &initial[i].y);
        if (initial[i].x < 0 || initial[i].x >= N || initial[i].y < 0 || initial[i].y >= N) {
            printf("Error: Invalid piece coordinates\n");
            fclose(input_file);
            return 1;
        }
        solution[i] = initial[i];
    }
    fclose(input_file);
    init_board();
    output_file = fopen("output.txt", "w");
    if (!output_file) {printf("Error: Cannot open output.txt\n");return 1;}
    //start recurse search 
    if (L == 0) {
        //if not necessary add figs, print start config 
        if (K > 0) {
            for (int i = 0; i < K; i++) {
                fprintf(output_file, "(%d,%d)", solution[i].x, solution[i].y);
                if (i < K - 1) {
                    fprintf(output_file, " ");
                }
            }
            fprintf(output_file, "\n");
            solution_count++;
        }
        else {
            //nothing on board 
        }
    }
    else place_pieces(0, 0, 0);
    if (solution_count == 0) fprintf(output_file, "no solutions\n");
    fclose(output_file);
    return 0;
}