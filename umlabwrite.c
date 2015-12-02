#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"

extern void Um_write_sequence(FILE *output, Seq_T asm);

extern void emit_halt_test(Seq_T asm);
extern void emit_goto_test(Seq_T asm);
extern void emit_loadval_test(Seq_T asm);
extern void emit_input_test(Seq_T asm);
extern void emit_add_test(Seq_T asm);
extern void emit_mul_test(Seq_T asm);
extern void emit_div_test(Seq_T asm);
extern void emit_nand_test(Seq_T asm);
extern void emit_cmove_test(Seq_T asm);
extern void emit_segLoadStore_test(Seq_T asm);
extern void emit_mapSegment_test(Seq_T asm);
extern void emit_unmapSegment_test(Seq_T asm);

static struct test_info {
        const char *name;
        const char *test_input;            /* NULL means no input needed */
        const char *expected_output;
        /* writes instructions into sequence */
        void (*emit_test) (Seq_T stream);
} tests[] = {
        {"halt", NULL, "", emit_halt_test},
        {"goto", NULL, "GOTO passed.\n", emit_goto_test},
        {"loadval", NULL, "T\n", emit_loadval_test},
        {"input", "T", "T\n", emit_input_test},
        {"add", "1", "2\n", emit_add_test},
        {"mul", NULL, "b\n", emit_mul_test},
        {"div", NULL, "1\n", emit_div_test},
        {"nand", NULL, "0\n", emit_nand_test},
        {"cmove", NULL, "b\na\n", emit_cmove_test},
        {"segLoadStore", NULL, "T\n", emit_segLoadStore_test},
        {"mapSegment", NULL, "1\n", emit_mapSegment_test},
        {"unmapSegment", NULL, "1\n", emit_unmapSegment_test},
};

#define NTESTS (sizeof(tests)/sizeof(tests[0]))

/*
 * open file 'path' for writing, then free the pathname;
 * if anything fails, checked runtime error
 */
static FILE *open_and_free_pathname(char *path);

/*
 * if contents is NULL or empty, remove the given 'path', 
 * otherwise write 'contents' into 'path'.  Either way, free 'path'.
 */
static void write_or_remove_file(char *path, const char *contents);

static void write_test_files(struct test_info *test)
{
        FILE *binary = open_and_free_pathname(Fmt_string("%s.um", test->name));
        Seq_T asm = Seq_new(0);
        test->emit_test(asm);
        Um_write_sequence(binary, asm);
        Seq_free(&asm);
        fclose(binary);

        write_or_remove_file(Fmt_string("%s.0", test->name), test->test_input);
        write_or_remove_file(Fmt_string("%s.1", test->name),
                             test->expected_output);
}

int main(int argc, char *argv[])
{
        bool failed = false;
        if (argc == 1)
                for (unsigned i = 0; i < NTESTS; i++) {
                        printf("***** Writing test '%s'.\n", tests[i].name);
                        write_test_files(&tests[i]);
        } else
                for (int j = 1; j < argc; j++) {
                        bool tested = false;
                        for (unsigned i = 0; i < NTESTS; i++)
                                if (!strcmp(tests[i].name, argv[j])) {
                                        tested = true;
                                        write_test_files(&tests[i]);
                                }
                        if (!tested) {
                                failed = true;
                                fprintf(stderr,
                                        "***** No test named %s *****\n",
                                        argv[j]);
                        }
                }
        return failed;   /* failed nonzero == exit nonzero == failure */
}

static void write_or_remove_file(char *path, const char *contents)
{
        if (contents == NULL || *contents == '\0') {
                remove(path);
        } else {
                FILE *input = fopen(path, "wb");
                assert(input);
                fputs(contents, input);
                fclose(input);
        }
        free(path);
}

static FILE *open_and_free_pathname(char *path)
{
        FILE *fp = fopen(path, "wb");
        assert(fp != NULL);

        free(path);
        return fp;
}
