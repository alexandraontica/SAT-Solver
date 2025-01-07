from collections import Counter

UNASSIGNED = -1
TRUE = 1
FALSE = 0


class SATSolver:
    def __init__(self, num_literals, input):
        self.__input = input
        self.__T = [UNASSIGNED] * (num_literals + 1)
        self.__sorted_literals = self.sort_literals_by_frequency()
        self.__num_literals = num_literals

    def get_literal_value(self, literal):
        return self.__T[abs(literal)]

    def sort_literals_by_frequency(self):
        # list comprehension poate fi putin mai rapid decat un for normal
        counter = Counter(abs(literal) for clause in self.__input for literal in clause)
        return [literal for literal, _ in counter.most_common()]

    def clause_is_true(self, clause):
        for literal in clause:
            if (literal > 0 and self.__T[literal] == TRUE) or (
                literal < 0 and self.__T[abs(literal)] == FALSE
            ):
                return True
        return False

    def pure_literal_elimination(self):
        positive_count = Counter()
        negative_count = Counter()

        for clause in self.__input:
            for literal in clause:
                if literal > 0:
                    positive_count[literal] += 1
                else:
                    negative_count[-literal] += 1

        for literal in range(1, self.__num_literals + 1):
            if self.__T[literal] == UNASSIGNED:
                if literal in positive_count and literal not in negative_count:
                    self.__T[literal] = TRUE
                elif literal in negative_count and literal not in positive_count:
                    self.__T[literal] = FALSE

        self.__input = [
            clause for clause in self.__input if not self.clause_is_true(clause)
        ]

    def all_clauses_are_satisfied(self):
        for clause in self.__input:
            if not self.clause_is_true(clause):
                return False
        return True

    def has_falsified_clause(self):
        for clause in self.__input:
            unassigned_literals_found = False
            is_true = False

            if self.clause_is_true(clause):
                is_true = True

            for literal in clause:
                abs_literal = abs(literal)

                if self.__T[abs_literal] == UNASSIGNED:
                    unassigned_literals_found = True
                    continue

            if not unassigned_literals_found and not is_true:
                return True

        return False
    
    def clause_is_false(self, clause):
        num_false_literals = 0
        for literal in clause:
            if (literal > 0 and self.__T[literal] == FALSE) or (
                literal < 0 and self.__T[-literal] == TRUE
            ):
                num_false_literals += 1

        if num_false_literals == len(clause):
            return True
        return False

    def unit_propagation(self):
        added_literals = []

        while True:
            unit_clause_found = False

            for clause in self.__input:
                if self.clause_is_true(clause):
                    # daca e adevarata, nu mai are rost sa caut, nu e clauza uitara
                    continue

                if self.clause_is_false(clause):
                    return added_literals, "false"

                num_unassigned = 0
                missing_element = None

                for literal in clause:
                    abs_literal = abs(literal)

                    if self.__T[abs_literal] == UNASSIGNED:
                        num_unassigned += 1
                        missing_element = literal

                        if num_unassigned > 1:
                            break

                if num_unassigned == 1:
                    abs_missing_element = abs(missing_element)

                    if missing_element > 0:
                        self.__T[abs_missing_element] = TRUE
                    else:
                        self.__T[abs_missing_element] = FALSE

                    unit_clause_found = True
                    added_literals.append(abs_missing_element)

            if not unit_clause_found:
                break

        # in cazul in care literalii adaugati nu sunt buni
        # si duc la UNSAT, trebuie sa ii retin ca sa ii pot scoate
        return added_literals, None

    def dpll(self):
        # if self.has_falsified_clause():
        #     return False

        if self.all_clauses_are_satisfied():
            return True

        for num in self.__sorted_literals:
            if self.__T[num] == UNASSIGNED:
                for value in [TRUE, FALSE]:  # verific si pentru negatul numarului
                    self.__T[num] = value

                    added_literals, val = self.unit_propagation()

                    if val is None:
                        if self.dpll():
                            return True

                    # nu am gasit o solutie, scoate literalii adaugati
                    self.__T[num] = UNASSIGNED
                    for var in added_literals:
                        self.__T[var] = UNASSIGNED
                return False

        return False
