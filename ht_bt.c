#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>

typedef struct file_data {
    // public data
    long word_pos; // zero-based
    long word_num; // zero-based
    char word[64];
    // private data
    FILE* fp;
    long current_pos; // zero-based
} file_data_t;

//Representa cada palavra distinta num determinado ficheiro
typedef struct word {
    struct word* left;
    struct word* right;
    char word[64];
    int hash;
    int first_location;
    int last_location;
    int max_dist;
    int min_dist;
    int medium_dist;
    int count;
} word_t;

typedef struct hash_table {
    unsigned int size;
    unsigned int count;
    word_t** table;
} hash_table_t;

int open_text_file(char* file_name, file_data_t* fd)
{
    fd->fp = fopen(file_name, "r");
    if (fd->fp == NULL){
        printf("File does not exist.\n");
        return -1;
    }
    fd->word_pos = -1;
    fd->word_num = -1;
    ;
    fd->word[0] = '\0';
    fd->current_pos = -1;
    return 0;
}

void close_text_file(file_data_t* fd)
{
    fclose(fd->fp);
    fd->fp = NULL;
}

int read_word(file_data_t* fd)
{
    int i, c;
    // skip white spaces
    do {
        c = fgetc(fd->fp);
        if (c == EOF)
            return -1;
        fd->current_pos++;

    } while (c <= 32);
    //record word
    fd->word_pos = fd->current_pos;
    fd->word_num++;
    fd->word[0] = (char)c;
    for (i = 1; i < (int)sizeof(fd->word) - 1; i++) {
        c = fgetc(fd->fp);
        if (c == EOF)
            break;
        // end of file
        fd->current_pos++;
        if (c <= 32)
            break;
        // terminate word
        fd->word[i] = (char)c;
    }
    fd->word[i] = '\0';
    return 0;
}

unsigned long
hash(unsigned char* str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return abs(hash);
}
void print2DUtil(word_t* root, int space)
{
    // Base case
    if (root == NULL) {
        printf("\n");
        for (int i = 10; i < space + 10; i++)
            printf(" ");
        printf("%15s\n", NULL);
        return;
    }
    // Increase distance between levels
    space += 10;

    // Process right child first
    print2DUtil(root->right, space);

    // Print current node after space
    // count
    printf("\n");
    for (int i = 10; i < space; i++)
        printf(" ");
    printf("%15s\n", root->word);

    // Process left child
    print2DUtil(root->left, space);
}

void visit(word_t* word)
{
    printf("%s (FL: %d, LL: %d, MAXD: %d, MIND: %d, MEDD: %d, WC: %d)\n", word->word, word->first_location, word->last_location, word->max_dist, word->min_dist, word->medium_dist, word->count);
}

word_t* search_recursive(word_t* link, char* data)
{
    if (link == NULL || strcmp(link->word, data) == 0)
        return link;
    if (strcmp(link->word, data) > 0)
        return search_recursive(link->left, data);
    else
        return search_recursive(link->right, data);
}

void insert_non_recursive(word_t** link, word_t** insert, char* data)
{
    word_t* parent = NULL;
    while (*link != NULL) {
        parent = *link;
        link = (strcmp((*link)->word, data) > 0) ? &((*link)->left) : &((*link)->right);
    }
    *link = *insert;
}

void traverse_in_order_recursive(word_t* link)
{
    if (link != NULL) {
        traverse_in_order_recursive(link->left);
        //printf("---------------------------\n");
        visit(link);
        //printf("---------------------------\n");
        traverse_in_order_recursive(link->right);
    }
}
void most_used_words(hash_table_t* hash_table)
{
    word_t* head_test;
    word_t* checker;
    head_test = (word_t*)malloc(sizeof(word_t));
    checker = (word_t*)malloc(sizeof(word_t));
    int max_prev = __INT_MAX__;
    int max_in_cicle = 0;
    int occ;
    char palavra[64] = "word";
    for (int c = 0; c < 10; c++) {
        max_in_cicle = 0;
        for (int f = 0; f < hash_table->size; f++) {

            checker = hash_table->table[f];
            if (checker->count < max_prev) {
                if (checker->count > max_in_cicle) {
                    strcpy(palavra, checker->word);
                    max_in_cicle = checker->count;
                    occ = checker->count;
                }
            }
        }
        printf("%-5s (%d)\n", palavra, occ);
        max_prev = max_in_cicle;
    }
}

