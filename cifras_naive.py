import itertools
from fractions import Fraction

TARGET = 3130
NUMBERS = [21, 12, 31, 41, 5, 6]

def find_expression(numbers, target):
    """Recursively tries to combine numbers to reach target."""
    if target in numbers:
        return str(target)
    # Base case: if we have one number, check if it matches
    if len(numbers) == 1:
        # Allow small tolerance for floating point, or exact match for fractions
        if numbers[0] == target:
            return str(target)
        return None
    
    # Try every pair combination
    for i in range(len(numbers)):
        for j in range(len(numbers)):
            if i == j:
                continue
            
            # Build the remaining list
            remaining = [numbers[k] for k in range(len(numbers)) if k not in (i,j)]
            
            a, b = numbers[i], numbers[j]
            
            # Try each operation (skip subtraction/division to avoid duplicates via order)
            operations = [
                (a + b, f"({a}+{b})"),
                (a * b, f"({a}*{b})"),
                (a - b, f"({a}-{b})") if a>b else (b - a, f"({b}-{a})"),
                (a // b, f"({a}/{b})") if b != 0 else None,
                (b // a, f"({b}/{a})") if a != 0 else None,
            ]
            
            for op in operations:
                if op is None:
                    continue
                result, expr = op
                if result is None:
                    continue
                
                result = Fraction(result)  # Use exact fractions
                new_numbers = remaining + [result]
                
                solution = find_expression(new_numbers, target)
                if solution:
                    return expr + " -> " + solution
    
    return None

print(f'{find_expression(NUMBERS, TARGET)}')