#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define listlength 60000
#define wordslength 25

char **getwords (FILE *fp, int *n);
void free_array (char** words, int rows);

int main (int argc, char **argv) {

    int i, nwords = 0;
    char **words = NULL;  /* file given as argv[1] */
    char *fname = argc > 1 ? argv[1] : "/usr/share/dict/cracklib-small"; /* ##### Change this file to use different dictionary files ##### */
    FILE *dictionary = fopen (fname, "r");

    if (!dictionary) { /* validate file open */
        fprintf (stderr, "error: file open failed.\n");
        return 1;
    }

    if (!(words = getwords (dictionary, &nwords))) {
        fprintf (stderr, "error: getwords returned NULL.\n");
        return 1;
    }
    fclose(dictionary);

    printf ("\n '%d' words read from '%s'\n\n", nwords, fname);

// ##### this is where your code will go, maybe a few things outside to initialize things. #####
//    for (i = 0; i < nwords; i++) {
//        printf ("%s\n", words[i]);
//    }

    free_array (words, nwords);

    return 0;
}

/* read all words 1 per-line, from 'fp', return
 * pointer-to-pointers of allocated strings on 
 * success, NULL otherwise, 'n' updated with 
 * number of words read.
 */
char **getwords (FILE *fp, int *n) {

    char **words = NULL;
    char buf[wordslength + 1] = {0};
    int maxlen = listlength > 0 ? listlength : 1;

    if (!(words = calloc (maxlen, sizeof *words))) {
        fprintf (stderr, "getwords() error: virtual memory exhausted.\n");
        return NULL;
    }

    while (fgets (buf, wordslength + 1, fp)) {

        size_t wordlen = strlen (buf);  /* get word length */

        if (buf[wordlen - 1] == '\n')   /* strip '\n' */
            buf[--wordlen] = 0;

        words[(*n)++] = strdup (buf);   /* allocate/copy */

        if (*n == maxlen) { /* realloc as required, update maxlen */
            void *tmp = realloc (words, maxlen * 2 * sizeof *words);
            if (!tmp) {
                fprintf (stderr, "getwords() realloc: memory exhausted.\n");
                return words; /* to return existing words before failure */
            }
            words = tmp;
            memset (words + maxlen, 0, maxlen * sizeof *words);
            maxlen *= 2;
        }
    }

    return words;
}

void free_array (char **words, int rows){

    int i;
    for (i = 0; i < rows; i++){
        free (words[i]);
    }
    free(words);
}
