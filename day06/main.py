input = open("input").read()


def solve(input, window_size):
    for i in range(len(input) - window_size):
        window = input[i:i + window_size]
        if (len(window) == len(set(window))):
            return i + window_size
    return None


print(solve(input, 4))
print(solve(input, 14))

input = open("input").read()

def solve(input, window_size):
    return next((i + window_size for i in range(len(input) - window_size) if len(input[i:i + window_size]) == len(set(input[i:i + window_size]))), None)


print(solve(input, 4))
print(solve(input, 14))
