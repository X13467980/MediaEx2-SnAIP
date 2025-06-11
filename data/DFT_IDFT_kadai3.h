#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void DFT(int size, double *xr, double *xi) // 離散フーリエ変換（DFT）関数
{
    double *Xr = (double *)calloc(size, sizeof(double)); // 実部を格納する配列
    double *Xi = (double *)calloc(size, sizeof(double)); // 虚部を格納する配列

    for (int k = 0; k < size; ++k) // 各周波数成分
    {
        for (int n = 0; n < size; ++n) // 各時間サンプル
        {
            double angle = 2.0 * M_PI * k * n / size;          // 位相角の計算
            Xr[k] += xr[n] * cos(angle) + xi[n] * sin(angle);  // 実部の計算
            Xi[k] += -xr[n] * sin(angle) + xi[n] * cos(angle); // 虚部の計算
        }
    }

    memcpy(xr, Xr, size * sizeof(double)); // 元の配列に結果をコピー
    memcpy(xi, Xi, size * sizeof(double)); // 元の配列に結果をコピー

    free(Xr); // メモリ解放
    free(Xi); // メモリ解放
}

void IDFT(int size, double *Xr, double *Xi) // 逆離散フーリエ変換（IDFT）関数
{
    double *xr = (double *)calloc(size, sizeof(double)); // 実部を格納する配列
    double *xi = (double *)calloc(size, sizeof(double)); // 虚部を格納する配列

    for (int n = 0; n < size; ++n) // 各時間サンプル
    {
        for (int k = 0; k < size; ++k) // 各周波数成分
        {
            double angle = 2.0 * M_PI * k * n / size;         // 位相角の計算
            xr[n] += Xr[k] * cos(angle) - Xi[k] * sin(angle); // 実部の計算
            xi[n] += Xr[k] * sin(angle) + Xi[k] * cos(angle); // 虚部の計算
        }
        xr[n] /= size; // 正規化
        xi[n] /= size; // 正規化
    }

    memcpy(Xr, xr, size * sizeof(double)); // 元の配列に結果をコピー
    memcpy(Xi, xi, size * sizeof(double)); // 元の配列に結果をコピー
    free(xr);                              // メモリ解放
    free(xi);                              // メモリ解放
}