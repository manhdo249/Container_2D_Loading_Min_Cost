#PYTHON 
num_lists = int(input())
lists = [sorted(map(int, input().split())) for _ in range(num_lists)]

def process_leq(params):
    i, j, D = params[0] - 1, params[1] - 1, params[2]
    changed = False
    
    # Kiểm tra lists[i] không rỗng trước khi truy cập lists[i][0]
    while lists[j] and lists[i] and lists[i][0] > lists[j][0] + D:
        lists[j].pop(0)
        changed = True
    
    # Kiểm tra lists[i] và lists[j] không rỗng trước khi truy cập lists[i][-1] và lists[j][-1]
    while lists[i] and lists[j] and lists[i][-1] > lists[j][-1] + D:
        lists[i].pop()
        changed = True
        
    return changed

def process_eq(params):
    i, j, a, b = params[0] - 1, params[1] - 1, params[2], params[3]
    changed = False
    
    filtered_i = {x for x in lists[i] for y in lists[j] if x == a * y + b}
    filtered_j = {y for y in lists[j] for x in lists[i] if x == a * y + b}
    
    if len(lists[i]) != len(filtered_i):
        lists[i] = sorted(filtered_i)
        changed = True

    if len(lists[j]) != len(filtered_j):
        lists[j] = sorted(filtered_j)
        changed = True
        
    return changed

commands = []
while True:
    command = input().split()
    if command[0] == "#":
        break
    commands.append(command)
          
while True:
    any_change = False
    for cmd in commands:
        operation, *params = cmd
        params = list(map(int, params))
        if operation.startswith("LEQ"):
            any_change |= process_leq(params)
        elif operation.startswith("EQ"):
            any_change |= process_eq(params)
    
    if not any_change:
        break
            
for lst in lists:
    if not lst:
        print("FAIL")
        exit()
        
for lst in lists:
    print(len(lst), *lst)
