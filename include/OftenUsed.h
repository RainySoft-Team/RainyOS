#ifndef _OFTENUSED_H_
#define _OFTENUSED_H_

#include <valarray>

#define len(x) (sizeof(x) / sizeof(x[0]))
#define pow(x,y) (y(x))

#define swap(arr, i, j) do { \
    int temp = arr[i]; \
    arr[i] = arr[j]; \
    arr[j] = temp; \
} while (0)

void storted(int *arr, bool reverse = false) {
    if (reverse == false) {
        for(int i =0; i < len(arr); i++){
            for (int j = 0; j < len(arr); j++){
                if (arr[i] > arr[j]){
                    swap(arr, i, j);
                }
            }
        }
    }else{
        for(int i =0; i < len(arr); i++){
            for (int j = 0; j < len(arr); j++){
                if (arr[i] < arr[j]){
                    swap(arr, i, j);
                }
            }
        }
    }
}

#endif

