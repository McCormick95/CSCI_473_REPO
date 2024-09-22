/* File:     omp_simple.c
 * Purpose:  Implement parallel sample sort of a list of ints. Each thread
 *           iterates through the shared list and pulls elements they are
 *           responsible for into their sublists. The sublists are then sorted
 *           and placed back into position in the original list.
 *
 * Compile:  gcc -g -Wall -lm -fopenmp -o omp_simple omp_simple.c
 * Run:      OMP_NUM_THREADS=<thread_count> ./omp_simple <n> <s> <m>
 *              n = list size
 *              s = sample size
 *              m = if m is not present, get list from stdin.  
 *                  Otherwise generate the list with m as the 
 *                  modulus.
 *
 * Input:    If m is on the command line, none.  Otherwise, the list
 * Output:   The sorted list and the run time for sorting it.
 *
 * Notes:
 * 1.  The number of buckets is the same as thread_count.
 * 2.  We assume that 1 < thread_count < s < n.  We also 
 *     assume that thread_count evenly divides both s and n.
 * 3.  Define DEBUG for output after each stage of the sort.
 *     Also define BIG_DEBUG for details on the sampling and
 *        assignment to the buckets.
 * 4.  Define NO_LIST_OUTPUT to suppress printing of the lists.
 *
 * IPP2:  7.2.6 (pp. 410 and ff.)
 */

#include <assert.h>
#include <limits.h>
#include <math.h>    /* For ceil   */
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  /* For memset */

#include "timer.h"

/* Seed for generating list */
#define SEED1 1

/* Seed for picking sample */
#define SEED2 2

/* Only used when DEBUG is defined */
#define MAX_STRING 1000


/*---------------------------------------------------------------------
 * Function prototypes
 */
int  Check_sorted(int list[], int n);
void Choose_sample(int list[], int n, int sample[], int s);
void Choose_splitters(int sample[], int s, int splitters[], int thread_count);
int  Compare(const void *a_p, const void *b_p);
void Get_args(int argc, char* argv[], int* m_p);
void Gen_list(int list[], int n, int modulus);
int  In_chosen(int sample[], int sz, int val);
void Print_list(char title[], int list[], int n);
void Read_list(char prompt[], int list[], int n);
void Sample_sort(void);

/*---------------------------------------------------------------------
 * Shared data structures
 */
int thread_count, n, s;
int *list;
int *splitters;
int *th_sublist_szs;

/*---------------------------------------------------------------------
 * Function:  main
 */
int main(int argc, char* argv[]) {
    int modulus;
    double start, finish;

    Get_args(argc, argv, &modulus);

    list = malloc(n*sizeof(int));

    if (modulus == 0)
        Read_list("Enter the list", list, n);
    else
        Gen_list(list, n, modulus);
#   ifndef NO_LIST_OUTPUT 
    Print_list("Before sort, list = ", list, n);
#   endif

    splitters = malloc((thread_count-1)*sizeof(int));
    th_sublist_szs = malloc(thread_count*sizeof(int));

    GET_TIME(start);
    Sample_sort();
    GET_TIME(finish);

#   ifndef NO_LIST_OUTPUT 
    Print_list("After sort, list = ", list, n);
#   endif
    printf("Elapsed time for the sort = %e seconds\n", finish-start);
    if (Check_sorted(list, n)) printf("List is sorted\n");
    free(list);

    free(splitters);
    free(th_sublist_szs);

    return 0;
}  /* main */


/*---------------------------------------------------------------------
 * Function:     Get_args
 * Purpose:      Get the command line arguments
 * In args:      argc, argv
 * Out args:     m_p
 * Out globals:  thread_count, n, s
 */
