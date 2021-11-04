#include <stdlib.h>

int init_random(int _seed){
    srand((unsigned) _seed);
}

int getRandom(int _rangeStart, int _rangeEnd){

    return (rand() % (_rangeEnd - _rangeStart)) + _rangeStart;
}