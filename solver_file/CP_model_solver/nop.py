#PYTHON 
from ortools.sat.python import cp_model

def read_input():
    n_packs, n_bins = map(int, input().split())
    packs = [tuple(map(int, input().split())) for _ in range(n_packs)]
    bins = [tuple(map(int, input().split())) for _ in range(n_bins)]
    return n_packs, n_bins, packs, bins

def main_solver():
    n_packs, n_bins, packs, bins = read_input()

    max_width = max(x[0] for x in bins)
    max_height = max(x[1] for x in bins)

    # Create the model
    model = cp_model.CpModel()

    # Variables
    X = {}
    R = []
    for i in range(n_packs):
        R.append(model.NewBoolVar(f'package_{i}_rotated'))
        for j in range(n_bins):
            X[i, j] = model.NewBoolVar(f'pack_{i}_in_bin_{j}')

    Z = [model.NewBoolVar(f'bin_{j}_is_used') for j in range(n_bins)]

    # Coordinate variables
    r, l, t, b = [], [], [], []
    for i in range(n_packs):
        r.append(model.NewIntVar(0, max_width, f'r_{i}'))
        l.append(model.NewIntVar(0, max_width, f'l_{i}'))
        t.append(model.NewIntVar(0, max_height, f't_{i}'))
        b.append(model.NewIntVar(0, max_height, f'b_{i}'))

        # Adjust coordinates based on rotation
        model.Add(r[i] == l[i] + packs[i][0]).OnlyEnforceIf(R[i].Not())
        model.Add(r[i] == l[i] + packs[i][1]).OnlyEnforceIf(R[i])
        model.Add(t[i] == b[i] + packs[i][1]).OnlyEnforceIf(R[i].Not())
        model.Add(t[i] == b[i] + packs[i][0]).OnlyEnforceIf(R[i])

    # Constraints
    # Each pack can only be placed in one bin
    for i in range(n_packs):
        model.Add(sum(X[i, j] for j in range(n_bins)) == 1)

    # If pack is in bin, it cannot exceed the bin size
    for i in range(n_packs):
        for j in range(n_bins):
            model.Add(r[i] <= bins[j][0]).OnlyEnforceIf(X[i, j])
            model.Add(t[i] <= bins[j][1]).OnlyEnforceIf(X[i, j])

    # No overlap between packs in the same bin
    for i in range(n_packs):
        for k in range(i + 1, n_packs):
            a1 = model.NewBoolVar(f'a1_{i}_{k}')
            a2 = model.NewBoolVar(f'a2_{i}_{k}')
            a3 = model.NewBoolVar(f'a3_{i}_{k}')
            a4 = model.NewBoolVar(f'a4_{i}_{k}')
            model.Add(r[i] <= l[k]).OnlyEnforceIf(a1)
            model.Add(r[i] > l[k]).OnlyEnforceIf(a1.Not())
            model.Add(t[i] <= b[k]).OnlyEnforceIf(a2)
            model.Add(t[i] > b[k]).OnlyEnforceIf(a2.Not())
            model.Add(r[k] <= l[i]).OnlyEnforceIf(a3)
            model.Add(r[k] > l[i]).OnlyEnforceIf(a3.Not())
            model.Add(t[k] <= b[i]).OnlyEnforceIf(a4)
            model.Add(t[k] > b[i]).OnlyEnforceIf(a4.Not())

            for j in range(n_bins):
                model.AddBoolOr([a1, a2, a3, a4]).OnlyEnforceIf(X[i, j], X[k, j])

    # Track if bins are used
    for j in range(n_bins):
        b1 = model.NewBoolVar(f'b1_{j}')
        model.Add(sum(X[i, j] for i in range(n_packs)) == 0).OnlyEnforceIf(b1)
        model.Add(Z[j] == 0).OnlyEnforceIf(b1)
        model.Add(sum(X[i, j] for i in range(n_packs)) != 0).OnlyEnforceIf(b1.Not())
        model.Add(Z[j] == 1).OnlyEnforceIf(b1.Not())

    # Minimize total cost
    cost = sum(Z[j] * bins[j][2] for j in range(n_bins))
    model.Minimize(cost)

    # Solve the model
    solver = cp_model.CpSolver()
    status = solver.Solve(model)

    # Output results
    if status == cp_model.OPTIMAL or status == cp_model.FEASIBLE:
        for i in range(n_packs):
            # Output index (1-based) and rotation status
            t_index = -1
            for j in range(n_bins):
                if solver.Value(X[i, j]) == 1:
                    t_index = j + 1  # 1-based index for bins
                    break
            rotated = solver.Value(R[i])
            x = solver.Value(r[i])
            y = solver.Value(t[i])

            # Print the solution in required format
            print(i + 1, t_index, x, y, rotated)

    else:
        print("NO SOLUTIONS")

if __name__ == "__main__":
    main_solver()
