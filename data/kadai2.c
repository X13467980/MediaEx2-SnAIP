#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>

#define SAMPLE_RATE 16000                                          // サンプリング周波数を16kHzに設定
#define SEGMENT_DURATION_MS 20                                     // セグメントの長さを20msに設定
#define SEGMENT_SAMPLES (SAMPLE_RATE * SEGMENT_DURATION_MS / 1000) // セグメントのサンプル数を計算=320サンプル

void cut_center_segment(const char *in_filename, const char *out_txt_filename) // 関数の定義
{
    struct stat st;                  // ファイルの情報を取得するための構造体
    if (stat(in_filename, &st) != 0) // ファイルの情報を取得
    {
        perror("stat");
        return;
    }

    long file_size = st.st_size;                     // ファイルサイズを取得
    int total_samples = file_size / sizeof(int16_t); // ファイルサイズを16ビット整数のサイズで割ってサンプル数を計算

    int start_sample;           // セグメントの開始サンプルを格納する変数
    if (total_samples % 2 == 0) // サンプル数が偶数の場合
    {
        start_sample = total_samples / 2 - SEGMENT_SAMPLES / 2; // セグメントの開始サンプルを計算
    }
    else // サンプル数が奇数の場合
    {
        start_sample = (total_samples + 1) / 2 - SEGMENT_SAMPLES / 2; // セグメントの開始サンプルを計算
    }

    long start_byte = start_sample * sizeof(int16_t); // 開始サンプルをバイトに変換

    FILE *in_fp = fopen(in_filename, "rb"); // 入力ファイルをバイナリモードで開く
    if (!in_fp)
    {
        perror("fopen input");
        return;
    }

    if (fseek(in_fp, start_byte, SEEK_SET) != 0) // ファイルポインタをセグメントの開始位置に移動
    {
        perror("fseek");
        fclose(in_fp);
        return;
    }

    int16_t buffer[SEGMENT_SAMPLES];                                              // セグメントのサンプルを格納するバッファを定義
    size_t read_samples = fread(buffer, sizeof(int16_t), SEGMENT_SAMPLES, in_fp); // セグメントのサンプルを読み込む
    fclose(in_fp);                                                                // 入力ファイルを閉じる

    if (read_samples != SEGMENT_SAMPLES) // 読み込んだサンプル数が期待値と異なる場合
    {
        fprintf(stderr, "error: expected %d samples, read %zu\n", SEGMENT_SAMPLES, read_samples);
    }

    FILE *out_fp = fopen(out_txt_filename, "w"); // 出力ファイルをテキストモードで開く
    if (!out_fp)
    {
        perror("fopen output");
        return;
    }

    double start_time_ms = (double)start_sample * 1000.0 / SAMPLE_RATE; // セグメントの開始時間をミリ秒で計算
    for (size_t i = 0; i < read_samples; ++i) // 読み込んだサンプル数分ループ
    {
        double time_ms = start_time_ms + i * 1000.0 / SAMPLE_RATE; // 各サンプルの時間をミリ秒で計算
        fprintf(out_fp, "%.3f %d\n", time_ms, buffer[i]); // 時間とサンプル値を出力ファイルに書き込む
    }

    fclose(out_fp); // 出力ファイルを閉じる
    printf("Saved %s (%zu samples)\n", out_txt_filename, read_samples); // 保存したファイル名とサンプル数を表示
}

int main()
{
    const char *in_files[] = {"a00.raw", "i00.raw", "u00.raw", "e00.raw", "o00.raw"}; // 入力ファイル名
    const char *out_txts[] = {"a00_cut.txt", "i00_cut.txt", "u00_cut.txt", "e00_cut.txt", "o00_cut.txt"}; // 出力ファイル名

    for (int i = 0; i < 5; ++i) // 5つの入力ファイルに対してループ
    {
        cut_center_segment(in_files[i], out_txts[i]);
    }

    return 0;
}