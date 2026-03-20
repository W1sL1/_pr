#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_FILE_SIZE 51200
#define MAX_FILENAME 40
enum State {
    NORMAL, IN_DOUBLE_QUOTE, IN_SINGLE_QUOTE, IN_SINGLE_COMMENT, IN_MULTI_COMMENT
};
void process_file(const char* input_filename, const char* output_filename) {
    FILE* input = fopen(input_filename, "r");
    if (!input) {
        //        error
        fprintf(stderr, "Cannot open input: %s\n", input_filename);return;
    }
    FILE* output = fopen(output_filename, "w");
    if (!output) {
        fprintf(stderr, "Cannot open output: %s\n", output_filename);fclose(input);return;
    }
    //          default void         byte        \0
    char* buffer = (char*)malloc(MAX_FILE_SIZE + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(input);
        fclose(output);
        return;
    }
    //no+-             *on arr 1el1by   max bytes   file* 
    size_t size = fread(buffer, 1, MAX_FILE_SIZE, input);
    //     end=\0
    buffer[size] = '\0'; fclose(input);
    enum State state = NORMAL;
    //  indx    flag for \n, 1>>may be skip i
    int i = 0, in_line_continuation = 0;
    while (i < size) { 
        //   current          next 
        char c = buffer[i], next = (i + 1 < size) ? buffer[i + 1] : '\0';
        if (in_line_continuation && c == '\n') {
            i++;
            in_line_continuation = 0;
            continue;
        }
        in_line_continuation = 0;
        switch (state) {
        case NORMAL:
            if (c == '"') { 
                //write c in file
                fputc(c, output);
                state = IN_DOUBLE_QUOTE;
            }
            else if (c == '\'') {
                fputc(c, output);
                state = IN_SINGLE_QUOTE;
            }
            else if (c == '/' && next == '/') {
                state = IN_SINGLE_COMMENT;
                i++;
            }
            else if (c == '/' && next == '*') {
                state = IN_MULTI_COMMENT;
                i++;
            }
            else {
                if (c == '\\' && next == '\n') {
                    fputc(c, output);
                    fputc(next, output);
                    in_line_continuation = 1;
                    i++;
                }
                // default
                else fputc(c, output); 
            }
            // end
            break; 
        // 
        case IN_DOUBLE_QUOTE:
            fputc(c, output);
            if (c == '\\' && next == '\n') {
                fputc(next, output);
                in_line_continuation = 1;
                i++;
            }
            else if (c == '\\' && next != '\0') {
                fputc(next, output);
                i++;
            }
            else if (c == '"') {
                state = NORMAL;
            }
            break;
        case IN_SINGLE_QUOTE:
            fputc(c, output);
            if (c == '\\' && next == '\n') {
                fputc(next, output);
                in_line_continuation = 1;
                i++;
            }
            else if (c == '\\' && next != '\0') {
                fputc(next, output);
                i++;
            }
            else if (c == '\'') {
                state = NORMAL;
            }
            break;
        case IN_SINGLE_COMMENT:
            if (c == '\\' && next == '\n') {
                in_line_continuation = 1;
                i++;
            }
            else if (c == '\n') {
                fputc(c, output);
                state = NORMAL;
            }
            break;
        case IN_MULTI_COMMENT:
            if (c == '*' && next == '/') {
                state = NORMAL;
                i++;
            }
            break;
        }
        i++;
    }
    free(buffer);
    fclose(output);
}
//                  recurs. from project.txt,        beg. idx
void process_project_recursive(FILE* project, int n, int idx) {
    if (idx >= n) return; //if all done 
    char input_filename[MAX_FILENAME], output_filename[MAX_FILENAME];
    if (fscanf(project, "%s", input_filename) != 1) {
        fprintf(stderr, "Error reading filename %d\n", idx + 1);
        // skip, next 
        process_project_recursive(project, n, idx + 1);
        return;
    }
    //          to               from 
    strcpy(output_filename, input_filename);
    //           reverse find .              
    char* dot = strrchr(output_filename, '.'); 
    if (dot) *dot = '\0';
    // plus       to          what 
    strcat(output_filename, ".wc"); 
    process_file(input_filename, output_filename);
    // next
    process_project_recursive(project, n, idx + 1);
}
//
int main() {
    FILE* project = fopen("project.txt", "r");
    if (!project) {
        fprintf(stderr, "Cannot open project.txt\n");
        return 1;
    }
    int n;
    //    before first space 
    if (fscanf(project, "%d", &n) != 1) {
        fprintf(stderr, "Invalid format in project.txt\n");
        fclose(project);
        return 1;
    }
    process_project_recursive(project, n, 0);
    fclose(project);
    return 0;
}
