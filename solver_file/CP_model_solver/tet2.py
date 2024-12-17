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

    # Xác định các thông số tối đa
    max_width = max(x[0] for x in bins)
    max_height = max(x[1] for x in bins)
    max_pack_width = max(x[0] for x in packs)
    max_pack_height = max(x[1] for x in packs)
    
    # Tạo mô hình
    model = cp_model.CpModel()

    # Các biến
    X = {}
    R = []
    for i in range(n_packs):
        # R[i] = 1 nếu gói đồ i bị xoay
        R.append(model.NewBoolVar(f'package_{i}_rotated'))
        for j in range(n_bins):
            # X[i, j] = 1 nếu gói đồ i được đóng gói vào thùng j
            X[i, j] = model.NewBoolVar(f'pack_{i}_in_bin_{j}')

    # Z[j] = 1 nếu thùng j được sử dụng
    Z = [model.NewBoolVar(f'bin_{j}_is_used') for j in range(n_bins)]

    # Kích thước và tọa độ của mỗi gói đồ
    width = []
    height = []
    x = []
    y = [] 
    for i in range(n_packs):
        width.append(model.NewIntVar(0, max_pack_width, f'width_{i}'))
        height.append(model.NewIntVar(0, max_pack_height, f'height_{i}'))
        x.append(model.NewIntVar(0, max_width, f'x_{i}'))
        y.append(model.NewIntVar(0, max_height, f'y_{i}'))

        # Nếu gói đồ bị xoay thì thay đổi chiều rộng và chiều cao
        model.Add(width[i] == packs[i][0]).OnlyEnforceIf(R[i].Not())
        model.Add(width[i] == packs[i][1]).OnlyEnforceIf(R[i])
        model.Add(height[i] == packs[i][1]).OnlyEnforceIf(R[i].Not())
        model.Add(height[i] == packs[i][0]).OnlyEnforceIf(R[i])

    # Ràng buộc
    # Mỗi gói đồ chỉ có thể được đặt vào một thùng
    for i in range(n_packs):
        model.Add(sum(X[i, j] for j in range(n_bins)) == 1)
        
    # Nếu gói đồ được đóng vào thùng, nó không được vượt quá kích thước của thùng
    for i in range(n_packs):
        for j in range(n_bins):
            model.Add(x[i] + width[i] <= bins[j][0]).OnlyEnforceIf(X[i, j])
            model.Add(y[i] + height[i] <= bins[j][1]).OnlyEnforceIf(X[i, j])
            model.Add(x[i] >= 0).OnlyEnforceIf(X[i, j])
            model.Add(y[i] >= 0).OnlyEnforceIf(X[i, j])            

    # Nếu hai gói đồ trong cùng một thùng thì chúng không được chồng lên nhau
    for i in range(n_packs-1):
        for k in range(i+1, n_packs):
            a1 = model.NewBoolVar(f'a1_{i}_{k}')        
            model.Add(x[i] + width[i] <= x[k]).OnlyEnforceIf(a1)
            model.Add(x[i] >= x[k] + width[k]).OnlyEnforceIf(a1.Not())
            
            a2 = model.NewBoolVar(f'a2_{i}_{k}')        
            model.Add(y[i] + height[i] <= y[k]).OnlyEnforceIf(a2)
            model.Add(y[i] >= y[k] + height[k]).OnlyEnforceIf(a2.Not())
            
            a3 = model.NewBoolVar(f'a3_{i}_{k}')        
            model.Add(x[k] + width[k] <= x[i]).OnlyEnforceIf(a3)
            model.Add(x[k] >= x[i] + width[i]).OnlyEnforceIf(a3.Not())
            
            a4 = model.NewBoolVar(f'a4_{i}_{k}')        
            model.Add(y[k] + height[k] <= y[i]).OnlyEnforceIf(a4)
            model.Add(y[k] >= y[i] + height[i]).OnlyEnforceIf(a4.Not())

            for j in range(n_bins):
                model.AddBoolOr([a1, a2, a3, a4]).OnlyEnforceIf(X[i, j], X[k, j])

    # Xác định thùng nào đã được sử dụng
    for j in range(n_bins):
        b1 = model.NewBoolVar(f'b_{j}')
        model.Add(sum(X[i, j] for i in range(n_packs)) == 0).OnlyEnforceIf(b1)
        model.Add(Z[j] == 0).OnlyEnforceIf(b1)
        model.Add(sum(X[i, j] for i in range(n_packs)) != 0).OnlyEnforceIf(b1.Not())
        model.Add(Z[j] == 1).OnlyEnforceIf(b1.Not())

    # Hàm mục tiêu: Giảm chi phí (tổng chi phí của các thùng đã sử dụng)
    cost = sum(Z[j] * bins[j][2] for j in range(n_bins))
    model.Minimize(cost)

    # Tạo solver và giải bài toán
    solver = cp_model.CpSolver()
    solver.parameters.max_time_in_seconds = time_limit
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
