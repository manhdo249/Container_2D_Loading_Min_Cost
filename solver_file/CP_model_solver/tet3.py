from ortools.sat.python import cp_model

def read_input():
    # Nhập số lượng pack và bin
    n_packs, n_bins = map(int, input().split())

    packs = []
    bins = []

    # Nhập kích thước của các pack
    for i in range(n_packs):
        width, height = map(int, input().split())
        packs.append((width, height))

    # Nhập kích thước và chi phí của các bin
    for i in range(n_bins):
        width, height, cost = map(int, input().split())
        bins.append((width, height, cost))

    return n_packs, n_bins, packs, bins

def main_solver(time_limit: int = 600):
    n_packs, n_bins, packs, bins = read_input()

    max_width = max(x[0] for x in bins)
    max_height = max(x[1] for x in bins)

    # Tạo mô hình
    model = cp_model.CpModel()

    # Khai báo các biến
    X = {}
    R = []

    for i in range(n_packs):
        R.append(model.NewBoolVar(f'package_{i}_rotated'))
        for j in range(n_bins):
            X[i, j] = model.NewBoolVar(f'pack_{i}_in_bin_{j}')

    Z = [model.NewBoolVar(f'bin_{j}_is_used') for j in range(n_bins)]

    # Tọa độ các pack
    r = []  # right coordinate
    l = []  # left coordinate
    t = []  # top coordinate
    b = []  # bottom coordinate

    for i in range(n_packs):
        r.append(model.NewIntVar(0, max_width, f'r_{i}'))
        l.append(model.NewIntVar(0, max_width, f'l_{i}'))
        t.append(model.NewIntVar(0, max_height, f't_{i}'))
        b.append(model.NewIntVar(0, max_height, f'b_{i}'))

        # Nếu pack bị xoay thì thay đổi tọa độ
        model.Add(r[i] == l[i] + packs[i][0]).OnlyEnforceIf(R[i].Not())
        model.Add(r[i] == l[i] + packs[i][1]).OnlyEnforceIf(R[i])
        model.Add(t[i] == b[i] + packs[i][1]).OnlyEnforceIf(R[i].Not())
        model.Add(t[i] == b[i] + packs[i][0]).OnlyEnforceIf(R[i])

    # Ràng buộc các pack chỉ có thể được đặt vào một bin duy nhất
    for i in range(n_packs):
        model.Add(sum(X[i, j] for j in range(n_bins)) == 1)

    # Nếu pack nằm trong bin, thì không vượt quá kích thước của bin
    for i in range(n_packs):
        for j in range(n_bins):
            model.Add(r[i] <= bins[j][0]).OnlyEnforceIf(X[i, j])
            model.Add(t[i] <= bins[j][1]).OnlyEnforceIf(X[i, j])

    # Nếu 2 pack nằm trong cùng một bin thì không được chồng lên nhau
    for i in range(n_packs):
        for k in range(i + 1, n_packs):
            a1 = model.NewBoolVar('a1')
            model.Add(r[i] <= l[k]).OnlyEnforceIf(a1)
            model.Add(r[i] > l[k]).OnlyEnforceIf(a1.Not())

            a2 = model.NewBoolVar('a2')
            model.Add(t[i] <= b[k]).OnlyEnforceIf(a2)
            model.Add(t[i] > b[k]).OnlyEnforceIf(a2.Not())

            a3 = model.NewBoolVar('a3')
            model.Add(r[k] <= l[i]).OnlyEnforceIf(a3)
            model.Add(r[k] > l[i]).OnlyEnforceIf(a3.Not())

            a4 = model.NewBoolVar('a4')
            model.Add(t[k] <= b[i]).OnlyEnforceIf(a4)
            model.Add(t[k] > b[i]).OnlyEnforceIf(a4.Not())

            for j in range(n_bins):
                model.AddBoolOr([a1, a2, a3, a4]).OnlyEnforceIf(X[i, j], X[k, j])

    # Xác định bin nào đã được sử dụng
    for j in range(n_bins):
        b1 = model.NewBoolVar('b')
        model.Add(sum(X[i, j] for i in range(n_packs)) == 0).OnlyEnforceIf(b1)
        model.Add(Z[j] == 0).OnlyEnforceIf(b1)
        model.Add(sum(X[i, j] for i in range(n_packs)) != 0).OnlyEnforceIf(b1.Not())
        model.Add(Z[j] == 1).OnlyEnforceIf(b1.Not())

    # Hàm mục tiêu: giảm chi phí
    cost = sum(Z[j] * bins[j][2] for j in range(n_bins))
    model.Minimize(cost)

    # Giải quyết mô hình
    solver = cp_model.CpSolver()

    # Thiết lập các tham số solver
    solver.parameters.max_time_in_seconds = 3.0  # Thời gian giới hạn là 3 giây
    solver.parameters.num_search_workers = 1  # Sử dụng 1 worker để tìm kiếm

    # Giải quyết mô hình
    status = solver.Solve(model)

    # In kết quả
    if status == cp_model.OPTIMAL or status == cp_model.FEASIBLE:
        for i in range(n_packs):
            t_i = -1
            for j in range(n_bins):
                if solver.Value(X[i, j]) == 1:
                    t_i = j + 1  # Bin được sử dụng (thêm 1 vì chỉ số bắt đầu từ 0)
                    break
            o_i = 1 if solver.Value(R[i]) == 1 else 0
            print(f'{i+1} {t_i} {solver.Value(r[i])} {solver.Value(t[i])} {o_i}')
    else:
        print('NO SOLUTIONS')

if __name__ == "__main__":
    main_solver()
