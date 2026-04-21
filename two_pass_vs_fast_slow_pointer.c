#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>

struct node {
    struct node *next;
    uint64_t value;
};

/* 防止編譯器把結果優化掉 */
volatile uint64_t sink = 0;

/* Fisher-Yates shuffle */
static void shuffle_nodes(struct node **arr, size_t n)
{
    if (n <= 1) return;

    for (size_t i = n - 1; i > 0; i--) {
        size_t j = (size_t)(rand() % (i + 1));
        struct node *tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

/* 建立一條「記憶體位置被打亂」的 linked list */
static struct node *build_random_list(size_t n)
{
    struct node **arr = malloc(n * sizeof(*arr));
    if (!arr) {
        perror("malloc arr failed");
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        arr[i] = malloc(sizeof(struct node));
        if (!arr[i]) {
            perror("malloc node failed");
            for (size_t k = 0; k < i; k++) {
                free(arr[k]);
            }
            free(arr);
            return NULL;
        }
        arr[i]->next = NULL;
        arr[i]->value = i;
    }

    shuffle_nodes(arr, n);

    for (size_t i = 0; i + 1 < n; i++) {
        arr[i]->next = arr[i + 1];
    }
    arr[n - 1]->next = NULL;

    struct node *head = arr[0];
    free(arr);
    return head;
}

/* 釋放 list */
static void free_list(struct node *head)
{
    while (head) {
        struct node *next = head->next;
        free(head);
        head = next;
    }
}

/* 方法一：快慢指標 */
static struct node *middle_fast_slow(struct node *head)
{
    struct node *slow = head;
    struct node *fast = head;

    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

/* 算長度 */
static size_t list_length(struct node *head)
{
    size_t len = 0;
    for (struct node *p = head; p != NULL; p = p->next) {
        len++;
    }
    return len;
}

/* 方法二：two-pass，先算長度，再走到中間 */
static struct node *middle_two_pass(struct node *head)
{
    size_t len = list_length(head);
    size_t mid = len / 2;

    struct node *p = head;
    for (size_t i = 0; i < mid; i++) {
        p = p->next;
    }
    return p;
}

/* benchmark：重複執行，降低 perf 雜訊 */
static void run_benchmark(struct node *head, int mode, size_t repeat)
{
    struct node *mid = NULL;

    for (size_t i = 0; i < repeat; i++) {
        if (mode == 0) {
            mid = middle_fast_slow(head);
        } else if (mode == 1) {
            mid = middle_two_pass(head);
        } else {
            fprintf(stderr, "Unknown mode: %d\n", mode);
            exit(1);
        }
    }

    if (mid != NULL) {
        sink = mid->value;
    }
}

static unsigned long long parse_ull(const char *s, const char *name)
{
    errno = 0;
    char *end = NULL;
    unsigned long long x = strtoull(s, &end, 10);

    if (errno != 0 || end == s || *end != '\0') {
        fprintf(stderr, "Invalid %s: %s\n", name, s);
        exit(1);
    }
    return x;
}

static int parse_int(const char *s, const char *name)
{
    errno = 0;
    char *end = NULL;
    long x = strtol(s, &end, 10);

    if (errno != 0 || end == s || *end != '\0') {
        fprintf(stderr, "Invalid %s: %s\n", name, s);
        exit(1);
    }
    return (int)x;
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <list_length> <mode> <repeat>\n", argv[0]);
        fprintf(stderr, "  mode = 0 : fast/slow pointer\n");
        fprintf(stderr, "  mode = 1 : two-pass\n");
        return 1;
    }

    size_t n = (size_t)parse_ull(argv[1], "list_length");
    int mode = parse_int(argv[2], "mode");
    size_t repeat = (size_t)parse_ull(argv[3], "repeat");

    srand(0);  /* 固定 seed，讓每次實驗一致 */

    struct node *head = build_random_list(n);
    if (!head) {
        fprintf(stderr, "Failed to build list.\n");
        return 1;
    }

    run_benchmark(head, mode, repeat);

    /* 輸出 sink，避免被最佳化 */
    printf("sink = %llu\n", (unsigned long long)sink);

    free_list(head);
    return 0;
}
