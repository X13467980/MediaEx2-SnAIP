#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "DFT_IDFT_kadai3.h" // 課題3で作成したDFT関数をインクルード

#define SAMPLE_RATE 16000                            // サンプリング周波数 [Hz]
#define FRAME_MS 20                                  // 切り出す中央フレームの長さ [ms]
#define FRAME_LENGTH (SAMPLE_RATE * FRAME_MS / 1000) // フレームの標本数 = 320
#define N 1024                                       // DFT点数

// ハミング窓関数を適用する
void apply_hamming_window(double *x, int L)
{
    for (int n = 0; n < L; ++n)
    {
        // ハミング窓の定義式：w[n] = 0.54 - 0.46 * cos(2πn / (L-1))
        double w = 0.54 - 0.46 * cos(2.0 * M_PI * n / (L - 1));
        x[n] *= w;
    }
}

// 対数パワースペクトルを計算する関数（dB単位）
void compute_log_power_spectrum(double *xr, double *xi, double *log_power)
{
    for (int k = 0; k < N; ++k)
    {
        double power = xr[k] * xr[k] + xi[k] * xi[k]; // パワースペクトル = |X[k]|^2
        log_power[k] = 10.0 * log10(power + 1e-12);   // dB変換（log(0)回避のため微小値を加算）
    }
}

// 音声ファイルから中央20msのデータを読み込む関数
int load_center_frame(const char *filename, double *frame)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("ファイルオープン失敗");
        return -1;
    }

    // 16bitの整数データを一時的に読み込むためのバッファ
    short *buffer = (short *)malloc(sizeof(short) * N);
    memset(buffer, 0, sizeof(short) * N);

    // ファイルサイズからサンプル数を計算
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    int total_samples = filesize / sizeof(short);

    // 中央FRAME_LENGTHサンプルの開始位置を計算
    int start = total_samples / 2 - FRAME_LENGTH / 2;
    fseek(fp, start * sizeof(short), SEEK_SET);
    fread(buffer, sizeof(short), FRAME_LENGTH, fp);

    // 読み込んだデータをdoubleに変換してframe[]に格納
    for (int i = 0; i < FRAME_LENGTH; ++i)
    {
        frame[i] = (double)buffer[i];
    }

    // 残りはゼロパディング（長さNまで）
    for (int i = FRAME_LENGTH; i < N; ++i)
    {
        frame[i] = 0.0;
    }

    free(buffer);
    fclose(fp);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "使い方: %s <入力ファイル名.raw> <出力ファイル名.txt>\n", argv[0]);
        return 1;
    }

    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    double xr[N] = {0};  // DFT入力の実部
    double xi[N] = {0};  // DFT入力の虚部（初期値は0）
    double log_power[N]; // 対数パワースペクトル

    // 音声データを中央20ms分読み込み
    if (load_center_frame(input_filename, xr) != 0)
    {
        fprintf(stderr, "読み込みエラー: %s\n", input_filename);
        return 1;
    }

    // ハミング窓を適用
    apply_hamming_window(xr, FRAME_LENGTH);

    // 離散フーリエ変換を実行
    DFT(N, xr, xi);

    // 対数パワースペクトルを計算
    compute_log_power_spectrum(xr, xi, log_power);

    // 出力ファイルを開く
    FILE *out = fopen(output_filename, "w");
    if (!out)
    {
        perror("出力ファイルオープン失敗");
        return 1;
    }

    // 出力：1列目=周波数[kHz], 2列目=パワー[dB]
    for (int k = 0; k < N; ++k)
    {
        double freq = (double)k * SAMPLE_RATE / N / 1000.0; // kHz単位
        fprintf(out, "%f %f\n", freq, log_power[k]);
    }

    fclose(out);
    printf("%s のスペクトルを %s に出力しました。\n", input_filename, output_filename);

    return 0;
}