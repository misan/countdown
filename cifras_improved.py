from fractions import Fraction
from itertools import combinations

TARGET = 100
NUMBERS = [Fraction(n) for n in [1, 2, 3, 4, 5, 6]]

def solve(numbers, target):
    n = len(numbers)
    if n == 1:
        return str(int(numbers[0])) if numbers[0] == target else None
    
    # Try all pairs (i, j) where i < j to avoid duplicates
    for i, j in combinations(range(n), 2):
        a, b = numbers[i], numbers[j]
        rest = [numbers[k] for k in range(n) if k not in (i, j)]
        
        candidates = [
            (a + b, f"({a}+{b})"),
            (a - b, f"({a}-{b})") if a>b else (b - a, f"({b}-{a})"),
            (a * b, f"({a}*{b})"),
            (a // b, f"({a}/{b})") if b != 0 else None,
            (b // a, f"({b}/{a})") if a != 0 else None,
       ]
        
        for val, expr in [x for x in candidates if x is not None]:
            sub = solve(rest + [val], target)
            if sub:
                return expr if sub == str(int(val)) else f"{expr} -> {sub}"
    return None

print(f'{solve(NUMBERS, TARGET)}')
