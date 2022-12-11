def solve(n, input):
    for i in range(n, len(input) + 1):
        if len(set(input[i - n:i])) == n:
            return i
    return None


input = open("input").read()

print(solve(14, input))
print(solve(4, input))
