#include <stdio.h>



int binarySearch(int arr[], int size, int target) {
    int left = 0;                  // 左边界索引
    int right = size - 1;          // 右边界索引

    while (left <= right) {
        int mid = left + (right - left) / 2;  // 避免溢出的中间索引计算[1,3,5](@ref)

        if (arr[mid] == target) {
            return mid;             // 找到目标，返回索引
        } else if (arr[mid] < target) {
            left = mid + 1;         // 目标在右半区
        } else {
            right = mid - 1;        // 目标在左半区
        }
    }
    return -1;  // 未找到返回-1
}

// 递归辅助函数
int binarySearchRecursive(int arr[], int left, int right, int target) {
    if (left > right) return -1;  // 终止条件：区间无效[3](@ref)

    int mid = left + (right - left) / 2;
    if (arr[mid] == target) {
        return mid;
    } else if (arr[mid] < target) {
        return binarySearchRecursive(arr, mid + 1, right, target);  // 搜索右半区
    } else {
        return binarySearchRecursive(arr, left, mid - 1, target);   // 搜索左半区
    }
}

// 封装接口
int binarySearch(int arr[], int size, int target) {
    return binarySearchRecursive(arr, 0, size - 1, target);
}



void bubbleSort(int arr[], int n) {
    int i, j, temp;
    int swapped;  // 交换标志位（优化关键）

    for (i = 0; i < n - 1; i++) {      // 外层循环控制排序轮次
        swapped = 0;                    // 每轮开始时重置标志位
        for (j = 0; j < n - i - 1; j++) {  // 内层循环控制比较范围
            if (arr[j] > arr[j + 1]) {     // 若前>后则交换（升序）
                // 元素交换
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;            // 标记发生交换[3,6](@ref)
            }
        }
        if (swapped == 0) break;        // 本轮无交换则提前终止[1,3](@ref)
    }
}

int a
void main()
{
  int *p;
  p=(int*)malloc(10*sizeof(100))
}