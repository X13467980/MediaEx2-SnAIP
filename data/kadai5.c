#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "DFT_IDFT_kadai3.h"  // 課題3のDFT関数をインクルード

#define PI M_PI
#define CUTOFF 0.4
#define DFT_SIZE 1024

// フィルタ係数 h[n] を計算
double calc_h(int n, int N) {
    int mid = N / 2;
    if (n == mid) {
        return CUTOFF; // 特異点：sin(0)/0 → CUTOFF
    } else {
        double x = PI * (n - mid);
        return sin(CUTOFF * x) / x;
    }
}

// スペクトルを dB に変換して保存
void save_spectrum(const char *filename, double *xr, double *xi, int size) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("スペクトルファイル書き込み失敗");
        exit(1);
    }

    for (int k = 0; k < size / 2; ++k) {  // Nyquistまで
        double magnitude = sqrt(xr[k] * xr[k] + xi[k] * xi[k]);
        double gain_db = 20.0 * log10(magnitude + 1e-12); // log(0)対策
        double freq = (double)k / (size / 2);
        fprintf(fp, "%.4f %.4f\n", freq, gain_db);
    }

    fclose(fp);
}

// N次フィルタの処理（係数保存 & DFT実行）
void process_filter(int N) {
    char impulse_file[64], spectrum_file[64];
    sprintf(impulse_file, "impulse_N%d.txt", N);
    sprintf(spectrum_file, "spectrum_N%d.txt", N);

    double xr[DFT_SIZE] = {0}; // 実部：h[n] + zero-padding
    double xi[DFT_SIZE] = {0}; // 虚部：0

    // インパルス応答の生成と書き出し
    FILE *fp = fopen(impulse_file, "w");
    if (!fp) {
        perror("係数ファイル書き込み失敗");
        exit(1);
    }

    for (int n = 0; n <= N; ++n) {
        xr[n] = calc_h(n, N);
        fprintf(fp, "%d %.6f\n", n, xr[n]);
    }
    fclose(fp);
    printf("係数ファイルを保存: %s\n", impulse_file);

    // DFTによるスペクトル解析
    DFT(DFT_SIZE, xr, xi);

    // 振幅スペクトルの保存
    save_spectrum(spectrum_file, xr, xi, DFT_SIZE);
    printf("スペクトルファイルを保存: %s\n", spectrum_file);
}

int main() {
    int N_list[] = {100, 500, 1000};
    int num = sizeof(N_list) / sizeof(N_list[0]);

    for (int i = 0; i < num; ++i) {
        process_filter(N_list[i]);
    }

    return 0;
}