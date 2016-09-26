#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include IMPL

#define DICT_FILE "./dictionary/words.txt"

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int i = 0;
    char line[MAX_LAST_NAME_SIZE];
    struct timespec start, end;
    double cpu_time1, cpu_time2;

    /* check file opening */
    fp = fopen(DICT_FILE, "r");
    if (fp == NULL) {
        printf("cannot open the file\n");
        return -1;
    }

    /* build the entry */
#if defined(HASH)
    entry pHead[HASH_TABLE_SIZE], *e[HASH_TABLE_SIZE];
    printf("size of entry : %lu bytes\n", sizeof(entry));
    int index = 0;
    do {
        e[index] = &pHead[index];
        e[index]->pNext = NULL;
        index++;
    } while(index < HASH_TABLE_SIZE);
#else
    entry *pHead, *e;
    pHead = (entry *) malloc(sizeof(entry));
    printf("size of entry : %lu bytes\n", sizeof(entry));
    e = pHead;
    e->pNext = NULL;
#endif

#if defined(HASH)
    __builtin___clear_cache((char *) pHead, (char *) pHead + HASH_TABLE_SIZE * sizeof(entry));
#else
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif

    clock_gettime(CLOCK_REALTIME, &start);
    while (fgets(line, sizeof(line), fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
#if defined(HASH)
        unsigned int key = hash(line) % HASH_TABLE_SIZE;
        e[key] = append(line, e[key]);
#else
        e = append(line, e);
#endif
    }
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);

    /* close file as soon as possible */
    fclose(fp);

    // Make e pointer return to head
#if defined(HASH)
    index = 0;
    do {
        e[index] = &pHead[index];
    } while (index < HASH_TABLE_SIZE);
#else
    e = pHead;
#endif

    /* the givn last name to find */
    char input[MAX_LAST_NAME_SIZE] = "zyxel";
#if defined(HASH)
    unsigned int key = hash(line) % HASH_TABLE_SIZE;
    e[key] = &pHead[key];
    assert(0 == strcmp(findName(input, e)->lastName, "zyxel"));    
#else
    e = pHead;
    assert(findName(input, e) &&
           "Did you implement findName() in " IMPL "?");
    assert(0 == strcmp(findName(input, e)->lastName, "zyxel"));
#endif

#if defined(__GNUC__)
#if defined(HASH)
    __builtin___clear_cache((char *) pHead, (char *) pHead + HASH_TABLE_SIZE * sizeof(entry));
#else
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
#endif

    /* compute the execution time */
    clock_gettime(CLOCK_REALTIME, &start);
#if defined(HASH)
    key = hash(line) % HASH_TABLE_SIZE;
    e[key] = &pHead[key];
    findName(input, e[key]);
#else
    findName(input, e);
#endif
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time2 = diff_in_second(start, end);

    FILE *output;
#if defined(OPT)
    output = fopen("opt.txt", "a");
#else
    output = fopen("orig.txt", "a");
#endif
    fprintf(output, "append() findName() %lf %lf\n", cpu_time1, cpu_time2);
    fclose(output);

    printf("execution time of append() : %lf sec\n", cpu_time1);
    printf("execution time of findName() : %lf sec\n", cpu_time2);

    if (pHead->pNext) free(pHead->pNext);
    free(pHead);

    return 0;
}
