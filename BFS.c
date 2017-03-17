//
//
//  P1: This program takes in a rectangular puzzle (in the form of a 1D character array) and uses Breadth First Seach to find the best path to the solution. Each position is a snapshot of a move in the solution.
//
//  Created by Zachary Shale Weiss on 11/28/16.
//  Copyright © 2016 Zachary Shale Weiss. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "Puzzle.h"
#include "Output.h"


/******************************************************************************************************************************
 Position Data Type
 ******************************************************************************************************************************/

/*Position data type.*/
typedef struct pnode {
    struct pnode *next; //Pointer to next position (For HashTable)
    struct pnode *back; //Pointer to previous position (For printing result)
    int step; //How many steps it took to get to this position
    char piece; //Piece that moved
    int dir; //Direction of move
    char board[BoardSize]; //For printing the output at the end
    char board2d[BoardHeight][BoardWidth]; //To find the possible moves in an intuitive way
} TypePosition;

/*Allocate memory for a new position.*/
TypePosition *NEWPOSITIONMEM() {
    TypePosition * p = malloc(sizeof(TypePosition));
    if (p==NULL) {
        printf("Malloc for a new position failed.");
        exit(1);
    }
    p->next = NULL;
    p->back = NULL;
    p->step = 0;
    p->piece = 0;
    p->dir = 0;
    return p;
}

//FOR FIRST AND LAST POSITIONS
TypePosition * DEFAULTPOSITION(char * gameboard) {
    TypePosition * position = NEWPOSITIONMEM();
    for (int i=0; i<BoardSize; i++){
        position->board[i] = gameboard[i];
    }
    for (int i = 0; i<BoardHeight; i++) {
        for (int j = 0; j<BoardWidth; j++) {
            position->board2d[i][j] = position->board[j%BoardWidth+i*BoardWidth];
        }
    }
    position->step = 0;
    position->dir = 0;
    position->piece = 'Z';
    return position;
}

//EVERY OTHER POSITION
TypePosition *NEWPOSITION(char gameboard[BoardSize], int step, int dir, char piece) {
    TypePosition * position = NEWPOSITIONMEM();
    for (int i=0; i<BoardSize; i++){
        position->board[i] = gameboard[i];
    }
    for (int i = 0; i<BoardHeight; i++) {
        for (int j = 0; j<BoardWidth; j++) {
            position->board2d[i][j] = position->board[j%BoardWidth+i*BoardWidth];
        }
    }
    position->step = step;
    position->dir = dir;
    position->piece = piece;
    position->next = NULL;
    return position;
}

//Checking if move is valid (piece == '.') moves 1 = true, 0 = false
int POSITIONEXISTS(char piece){
    //if (piece == NULL) return 0;
    if (piece == '.')
        return 1;
    else return 0;
}

//Checks if piece is valid (Not '.' or '$')
int VALIDPIECE(int i, int j, char piece){
    if (i < 0 || i >= BoardHeight || j < 0 || j >= BoardWidth) return 0;
    if (piece != '$' && piece != '.')
        return 1;
    else return 0;
}

//Check if two positions are equal 1 = true, 0 = false
int SAMEBOARD(TypePosition *pos1, TypePosition *pos2){
    if (pos1 == NULL || pos2 == NULL) return 0;
    for(int i=0; i<BoardSize; i++){
        if(pos1->board[i] != pos2->board[i]) {
            return 0;
        }
    }
    return 1;
}

//This is the function that makes the moves.
void SWAP(char newboard[BoardSize], char board[BoardHeight][BoardWidth], int i, int j, int dir) {
    char board2[BoardHeight][BoardWidth];
    for (int i = 0; i<BoardHeight; i++) {
        for (int j = 0; j<BoardWidth; j++) {
            board2[i][j] = board[i][j];
        }
    }
    if (dir == 0){ //North
        board2[i-1][j] = board[i][j];
        board2[i][j] = '.';
    } else if (dir == 1) { //East
        board2[i][j+1] = board[i][j];
        board2[i][j] = '.';
    } else if (dir == 2) { //South
        board2[i+1][j] = board[i][j];
        board2[i][j] = '.';
    }else { //West
        board2[i][j-1] = board[i][j];
        board2[i][j] = '.';
    }
    int k = 0;
    for (int i = 0; i<BoardHeight; i++) {
        for (int j = 0; j<BoardWidth; j++) {
            newboard[k++] = board2[i][j];
        }
    }
}

