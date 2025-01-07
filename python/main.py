import sys
from SATSolver import SATSolver, FALSE


def main():
    if len(sys.argv) != 3:
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    input = []

    with open(input_file, "r") as file:
        for line in file:
            if line[0] == "p":
                parts = line.split()
                num_literals = int(parts[2])
                # nu am nevoie de numarul de clauze
                continue

            if line[0] == "c" or line.strip() == "":
                # sar peste comentarii si linii goale
                continue

            clause = set()
            for item in line.strip().split():
                if int(item) != 0:
                    clause.add(int(item))

            input.append(clause)

    solver = SATSolver(num_literals, input)
    solver.pure_literal_elimination()
    solver.unit_propagation()

    output = solver.dpll()

    with open(output_file, "w") as file:
        if not output:
            file.write("s UNSATISFIABLE")
        else:
            file.write("s SATISFIABLE\n")
            file.write("v ")

            cnt = 0

            for literal in range(1, num_literals + 1):
                value = solver.get_literal_value(literal)
                if value == FALSE:
                    file.write(f"-{literal} ")
                else:
                    file.write(f"{literal} ")

                cnt += 1

                # afisez pe mai multe linii ca sa fie mai usor de citit
                if cnt % 10 == 0:
                    file.write("\n")
                    file.write("v ")

            file.write("0\n")


if __name__ == "__main__":
    main()
