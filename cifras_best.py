
from itertools import combinations
from functools import lru_cache

def solve_optimized(numbers_tuple, target):
    numbers = list(numbers_tuple)
    n = len(numbers)
    
    if n == 1:
        return int(numbers[0]) if numbers[0] == target else None
    
    for i, j in combinations(range(n), 2):
        a, b = numbers[i], numbers[j]
        rest = tuple(numbers[k] for k in range(n) if k not in (i, j))
        
        candidates = [
            (a + b, f"({a}+{b})"),
            (a - b, f"({a}-{b})") if a>b else None,
            (a * b, f"({a}*{b})"),
            (a // b, f"({a}/{b})") if b > 1 else None,
            (b // a, f"({b}/{a})") if a > 1 else None,
        ]
                
        for val, expr in [x for x in candidates if x is not None]:
            sub = solve_optimized(rest + (val,), target)
            if sub is not None:
                if sub == int(val):
                    return expr
                return f"{expr} -> {sub}"
    return None

TARGET = 913
NUMBERS = (1, 2, 3, 4, 5, 6)
print(f'{solve_optimized(NUMBERS, TARGET)}')
