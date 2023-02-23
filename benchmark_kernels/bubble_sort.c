#include <stdint.h>

int32_t bubbleSort(int32_t arr[], int32_t n);

int32_t _start() {
    int32_t arr[1000];
    // Create an array of length 100 descending
    for (int32_t i = 0; i < 1000; i++) {
        arr[i] = 1000 - i;
    }
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