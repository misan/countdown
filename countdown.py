TARGET = 913 
NUMBERS = [1, 2, 3, 4, 5, 6]

def solve_countdown(numbers, target, max_depth, current_depth=0, history=None, best_match=None):
    """
    Recursively finds the shortest path to reach the target number.
    Updates the best_match dictionary to keep the closest approximation.
    """
    if history is None:
        history = []
        
    # Evaluate all current numbers to find the closest match to the target
    for n in numbers:
        diff = abs(n - target)
        
        # Update if it's a closer match, or same distance but achieved in fewer steps
        if diff < best_match['diff'] or (diff == best_match['diff'] and len(history) < len(best_match['history'])):
            best_match['diff'] = diff
            best_match['val'] = n
            best_match['history'] = list(history)
            
        # Exact match found, stop exploring this branch
        if diff == 0:
            return True
            
    # Stop condition: reached max depth allowed for this iteration, or only 1 number left
    if current_depth >= max_depth or len(numbers) == 1:
        return False
    
    # Iterate over unique pairs. j starts at i+1 to avoid duplicate combinations like (0,1) and (1,0)
    for i in range(len(numbers)):
        for j in range(i + 1, len(numbers)):
            
            # Sort the pair so 'a' is always >= 'b'
            # This avoids calculating both 5+12 and 12+5
            num1, num2 = numbers[i], numbers[j]
            a, b = max(num1, num2), min(num1, num2)
            
            # Create a list of the unused numbers
            remaining = [numbers[k] for k in range(len(numbers)) if k not in (i, j)]
            
            operations = []
            
            # Addition (always valid)
            operations.append((a + b, f"{a} + {b} = {a+b}"))
            
            # Multiplication (skip if b == 1 to avoid useless steps like 5 * 1 = 5)
            if b > 1:
                operations.append((a * b, f"{a} * {b} = {a*b}"))
                
            # Subtraction (skip if a == b to avoid generating 0)
            if a > b:
                operations.append((a - b, f"{a} - {b} = {a-b}"))
                
            # Division (must have remainder 0, and skip if dividing by 1)
            if b > 1 and a % b == 0:
                operations.append((a // b, f"{a} / {b} = {a//b}"))
                
            # Try all valid operations for this pair
            for result, expr in operations:
                
                # Apply step (forward)
                history.append(expr)
                new_numbers = remaining + [result]
                
                # Recursive call
                if solve_countdown(new_numbers, target, max_depth, current_depth + 1, history, best_match):
                    return True
                    
                # Backtrack: remove step to explore the next branch cleanly
                history.pop()
                
    return False

# Global state to track the best approximation found across all branches
global_best = {
    'diff': float('inf'), 
    'val': None, 
    'history': []
}

# Iterative Deepening Depth-First Search (IDDFS)
# Try to solve in 1 step, then 2 steps, up to the maximum possible steps
max_possible_steps = len(NUMBERS)

for depth in range(1, max_possible_steps):
    if solve_countdown(NUMBERS, TARGET, max_depth=depth, best_match=global_best):
        print("Exact solution found!\n")
        break

# Output the results
print(f"Target: {TARGET}")
print(f"Best result: {global_best['val']} (Difference: {global_best['diff']})")
print("Steps taken:")
for step in global_best['history']:
    print(f"  {step}")