int main(int argc, char* argv[])
{
    file_data_t* fl;
    fl = (file_data_t*)malloc(sizeof(file_data_t));

    if (open_text_file(argv[1], fl) == -1) {
        return EXIT_FAILURE;
    }

    double time_spent_total = 0;

    clock_t begin_total = clock();

    // HASHTABLE INITIALIZATION
    hash_table_t* hash_table = NULL;
    hash_table = malloc(sizeof(hash_table_t));
    hash_table->table = malloc(2000 * sizeof(word_t*));
    hash_table->size = 2000;
    hash_table->count = 0;
    for (int i = 0; i < 2000; i++) {
        hash_table->table[i] = NULL;
    }

    // HEAD DECLARATION TO USE INSIDE WHILE CYCLE
    word_t* head;
    head = (word_t*)malloc(sizeof(word_t));
    int counterrr = 0;
    int hashcode = 0;
    int word_counter = 0;
    while (read_word(fl) != -1) {
        word_counter++;
        hashcode = hash(fl->word) % hash_table->size;
        head = hash_table->table[hashcode];
        if (head == NULL) {
            word_t* new;
            new = (word_t*)malloc(sizeof(word_t));
            new->left = NULL;
            new->right = NULL;
            new->hash = hash(fl->word);
            new->first_location = fl->current_pos;
            new->last_location = fl->current_pos;
            new->max_dist = NULL;
            new->min_dist = NULL;
            new->medium_dist = 0;
            new->count = 1;
            strcpy(new->word, fl->word);
            hash_table->table[hashcode] = new;
            hash_table->count += 1;
            counterrr++;
        }
        else {
            word_t* this_one;
            this_one = search_recursive(head, fl->word);
            if (this_one != NULL) {
                int temp = this_one->last_location;
                int dist = fl->current_pos - temp;
                this_one->last_location = fl->current_pos;
                if (dist > this_one->max_dist || this_one->max_dist == NULL) {
                    this_one->max_dist = dist;
                }
                if (dist < this_one->min_dist || this_one->min_dist == NULL) {
                    this_one->min_dist = dist;
                }
                this_one->medium_dist = this_one->medium_dist + (dist - this_one->medium_dist) / this_one->count;
                this_one->count++;
            }
            else {
                word_t* new1;
                new1 = (word_t*)malloc(sizeof(word_t));
                new1->left = NULL;
                new1->right = NULL;
                new1->hash = hash(fl->word);
                new1->first_location = fl->current_pos;
                new1->last_location = fl->current_pos;
                new1->max_dist = NULL;
                new1->min_dist = NULL;
                new1->medium_dist = 0;
                new1->count = 1;
                strcpy(new1->word, fl->word);
                insert_non_recursive(&head, &new1, new1->word);
                counterrr++;
            }
        }
    }

    clock_t end_total = clock();
    time_spent_total = (double)(end_total - begin_total) / CLOCKS_PER_SEC;

    word_t* word;
    for (int k = 0; k < hash_table->size; k++) {
        printf("==================\n");
        if (hash_table->table[k] == NULL) {
            printf("NULL\n");
        }
        else {
            word = hash_table->table[k];
            traverse_in_order_recursive(word);
            //print2DUtil(word,0);
        }
        printf("==================\n");
        printf("LINE: %d ABOVE\n", k);
    }
    printf("==================\n");
    printf("TABLE STATS\n");
    printf("Words read: %d\n", word_counter);
    printf("Hash elements count: %d\n", hash_table->count);
    printf("Hash elements size: %d\n", hash_table->size);
    printf("Total duration: %5.4fs\n", time_spent_total);
    printf("Number of words inside hash table: %d\n", counterrr);

    file_data_t* ft;
    ft = (file_data_t*)malloc(sizeof(file_data_t));
    word_t* head_test;
    word_t* checker;
    head_test = (word_t*)malloc(sizeof(word_t));
    checker = (word_t*)malloc(sizeof(word_t));
    if (open_text_file("Teste.txt", ft) == -1) {
        return EXIT_FAILURE;
    }
    int count_words = 0;
    int hashcode_test = 0;
    while (read_word(ft) != -1) {
        hashcode_test = 0;
        int flag_test = 0;

        hashcode_test = hash(fl->word) % hash_table->size;
        head_test = hash_table->table[hashcode_test];
        checker = search_recursive(head_test, fl->word);
        if (checker != NULL) {
            flag_test = 1;
            count_words = count_words + checker->count;
        }
        assert(flag_test);
    }
    assert(count_words == word_counter);
    printf("==================\n");
    printf("TOP 10 MOST FREQUENT WORDS\n");
    most_used_words(hash_table);
    return 0;
}