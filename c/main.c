#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNASSIGNED -1
#define TRUE 1
#define FALSE 0

#define MAX_LINE_LENGTH 1024
#define MAX_NUM_CLAUSES 1070
#define MAX_NUM_LITERALS 350

#define abs(x) ((x) < 0 ? -(x) : (x))

int **clauses;
int num_clauses;
int num_literals;
int assignments[MAX_NUM_LITERALS];
int sorted_literals[MAX_NUM_LITERALS];
int positive_count[MAX_NUM_LITERALS];
int negative_count[MAX_NUM_LITERALS];

typedef struct {
    int **clauses;
    int num_clauses;
    int assignments[MAX_NUM_LITERALS];
} State;

State state_stack[MAX_NUM_LITERALS];
int stack_top = -1;

typedef struct {
    int literal;
    int frequency;
    int statistica;
} LiteralFrequency;

int compare(const void *a, const void *b) {
    LiteralFrequency *lf_a = (LiteralFrequency *)a;
    LiteralFrequency *lf_b = (LiteralFrequency *)b;
    return lf_b->frequency - lf_a->frequency;
}

void sort_literals_by_frequency() {
    int frequency[MAX_NUM_LITERALS + 1] = {0};

    for (int i = 0; i < num_clauses; i++) {
        for (int j = 0; clauses[i][j] != 0; j++) {
            int literal = abs(clauses[i][j]);
            frequency[literal]++;
        }
    }

    LiteralFrequency literal_frequencies[MAX_NUM_LITERALS];
    int count = 0;
    for (int i = 1; i <= num_literals; i++) {
        if (frequency[i] > 0) {
            literal_frequencies[count].literal = i;
            literal_frequencies[count].frequency = frequency[i];
            count++;
        }
    }

    qsort(literal_frequencies, count, sizeof(LiteralFrequency), compare);

    for (int i = 0; i < count; i++) {
        sorted_literals[i] = literal_frequencies[i].literal;
    }
}

void push_state() {
    stack_top++;
    state_stack[stack_top].clauses = clauses;
    state_stack[stack_top].num_clauses = num_clauses;
    memcpy(state_stack[stack_top].assignments, assignments, sizeof(assignments));
}

void pop_state() {
    if (stack_top < 0) return;

    clauses = state_stack[stack_top].clauses;
    num_clauses = state_stack[stack_top].num_clauses;
    memcpy(assignments, state_stack[stack_top].assignments, sizeof(assignments));
    stack_top--;
}

int clause_is_true(int *clause) {
    for (int *ptr = clause; *ptr; ++ptr) {
        int literal = *ptr;
        if ((literal > 0 && assignments[literal] == TRUE) ||
            (literal < 0 && assignments[-literal] == FALSE)) {
            return 1;
        }
    }
    return 0;
}

void filter_unsatisfied_clauses() {
    int **new_clauses = (int **)malloc(num_clauses * sizeof(int *));
    int new_num_clauses = 0;

    for (int **clauses_ptr = clauses, **new_clauses_ptr = new_clauses; 
         clauses_ptr < clauses + num_clauses; 
         clauses_ptr++) {
        if (!clause_is_true(*clauses_ptr)) {
            *new_clauses_ptr = *clauses_ptr;
            new_clauses_ptr++;
            new_num_clauses++;
        }
    }

    clauses = new_clauses;
    num_clauses = new_num_clauses;
}

void pure_literal_elimination() {
    for (int i = 1; i <= num_literals; i++) {
        positive_count[i] = 0;
        negative_count[i] = 0;
    }

    for (int i = 0; i < num_clauses; i++) {
        int *clause = clauses[i];
        for (int j = 0; clause[j] != 0; j++) {
            int literal = clause[j];
            if (literal > 0) {
                positive_count[literal]++;
            } else {
                negative_count[-literal]++;
            }
        }
    }

    for (int i = 1; i <= num_literals; i++) {
        if (assignments[i] == UNASSIGNED) {
            if (positive_count[i] > 0 && negative_count[i] == 0) {
                assignments[i] = TRUE;
            } else if (negative_count[i] > 0 && positive_count[i] == 0) {
                assignments[i] = FALSE;
            }
        }
    }

    filter_unsatisfied_clauses();
}

int all_clauses_are_satisfied() {
    for (int **clauses_ptr = clauses; clauses_ptr < clauses + num_clauses; clauses_ptr++) {
        if (!clause_is_true(*clauses_ptr)) {
            return 0;
        }
    }
    return 1;
}

int clause_is_false(int *clause) {
    int num_false_literals = 0;
    int clause_size = 0;
    for (int *ptr = clause; *ptr; ++ptr) {
        int literal = *ptr;
        clause_size++;
        if ((literal > 0 && assignments[literal] == FALSE) ||
            (literal < 0 && assignments[-literal] == TRUE)) {
            num_false_literals++;
        }
    }

    return num_false_literals == clause_size;
}