//Checks for all posible moves from the current position and returns index for looping
int GETPOSSIBLEMOVES(TypePosition * ValidMoves[BoardWidth * BoardHeight], TypePosition * position) {
    int index = 0;
    int step = position->step + 1;
    for (int i = 0; i<BoardHeight; i++) {
        for (int j = 0; j<BoardWidth; j++) {
            char piece = position->board2d[i][j];
            if (VALIDPIECE(i-1, j, piece) == 1 && POSITIONEXISTS(position->board2d[i-1][j]) == 1) { //NORTH
                char newboard[BoardSize];
                SWAP(newboard,position->board2d, i, j, 0);
                ValidMoves[index++] = NEWPOSITION(newboard, step, 0, piece);
            }
            if (VALIDPIECE(i, j+1, piece) == 1 && POSITIONEXISTS(position->board2d[i][j+1]) == 1) { //EAST
                char newboard[BoardSize];
                SWAP(newboard,position->board2d, i, j, 1);
                ValidMoves[index++] = NEWPOSITION(newboard, step, 1, piece);
            }
            if (VALIDPIECE(i+1, j, piece) == 1 && POSITIONEXISTS(position->board2d[i+1][j]) == 1) { //SOUTH
                char newboard[BoardSize];
                SWAP(newboard,position->board2d, i, j, 2);
                ValidMoves[index++] = NEWPOSITION(newboard, step, 2, piece);
            }
            if (VALIDPIECE(i, j-1, piece) == 1 && POSITIONEXISTS(position->board2d[i][j-1]) == 1) { //WEST
                char newboard[BoardSize];
                SWAP(newboard,position->board2d, i, j, 3);
                ValidMoves[index++] = NEWPOSITION(newboard, step, 3, piece);
            }
        }
    }
    return index;
}

/******************************************************************************************************************************
 Position Data Type (End)
 ******************************************************************************************************************************/

//NEXT SECTION//

/******************************************************************************************************************************
 HashTable Data Type
 ******************************************************************************************************************************/

//HASHTABLE
typedef struct htbody{
    TypePosition * HashTable[HashArraySize];
    int ActiveBuckets;
}HashTable;

/*Allocate memory for a new position.*/
HashTable *NEWHASHTABLE() {
    HashTable * ht = malloc(sizeof(HashTable));
    if (ht==NULL) {
        printf("Malloc for a new position failed.");
        exit(1);
    }
    ht->ActiveBuckets = 0;
    for (int i =0; i<HashArraySize; i++){
        ht->HashTable[i] = NULL;
    }
    return ht;
}

//HASH FUNCTION
int HASH(TypePosition * position){
    int key = 0;
    int weight[12] = {107, 241, 17, 19, 23, 29,211, 223, 397,109,251,401}; //Assuming no board greater than 12 is used.
    for (int i = 0; i<BoardSize; i++) {
        int piece = position->board[i];
        int j = i;
        if (i>12) j %=12;
        key = (key + piece*weight[j]*i*i);
    }
    key = key % HashArraySize;
    return key;
}

//checks to see if position is already in hashtable 1 = true, 0 = false
int MEMBER(HashTable * ht, TypePosition * position) {
    int key = HASH(position);
    TypePosition * bucket = ht->HashTable[key];
    if (bucket == '\0') return 0;
    while (bucket != NULL) {
        if (SAMEBOARD(bucket, position) == 1){
            
            return 1;
        }
        bucket = bucket->next;
    }
    return 0;
}

//insert into hash table;
void INSERT(HashTable * ht, TypePosition * position) {
    int key = HASH(position);
    if (MEMBER(ht, position) == 0) {
        position->next = ht->HashTable[key];
        ht->HashTable[key] = position;
        ht->ActiveBuckets++;
    }
}

/******************************************************************************************************************************
 HashTable Data Type (End)
 ******************************************************************************************************************************/

//NEXT SECTION//

/******************************************************************************************************************************
 Queue Data Type
 ******************************************************************************************************************************/

//QUEUE Struct
typedef struct qbody{
    TypePosition * queue[QueueArraySize];
    int size;
    int front;
    int rear;
    int MaxSize;
}Queue;


/*Allocate memory for a new position.*/
Queue *NEWQUEUE() {
    Queue * p = malloc(sizeof(Queue));
    if (p==NULL) {
        printf("Malloc for a new position failed.");
        exit(1);
    }
    p->size = 0;
    p->front = 0;
    p->rear = 0;
    p->MaxSize = 0;
    for (int i =0; i<QueueArraySize; i++){
        p->queue[i] = NULL;
    }
    return p;
}

