#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "DFT_IDFT_kadai3.h"


#define SAMPLE_RATE 16000 // サンプリング周波数を16kHzに設定
#define N 1024            // データの長さを1024サンプルに設定
#define K 10              // 周波数成分のインデックスを10に設定

void write_txt(const char *filename, double *data)
{
    FILE *fp = fopen(filename, "w");
    if (!fp)
    {
        perror("fopen");
        exit(1);
    }

    for (int i = 0; i < N; ++i)
    {
        double freq_khz = (double)i * SAMPLE_RATE / N / 1000.0; // 周波数[kHz]
        fprintf(fp, "%.6f\t%.6f\n", freq_khz, data[i]); // 2列出力: 周波数[kHz], 値
    }

    fclose(fp);
}

void write_txt_time(const char *filename, double *data)
{
    FILE *fp = fopen(filename, "w");
    if (!fp)
    {
        perror("fopen");
        exit(1);
    }

    for (int i = 0; i < N; ++i)
    {
        fprintf(fp, "%d\t%.6f\n", i, data[i]); // 時間インデックスと値
    }

    fclose(fp);
}

void write_raw(const char *filename, double *data) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        exit(1);
    }

    for (int i = 0; i < N; ++i) {
        int16_t sample = (int16_t)(data[i] * 32767.0); // -1〜1を16bit整数にスケーリング
        fwrite(&sample, sizeof(int16_t), 1, fp);
    }

    fclose(fp);
}

int main()
{
    double sin_wave[N] = {0}, cos_wave[N] = {0}; // 正弦波と余弦波の配列を初期化
    double sin_i[N] = {0}, cos_i[N] = {0};       // DFTの虚部を格納する配列を初期化

    for (int n = 0; n < N; ++n) // 各時間サンプル
    {
        sin_wave[n] = sin(2 * M_PI * K * n / N); // 正弦波の計算
        cos_wave[n] = cos(2 * M_PI * K * n / N); // 余弦波の計算
    }

    double sin_r[N], cos_r[N];                 // DFTの実部を格納する配列を初期化
    memcpy(sin_r, sin_wave, sizeof(sin_wave)); // 正弦波の実部をコピー
    memcpy(cos_r, cos_wave, sizeof(cos_wave)); // 余弦波の実部をコピー

    DFT(N, sin_r, sin_i); // 正弦波のDFTを計算
    DFT(N, cos_r, cos_i); // 余弦波のDFTを計算

    write_txt_time("sin.txt", sin_wave); // 元の正弦波の波形を保存
    write_txt_time("cos.txt", cos_wave); // 元の余弦波の波形を保存
    write_raw("sin.raw", sin_wave); // 正弦波をRAW形式で保存
    write_raw("cos.raw", cos_wave); // 余弦波をRAW形式で保存
    write_txt("sin_real.txt", sin_r); // 正弦波の実部をファイルに書き込む
    write_txt("sin_imag.txt", sin_i); // 正弦波の虚部をファイルに書き込む
    write_txt("cos_real.txt", cos_r); // 余弦波の実部をファイルに書き込む
    write_txt("cos_imag.txt", cos_i); // 余弦波の虚部をファイルに書き込む

    IDFT(N, sin_r, sin_i); // 正弦波の逆DFTを計算
    IDFT(N, cos_r, cos_i); // 余弦波の逆DFTを計算

    write_txt_time("sin_idft.txt", sin_r); // 正弦波の逆DFTの結果をファイルに書き込む
    write_txt_time("cos_idft.txt", cos_r); // 余弦波の逆DFTの結果をファイルに書き込む

    write_txt_time("sin_idft.txt", sin_r); // 正弦波の逆DFTの結果をファイルに書き込む
    write_txt_time("cos_idft.txt", cos_r); // 余弦波の逆DFTの結果をファイルに書き込む

    double sin_power[N], cos_power[N];                 // パワーを格納する配列を初期化
    double sin_time_power = 0.0, cos_time_power = 0.0; // 時間平均パワーを格納する変数を初期化
    double sin_spec_power = 0.0, cos_spec_power = 0.0; // スペクトル平均パワーを格納する変数を初期化

    for (int k = 0; k < N; ++k) // 各周波数成分
    {
        sin_power[k] = sin_r[k] * sin_r[k] + sin_i[k] * sin_i[k]; // パワーの計算
        cos_power[k] = cos_r[k] * cos_r[k] + cos_i[k] * cos_i[k]; // パワーの計算
        sin_spec_power += sin_power[k];                           // スペクトル平均パワーの計算
        cos_spec_power += cos_power[k];                           // スペクトル平均パワーの計算
    }

    for (int n = 0; n < N; ++n) // 各時間サンプル
    {
        sin_time_power += sin_wave[n] * sin_wave[n]; // 時間平均パワーの計算
        cos_time_power += cos_wave[n] * cos_wave[n]; // 時間平均パワーの計算
    }

    sin_spec_power /= N; // スペクトル平均パワーを正規化
    cos_spec_power /= N; // スペクトル平均パワーを正規化
    sin_time_power /= N; // 時間平均パワーを正規化
    cos_time_power /= N; // 時間平均パワーを正規化

    write_txt("sin_power.txt", sin_power); // 正弦波のパワーをファイルに書き込む
    write_txt("cos_power.txt", cos_power); // 余弦波のパワーをファイルに書き込む
    write_txt("sin_spec_power.txt", &sin_spec_power); // 正弦波のスペクトル平均パワーをファイルに書き込む
    write_txt("cos_spec_power.txt", &cos_spec_power); // 余弦波のスペクトル平均パワーをファイルに書き込む

    // printf("正弦波：時間平均パワー = %.6f, スペクトル平均パワー = %.6f\n", sin_time_power, sin_spec_power);
    // printf("余弦波：時間平均パワー = %.6f, スペクトル平均パワー = %.6f\n", cos_time_power, cos_spec_power);

    printf("Done\n");
    return 0;
}

//横軸周波数・縦軸振幅の振幅スペクトルを計算