#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SAMPLE_RATE 16000 // サンプリング周波数を16kHzに設定
#define N 1024            // データの長さを1024サンプルに設定
#define K 10              // 周波数成分のインデックスを10に設定

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

void write_txt(const char *filename, double *data) // テキストファイルにデータを書き込む関数
{
    FILE *fp = fopen(filename, "w"); // ファイルを開く
    if (!fp)                         // ファイルオープンに失敗した場合
    {
        perror("fopen"); // エラーメッセージを表示
        exit(1);
    }
    for (int i = 0; i < N; ++i) // データの数だけループ
    {
        fprintf(fp, "%.6f\n", data[i]); // データを書き込む
    }
    fclose(fp); // ファイルを閉じる
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

    write_txt("sin.txt", sin_wave); // 元の正弦波の波形を保存
    write_txt("cos.txt", cos_wave); // 元の余弦波の波形を保存
    write_txt("sin_real.txt", sin_r); // 正弦波の実部をファイルに書き込む
    write_txt("sin_imag.txt", sin_i); // 正弦波の虚部をファイルに書き込む
    write_txt("cos_real.txt", cos_r); // 余弦波の実部をファイルに書き込む
    write_txt("cos_imag.txt", cos_i); // 余弦波の虚部をファイルに書き込む

    IDFT(N, sin_r, sin_i); // 正弦波の逆DFTを計算
    IDFT(N, cos_r, cos_i); // 余弦波の逆DFTを計算

    write_txt("sin_idft.txt", sin_r); // 正弦波の逆DFTの結果をファイルに書き込む
    write_txt("cos_idft.txt", cos_r); // 余弦波の逆DFTの結果をファイルに書き込む

    write_txt("sin_idft.txt", sin_r); // 正弦波の逆DFTの結果をファイルに書き込む
    write_txt("cos_idft.txt", cos_r); // 余弦波の逆DFTの結果をファイルに書き込む

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