//Enqueue function
void ENQUEUE(Queue* q, TypePosition* position) {
    if (q->size >= QueueArraySize) {
        printf("QUEUE IS FULL!\n");
    } else {
        int rear = q->rear;
        q->queue[rear] = position;
        q->rear++;
        if (rear == QueueArraySize-1) {
            q->rear = 0;
        }
        q->size++;
        if (q->size > q->MaxSize) q->MaxSize = q->size;
    }
}

//Dequeue function
TypePosition * DEQUEUE(Queue* q) {
    if (q->size == 0) {
        printf("QUEUE IS EMPTY!\n");
    } else {
        int front = q->front;
        TypePosition * temp = q->queue[front];
        q->front++;
        if (front == QueueArraySize-1) {
            q->front = 0;
        }
        q->size--;
        return temp;
    }
    return NULL;
}

/******************************************************************************************************************************
 Queue Data Type (End)
 ******************************************************************************************************************************/

//NEXT SECTION//

/******************************************************************************************************************************
 Functions to output a solution and associated statistics.
 ******************************************************************************************************************************/

//For printing bucket statistics at the end. Search's entire hashtbale for buckets of a certain size.
int BUCKETSOFSIZE(int i, HashTable * ht) {
    int buckets = 0;
    for (int j=0; j<HashArraySize; j++) {
        TypePosition *  p = ht->HashTable[j];
        int size = 0;
        while (p!=NULL){
            size++;
            p=p->next;
        }
        if (size == i) buckets++;
    }
    return buckets;
}

//For printing bucket statistics at the end. Gets indiviual bucket size.
int BUCKETSIZE(TypePosition * bucket) {
    int size = 0;
    while (bucket!=NULL) {
        size++;
        bucket = bucket->next;
    }
    return size;
}

//Prints out statistics for queue, hashtable, and buckets.
void PRINTSTATS(Queue * q, HashTable * ht){
    QueueStats(QueueArraySize, q->MaxSize, q->front, q->rear);
    int MaxBucketSize = 0;
    for (int j = 0; j<HashArraySize; j++){
        int size = BUCKETSIZE(ht->HashTable[j]);
        if (size > MaxBucketSize) MaxBucketSize = size;
    }
    HashStats(HashArraySize, ht->ActiveBuckets, MaxBucketSize);
    for (int i=0; i<=MaxBucketSize; i++){
        BucketStat(i, BUCKETSOFSIZE(i, ht));
    }
}

//Reverses back pointers to show solution in order.
TypePosition *REVERSELIST(TypePosition * L){
    TypePosition * previous = NULL;
    TypePosition * current = L;
    while (current != NULL) {
        TypePosition * temp = current->back;
        current->back = previous;
        previous = current;
        current = temp;
    }
    return previous;
}

//Prints solution after BFT
void PRINTSOLUTION(TypePosition * end){
    end = REVERSELIST(end);
    while (end != NULL) {
        OutputPosition(end->board, end->step, end->piece, end->dir);
        end = end->back;
    }
}

/******************************************************************************************************************************
 Functions to output a solution and associated statistics. (End)
 ******************************************************************************************************************************/

//NEXT SECTION//

/******************************************************************************************************************************
 Breadth First Search (BFS)
 ******************************************************************************************************************************/

//Breadth-first search algorthm
TypePosition * BFS(Queue* q, HashTable *ht){
    TypePosition * StartPosition = DEFAULTPOSITION(StartBoard);
    TypePosition * EndPosition = DEFAULTPOSITION(GoalBoard);
    INSERT(ht, StartPosition);
    ENQUEUE(q, StartPosition);
    while (q->size > 0){
        TypePosition * position = DEQUEUE(q);
        //Stop if you hit the goal.
        if (SAMEBOARD(position, EndPosition)) {
            EndPosition = position;
            break;
        }
        //Checks for all valid moves.
        TypePosition * ValidMoves[BoardWidth * BoardHeight];
        int size = GETPOSSIBLEMOVES(ValidMoves, position);
        for (int i =0; i<size; i++) {
            TypePosition * temp = ValidMoves[i];
            if (MEMBER(ht, temp) == 0){
                temp->back = position;
                INSERT(ht, temp);
                ENQUEUE(q, temp);
            }
        }
    }
    return EndPosition;
}

/******************************************************************************************************************************
 Breadth First Search (BFS) - END
 ******************************************************************************************************************************/

int main() {
    Queue * q = NEWQUEUE();
    HashTable * ht = NEWHASHTABLE();
    TypePosition * solution = BFS(q, ht);
    PRINTSOLUTION(solution);
    PRINTSTATS(q, ht);
    return 0;
}
