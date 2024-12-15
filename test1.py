from ortools.linear_solver import pywraplp

def main():
    first_line = input().strip()
    while first_line == '':
        first_line = input().strip()
    n, m = map(int, first_line.split())
    
    A_line = input().strip()
    while A_line == '':
        A_line = input().strip()
    A = list(map(int, A_line.split()))
    if len(A) != n:
        raise ValueError("Số lượng dung lượng kho không khớp với n.")
    
    B_line = input().strip()
    while B_line == '':
        B_line = input().strip()
    B = list(map(int, B_line.split()))
    if len(B) != m:
        raise ValueError("Số lượng nhu cầu siêu thị không khớp với m.")
    
    C = []
    for i in range(n):
        cost_line = input().strip()
        while cost_line == '':
            cost_line = input().strip()
        cost = list(map(int, cost_line.split()))
        if len(cost) != m:
            raise ValueError(f"Số lượng chi phí tại kho {i+1} không khớp với m.")
        C.append(cost)
    
    solver = pywraplp.Solver.CreateSolver('GLOP')
    if not solver:
        print("Solver không khả dụng.")
        return
    
    x = {}
    for i in range(n):
        for j in range(m):
            x[i, j] = solver.NumVar(0, solver.infinity(), f'x_{i}_{j}')
    
    objective = solver.Objective()
    for i in range(n):
        for j in range(m):
            objective.SetCoefficient(x[i, j], C[i][j])
    objective.SetMinimization()
    
    for i in range(n):
        constraint = solver.Constraint(0, A[i])
        for j in range(m):
            constraint.SetCoefficient(x[i, j], 1)
    
    for j in range(m):
        constraint = solver.Constraint(B[j], solver.infinity())
        for i in range(n):
            constraint.SetCoefficient(x[i, j], 1)
    
    status = solver.Solve()
    
    if status != pywraplp.Solver.OPTIMAL:
        print("No optimal solution found.")
        return
    
    K = n * m
    print(K)
    for i in range(n):
        for j in range(m):
            value = x[i, j].solution_value()
            print(f"{i+1} {j+1} {value:.1f}")

if __name__ == "__main__":
    main()