void Get_args(int argc, char* argv[], int* m_p) {
    if (argc != 3 && argc != 4) {
        fprintf(stderr,"%s <n> <s> <m>\n", argv[0]);
        fprintf(stderr,"      n = list size\n");
        fprintf(stderr,"      s = sample size\n");
        fprintf(stderr,"      m = if m is not present, get list from\n");
        fprintf(stderr,"         stdin.  Otherwise generate the list with\n");
        fprintf(stderr,"         m as the modulus.\n");
        exit(0);
    }

#   pragma omp parallel
    {
        thread_count = omp_get_num_threads();
    }

    n = strtol(argv[1], NULL, 10);
    s = strtol(argv[2], NULL, 10);
    if (argc == 4)
        *m_p = strtol(argv[3], NULL, 10);
    else 
        *m_p = 0;
}  /* Get_args */


/*---------------------------------------------------------------------
 * Function:   Read_list
 * Purpose:    Read a list of ints from stdin
 * In args:    prompt, n
 * Out arg:    list
 */
void Read_list(char prompt[], int list[], int n) {
    int i;

    printf("%s\n", prompt);
    for (i = 0; i < n; i++)
        scanf("%d", &list[i]);
}  /* Read_list */


/*---------------------------------------------------------------------
 * Function:   Gen_list
 * Purpose:    Generate a list of ints using the random function
 * In args:    n, modulus 
 * Out arg:    list
 */
void Gen_list(int list[], int n, int modulus) {
    int i;

    srandom(SEED1);
    for (i = 0; i < n; i++)
        list[i] = random() % modulus;
}  /* Gen_list */


/*---------------------------------------------------------------------
 * Function:   Print_list
 * Purpose:    Print a list of ints to stdout
 * In args:    all
 */
void Print_list(char title[], int list[], int n) {
    int i;

    printf("%s  ", title);
    for (i = 0; i < n; i++)
        printf("%d ", list[i]);
    printf("\n");
}  /* Print_list */


/*---------------------------------------------------------------------
 * Function:   Check_sorted
 * Purpose:    Determine whether the list is sorted in increasing order
 * In args:    all
 * Ret val:    1 if the list is sorted, 0 otherwise
 */
int Check_sorted(int list[], int n) {
    int i;

    for (i = 0; i < n-1; i++)
        if (list[i] > list[i+1]) {
            printf("list isn't sorted: list[%d] = %d > %d = list[%d]\n",
                    i, list[i], list[i+1], i+1);
            return 0;
        }
    return 1;
}


/*---------------------------------------------------------------------
 * Function:        Sample_sort
 * Purpose:         Use sample sort to sort a list of n ints
 * globals in:      n:  size of the list
 *                  s:  sample size
 *                  thread_count:  number of buckets and number of threads
 * In/out global:   list
 * Scratch globals: splitters
 */
void Sample_sort(void) {
    int* sample;

    sample = malloc(s*sizeof(int));
    Choose_sample(list, n, sample, s);
#   ifdef DEBUG
    Print_list("sample =", sample, s);
#   endif

    Choose_splitters(sample, s, splitters, thread_count);
#   ifdef DEBUG
    Print_list("splitters =", splitters, thread_count-1);
#   endif
    free(sample);

#   pragma omp parallel
    {
        int my_rank = omp_get_thread_num();
        int my_sublist_sz, *my_sublist;
        int my_offset = 0;

        my_sublist_sz = (n / thread_count) * 2;
        my_sublist = malloc(my_sublist_sz*sizeof(int));
        if (my_sublist == NULL) {
            fprintf(stderr, "Could not allocate my_sublist!\n");
        }

        int my_lo, my_hi;
        if (my_rank == 0) {
            my_lo = INT_MIN;
            my_hi = splitters[my_rank];
        } else if (my_rank == thread_count - 1) {
            my_lo = splitters[my_rank - 1];
            my_hi = INT_MAX;
        } else {
            my_lo = splitters[my_rank - 1];
            my_hi = splitters[my_rank];
        }

        if (omp_get_num_threads() == 1) {
            my_lo = INT_MIN;
            my_hi = INT_MAX;
        }

        int i;
        th_sublist_szs[my_rank] = 0;
        for (i = 0; i < n; ++i) {
            if (list[i] >= my_lo && list[i] < my_hi) {
                if (th_sublist_szs[my_rank] >= my_sublist_sz) {
                    // Sublist too small; realloc
                    my_sublist_sz *= 2;
                    int *new_sublist = realloc(my_sublist, my_sublist_sz*sizeof(int));
                    if (new_sublist == NULL) {
                        fprintf(stderr, "Could not reallocate my_sublist!\n");
                        continue;
                    } else {
                        my_sublist = new_sublist;
                    }
                }
                my_sublist[th_sublist_szs[my_rank]++] = list[i];
            }
        }

        qsort(my_sublist, th_sublist_szs[my_rank], sizeof(int), Compare);

#       pragma omp barrier

        for (i = 0; i < my_rank; ++i) {
            my_offset += th_sublist_szs[i];
        }

        memcpy(list + my_offset, my_sublist, th_sublist_szs[my_rank] * sizeof(int));
        free(my_sublist);
    }

}  /* Sample_sort */


