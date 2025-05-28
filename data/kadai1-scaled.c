#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    FILE *fp_in, *fp_out;
    struct stat st;
    short sample, scaled_sample;
    int num_samples;
    double gain = 30.0;

    // 引数チェック
    if (argc != 3) {
        printf("使い方: %s <入力ファイル> <出力ファイル>\n", argv[0]);
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
    fp_out = fopen(output_filename, "wb");
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

    num_samples = st.st_size / sizeof(short);

    for (int i = 0; i < num_samples; i++) {
        if (fread(&sample, sizeof(short), 1, fp_in) != 1) break;

        int temp = (int)(sample * gain);
        // if (temp > 32767) temp = 32767;
        // if (temp < -32768) temp = -32768;
        scaled_sample = (short)temp;

        fwrite(&scaled_sample, sizeof(short), 1, fp_out);
    }

    fclose(fp_in);
    fclose(fp_out);

    printf("振幅を %.1f 倍した音声を %s に出力しました。\n", gain, output_filename);
    return 0;
}

// a00 8000
// a000-loud3 15000
// a000-loud30 400000