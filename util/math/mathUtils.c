#include "mathUtils.h"

long double pow2(long double number) {
    return number * number;
}

long double neg(long double number) {
    return -1 * number;
}

long double expTo(long double num, int exp){
    int i;
    long double res = num;
    for(i =  0; i < exp; i++)
        res = res * num;
    return res;
}

long double min(long double a, long double b){
    if(a < b)
        return a;
    else
        return b;
}

long double productoPunto(vector a, vector b){
    long double result;
    result = a.x*b.x + a.y*b.y + a.z*b.z;
    return result;
}
