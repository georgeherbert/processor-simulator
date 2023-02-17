#include <stdint.h>

int32_t bubbleSort(int32_t arr[], int32_t n);

int32_t _start() {
    int32_t arr[7];
    arr[0] = 64;
    arr[1] = 34;
    arr[2] = 25;
    arr[3] = 12;
    arr[4] = 22;
    arr[5] = 11;
    arr[6] = 90;
    int32_t n = sizeof(arr) / sizeof(arr[0]);
    bubbleSort(arr, n);
    return 0;
}

int32_t bubbleSort(int32_t arr[], int32_t n) {
    int32_t i, j;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // swap arr[j] and arr[j+1]
                int32_t temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}