/*----------------------------------------------------------------------
 * Function:   Compare
 * Purpose:    Comparison function for use in calls to qsort
 * In args:    a_p, b_p
 * Ret val:    See below
 */
int Compare(const void *a_p, const void *b_p) {
    int a = *((int*) a_p);
    int b = *((int*) b_p);
    return a - b;
}  /* Compare */


/*----------------------------------------------------------------------
 * Function:   In_chosen
 * Purpose:    Check whether the subscript j has already been chosen
 *             for the sample.  If it has return 1.  Otherwise return
 *             0.
 * In args:    chosen:  subscripts chosen so far
 *             sz:  number of subscripts chosen so far
 *             j:  current candidate for a new subscript
 * Ret val:    True, if j has already been chosen; False otherwise
 */
int In_chosen(int chosen[], int sz, int j) {
    int i;

    for (i = 0 ; i < sz; i++)
        if (chosen[i] == j)
            return 1;
    return 0;
}  /* In_chosen */


/*----------------------------------------------------------------------
 * Function:   Choose_sample
 * Purpose:    Choose the sample from the original list.
 * In args:    list, n, s
 * Out arg:    sample
 * Note:       The sample is sorted before return.
 */
void Choose_sample(int list[], int n, int sample[], int s) {
    int i, j;
    int chosen[s];

    srandom(SEED2);
    for (i = 0; i < s; i++) {
        j = random() % n;
#     ifdef BIG_DEBUG
        printf("i = %d, j = %d\n", i, j);
#     endif
        /* Make sure j hasn't been chosen yet */
        while (In_chosen(chosen, i, j))
            j = random() % n;
        chosen[i] = j;
        sample[i] = list[j];
    }
    qsort(sample, s, sizeof(int), Compare);

#  ifdef BIG_DEBUG
    Print_list("In Choose_sample, chosen = ", chosen, s);
#  endif
}  /* Choose_sample */


/*----------------------------------------------------------------------
 * Function:   Choose_splitters 
 * Purpose:    Determine the cutoffs for the buckets.  There will be
 *             b-1 splitters: splitters[0], splitters[1], ..., 
 *             splitters[b-2], and each bucket should get s/b elements
 *             of the sample.
 * In args:    sample, s, b = thread_count
 * Out arg:    splitters
 * Note:       The splitters and buckets are
 *             bucket 0 =      -infty    <= x < splitters[0] 
 *             bucket 1 =   splitters[0] <= x < splitters[1] 
 *             bucket 2 =   splitters[1] <= x < splitters[2] 
 *                 ...
 *             bucket b-1 = splitters[b-2] <= x < infty
 */
void Choose_splitters(int sample[], int s, int splitters[], int b) {
    assert(s > b);
    int per_bucket = s/b; // Note that s is evenly divisible by b
    int i, j;

    for (i = 0; i < b-1; i++) {
        j = (i+1)*per_bucket;
        // Note that j >= 1, since i+1 >= 0 and per_bucket >= 1
        // Also note that dividing by 2.0 is necessary:  otherwise
        //    integer division will take the floor and the call
        //    to ceil will have no effect
        splitters[i] = ceil((sample[j-1] + sample[j])/2.0);
    }
}  /* Choose_splitters */

