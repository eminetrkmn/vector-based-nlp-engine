#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *question_embedding;
    char *answer_embedding;
    char *knowledge_base;
} QAPair;

int max_emb_size;
int vector_size;
int qa_count;
QAPair *qa_pairs;

char* perform_xor(char *emb1, char *emb2, int length) {
    char *result = (char*)malloc((length + 1) * sizeof(char));
    int i;
    for (i = 0; i < length; i++) {
        result[i] = (emb1[i] == emb2[i]) ? '0' : '1';
    }
    result[length] = '\0';
    return result;
}

float calculate_matching_score(char *emb1, char *emb2, int length) {
    char *xor_result = perform_xor(emb1, emb2, length);
    int count = 0, i;
    for (i = 0; i < length; i++) {
        if (xor_result[i] == '1') count++;
    }
    free(xor_result);
    return 1.0 - ((float)count / length);
}

void read_metadata(FILE *file) {
    char line[5000];
    fgets(line, sizeof(line), file); // Metadata Part
    fgets(line, sizeof(line), file);
    sscanf(line, "The maximum length of a sentence embedding across all questions and answers:%d", &max_emb_size);
    fgets(line, sizeof(line), file);
    sscanf(line, "The dimension of the word embeddings:%d", &vector_size);
    fgets(line, sizeof(line), file);
    sscanf(line, "The number of question-answer pairs:%d", &qa_count);
    fgets(line, sizeof(line), file); // #####
    qa_pairs = (QAPair*)malloc(qa_count * sizeof(QAPair));
}

void read_embeddings_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Embeddings.txt dosyasi acilamadi!\n");
        exit(1);
    }
    read_metadata(file);
    int total_len = max_emb_size * vector_size;
    char line[10000];
    int i;
    for (i = 0; i < qa_count; i++) {
        fgets(line, sizeof(line), file);
        qa_pairs[i].question_embedding = (char*)malloc((total_len + 1) * sizeof(char));
        strncpy(qa_pairs[i].question_embedding, line + 10, total_len);
        qa_pairs[i].question_embedding[total_len] = '\0';

        fgets(line, sizeof(line), file);
        qa_pairs[i].answer_embedding = (char*)malloc((total_len + 1) * sizeof(char));
        strncpy(qa_pairs[i].answer_embedding, line + 8, total_len);
        qa_pairs[i].answer_embedding[total_len] = '\0';

        fgets(line, sizeof(line), file); // ---
    }
    fclose(file);
}

void create_knowledge_base() {
    int i;
    int total_len = max_emb_size * vector_size;
    for (i = 0; i < qa_count; i++) {
        qa_pairs[i].knowledge_base = perform_xor(qa_pairs[i].question_embedding,
                                                  qa_pairs[i].answer_embedding, total_len);
    }
}

void write_vector(FILE *f, char *str, int max_len, int vector_size) {
    char temp[1000], temp1[1000];
    int i, j;
    int len = strlen(str);
    for (i = 0; i < max_len - len; i++) {
        for (j = 0; j < vector_size; j++) fprintf(f, "0");
    }
    for (i = 0; i < len; i++) {
        FILE *p = fopen("letters.txt", "r");
        if (p == NULL) {
            printf("letters.txt dosyasi acilamadi!\n");
            return;
        }
        int found = 0;
        while (fgets(temp, sizeof(temp), p) != NULL) {
            temp[strcspn(temp, "\n")] = '\0';
            if (temp[0] == str[i] && temp[1] == ':') {
                strcpy(temp1, temp + 2);
                fprintf(f, "%s", temp1);
                found = 1;
                break;
            }
        }
        fclose(p);
        if (!found) {
            for (j = 0; j < vector_size; j++) fprintf(f, "0");
        }
    }
}

char* create_query_embedding(char *query_text) {
    FILE *temp_file = fopen("temp_query.txt", "w");
    if (temp_file == NULL) {
        printf("Gecici dosya yazilamadi!\n");
        exit(1);
    }
    write_vector(temp_file, query_text, max_emb_size, vector_size);
    fclose(temp_file);

    temp_file = fopen("temp_query.txt", "r");
    if (temp_file == NULL) {
        printf("Gecici dosya okunamadi!\n");
        exit(1);
    }

    int total_len = max_emb_size * vector_size;
    char *query_embedding = (char*)malloc((total_len + 1) * sizeof(char));
    fread(query_embedding, 1, total_len, temp_file);
    query_embedding[total_len] = '\0';
    fclose(temp_file);
    remove("temp_query.txt");
    return query_embedding;
}

void answer_query(char *query_embedding, float threshold) {
    int i, total_len = max_emb_size * vector_size, found = 0;
    printf("\nSorgulama Sonuclari (Eslesme > %.2f%%):\n", threshold * 100);
    for (i = 0; i < qa_count; i++) {
        char *calculated_answer = perform_xor(qa_pairs[i].knowledge_base, query_embedding, total_len);
        float score = calculate_matching_score(calculated_answer, qa_pairs[i].answer_embedding, total_len);
        if (score > threshold) {
            printf("\nEslesme #%d (Skor: %.2f%%)\n", found + 1, score * 100);
            printf("Hesaplanan cevap embedding'i: %.*s...\n", 50, calculated_answer);
            printf("Veritabanindaki cevap embedding'i: %.*s...\n", 50, qa_pairs[i].answer_embedding);
            found++;
        }
        free(calculated_answer);
    }
    if (found == 0) {
        printf("Sorguya uygun cevap bulunamadi.\n");
    }
}

void cleanup() {
    int i;
    for (i = 0; i < qa_count; i++) {
        free(qa_pairs[i].question_embedding);
        free(qa_pairs[i].answer_embedding);
        free(qa_pairs[i].knowledge_base);
    }
    free(qa_pairs);
}

int main() {
    printf("CSE102 - Odev 10 Soru-Cevap Sistemi\n");
    printf("====================================\n");

    read_embeddings_file("embeddings.txt");
    printf("Metadata:\n- Max embedding: %d\n- Vektör boyutu: %d\n- QA sayısı: %d\n",
           max_emb_size, vector_size, qa_count);

    create_knowledge_base();
    printf("Bilgi tabani olusturuldu.\n");

    int choice = 1;
    char query[1000];
    float threshold = 0.80;

    while (choice != 0) {
        printf("\nMenu:\n1. Soru sor\n2. Eslesme esigini degistir (su anki: %.2f%%)\n0. Cikis\nSeciminiz: ",
               threshold * 100);
        scanf("%d", &choice);
        getchar(); // newline temizle

        switch (choice) {
            case 1:
                printf("Sorunuzu girin: ");
                fgets(query, sizeof(query), stdin);
                query[strcspn(query, "\n")] = '\0';

                {
                    char *query_embedding = create_query_embedding(query);
                    printf("Sorgu embedding'i (ilk 50 bit): %.*s...\n", 50, query_embedding);
                    answer_query(query_embedding, threshold);
                    free(query_embedding);
                }
                break;

            case 2:
                printf("Yeni esik (0.0 - 1.0): ");
                scanf("%f", &threshold);
                if (threshold < 0.0 || threshold > 1.0) {
                    printf("Gecersiz! Varsayilan (0.80) kullanilacak.\n");
                    threshold = 0.80;
                }
                break;

            case 0:
                printf("Programdan cikiliyor...\n");
                break;

            default:
                printf("Gecersiz secim!\n");
                break;
        }
    }

    cleanup();
    return 0;
}
