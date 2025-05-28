#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    FILE *fp_in, *fp_out;
    struct stat st;
    short sample;
    int num_samples;
    const double sampling_rate = 16000.0;

    // 引数チェック
    if (argc != 3) {
        fprintf(stderr, "使い方: %s <入力rawファイル> <出力txtファイル>\n", argv[0]);
        return 1;
    }

    // 入出力ファイル名
    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    // 入力ファイルを開く
    fp_in = fopen(input_filename, "rb");
    if (fp_in == NULL) {
        perror("入力ファイルが開けませんでした");
        return 1;
    }

    // 出力ファイルを開く
    fp_out = fopen(output_filename, "w");
    if (fp_out == NULL) {
        perror("出力ファイルが開けませんでした");
        fclose(fp_in);
        return 1;
    }

    // ファイルサイズ取得
    if (stat(input_filename, &st) != 0) {
        perror("ファイルサイズ取得失敗");
        fclose(fp_in);
        fclose(fp_out);
        return 1;
    }

    // 標本数 = バイト数 / 2（16bit = 2バイト）
    num_samples = st.st_size / sizeof(short);

    // 読み取りと出力（ミリ秒単位）
    for (int i = 0; i < num_samples; i++) {
        if (fread(&sample, sizeof(short), 1, fp_in) != 1) {
            break;
        }
        double time_ms = (i * 1000.0) / sampling_rate;
        fprintf(fp_out, "%.3f %d\n", time_ms, sample);
    }

    fclose(fp_in);
    fclose(fp_out);
    printf("出力完了: %s\n", output_filename);

    return 0;
}