int unit_propagation(int *added_literals, int *num_added_literals) {
    while (1) {
        int unit_clause_found = 0;

        for (int **clause_ptr = clauses; clause_ptr < clauses + num_clauses; clause_ptr++) {
            if (clause_is_true(*clause_ptr)) {
                continue;
            }
            if (clause_is_false(*clause_ptr)) {
                return 0;
            }

            int num_unassigned = 0;
            int missing_element = 0;

            for (int *literal_ptr = *clause_ptr; *literal_ptr != 0; literal_ptr++) {
                int literal = *literal_ptr;
                int abs_literal = abs(literal);

                if (assignments[abs_literal] == UNASSIGNED) {
                    num_unassigned++;
                    missing_element = literal;

                    if (num_unassigned > 1) {
                        break;
                    }
                }
            }

            if (num_unassigned == 1) {
                int abs_missing = abs(missing_element);
                assignments[abs_missing] = (missing_element > 0) ? TRUE : FALSE;
                added_literals[*num_added_literals] = abs_missing;
                (*num_added_literals)++;

                unit_clause_found = 1;
            }
        }

        if (!unit_clause_found) {
            break;
        }
    }
    filter_unsatisfied_clauses();
    return 1;
}

int dpll() {
    if (all_clauses_are_satisfied()) {
        return 1;
    }

    for (int *literal_ptr = sorted_literals + 1; literal_ptr <= sorted_literals + num_literals; literal_ptr++) {
        int literal = *literal_ptr;
        if (assignments[literal] == UNASSIGNED) {
            int value = positive_count[literal] > negative_count[literal] ? TRUE : FALSE;
            push_state();

            assignments[literal] = value;

            int added_literals[MAX_NUM_LITERALS];
            int num_added_literals = 0;

            int result = unit_propagation(added_literals, &num_added_literals);
            if (result)
                if (dpll())
                    return 1;

            // daca nu am gasit o solutie, revin la starea anterioara
            pop_state();

            push_state();

            assignments[literal] = value == TRUE ? FALSE : TRUE;

            int added_literals2[MAX_NUM_LITERALS];
            num_added_literals = 0;

            result = unit_propagation(added_literals2, &num_added_literals);
            if (result)
                if (dpll())
                    return 1;

            // daca nu am gasit o solutie, revin la starea anterioara
            pop_state();

            return 0;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    FILE *input_file = fopen(argv[1], "r");
    FILE *output_file = fopen(argv[2], "w");

    char line[1024];
    int current_clause = 0;
    clauses = NULL;

    while (fgets(line, sizeof(line), input_file)) {
        if (line[0] == 'c') {
            continue;
        }
        if (strlen(line) == 0) {
            continue;
        }

        if (line[0] == 'p') {
            sscanf(line, "p cnf %d %d", &num_literals, &num_clauses);
        } else if (line[0] == '0') {
            break;
        } else {
            if (current_clause > num_clauses) {
                break;
            }
            if (clauses == NULL) {
                // am nevoie de malloc pt ca unele inputuri au multe clauze (peste 20.000)
                clauses = (int **)malloc((num_clauses + 1) * sizeof(int *));
            }

            int index = 0;
            char *token = strtok(line, " ");
            int clause[MAX_NUM_LITERALS];
            while (token != NULL) {
                if (token[0] == '0') {
                    break;
                }
                int number = 0;
                int is_negative = 1;
                if (token[0] == '-') {
                    is_negative = -1;
                    token++;
                }
                for (int i = 0; i < strlen(token); i++) {
                    number = number * 10 + (token[i] - '0');
                }
                clause[index++] = number * is_negative;
                token = strtok(NULL, " ");
            }
            clause[index++] = 0;
            clauses[current_clause] = (int *)malloc((num_literals + 1) * sizeof(int));
            memcpy(clauses[current_clause], clause, index * sizeof(int));
            current_clause++;
        }
    }

    for (int i = 1; i <= num_literals; i++) {
        assignments[i] = UNASSIGNED;
    }
    sort_literals_by_frequency();
    pure_literal_elimination();

    int added_literals[num_literals + 1];
    int num_added_literals = 0;

    int result = unit_propagation(added_literals, &num_added_literals);

    if (result)
        result = dpll();

    if (!result) {
        fprintf(output_file, "s UNSATISFIABLE\n");
    } else {
        for (int i = 1; i <= num_literals; i++) {
            if (assignments[i] == UNASSIGNED) {
                assignments[i] = TRUE;
            }
        }

        fprintf(output_file, "s SATISFIABLE\n");
        fprintf(output_file, "v ");

        for (int i = 1; i <= num_literals; i++) {
            if (assignments[i] == TRUE) {
                fprintf(output_file, "%d ", i);
            } else if (assignments[i] == FALSE) {
                fprintf(output_file, "-%d ", i);
            }
        }
        fprintf(output_file, "0\n");
    }

    fclose(input_file);
    fclose(output_file);

    for (int i = 0; i < num_clauses; i++) {
        free(clauses[i]);
    }
    free(clauses);

    return 0;
}
