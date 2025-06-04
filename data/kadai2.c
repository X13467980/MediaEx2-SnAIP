#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>

#define SAMPLE_RATE 16000
#define SEGMENT_DURATION_MS 20
#define SEGMENT_SAMPLES (SAMPLE_RATE * SEGMENT_DURATION_MS / 1000)

void cut_center_segment(const char* in_filename, const char* out_txt_filename) {
    struct stat st;
    if (stat(in_filename, &st) != 0) {
        perror("stat");
        return;
    }

    long file_size = st.st_size;
    int total_samples = file_size / sizeof(int16_t);

    int start_sample;
    if (total_samples % 2 == 0) {
        start_sample = total_samples / 2 - SEGMENT_SAMPLES / 2;
    } else {
        start_sample = (total_samples + 1) / 2 - SEGMENT_SAMPLES / 2;
    }

    long start_byte = start_sample * sizeof(int16_t);

    FILE* in_fp = fopen(in_filename, "rb");
    if (!in_fp) {
        perror("fopen input");
        return;
    }

    if (fseek(in_fp, start_byte, SEEK_SET) != 0) {
        perror("fseek");
        fclose(in_fp);
        return;
    }

    int16_t buffer[SEGMENT_SAMPLES];
    size_t read_samples = fread(buffer, sizeof(int16_t), SEGMENT_SAMPLES, in_fp);
    fclose(in_fp);

    if (read_samples != SEGMENT_SAMPLES) {
        fprintf(stderr, "Warning: expected %d samples, read %zu\n", SEGMENT_SAMPLES, read_samples);
    }

    FILE* out_fp = fopen(out_txt_filename, "w");
    if (!out_fp) {
        perror("fopen output");
        return;
    }

    double start_time_ms = (double)start_sample * 1000.0 / SAMPLE_RATE;
    for (size_t i = 0; i < read_samples; ++i) {
        double time_ms = start_time_ms + i * 1000.0 / SAMPLE_RATE;
        fprintf(out_fp, "%.3f %d\n", time_ms, buffer[i]);
    }

    fclose(out_fp);
    printf("Saved %s (%zu samples)\n", out_txt_filename, read_samples);
}

int main() {
    const char* in_files[]  = {"a00.raw", "i00.raw", "u00.raw", "e00.raw", "o00.raw"};
    const char* out_txts[]  = {"a00_cut.txt", "i00_cut.txt", "u00_cut.txt", "e00_cut.txt", "o00_cut.txt"};

    for (int i = 0; i < 5; ++i) {
        cut_center_segment(in_files[i], out_txts[i]);
    }

    return 0;
}