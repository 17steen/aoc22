import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.stream.IntStream;

class Program {
    
    public static void main(String[] args) throws IOException {
        
        var input = Files.readString(Path.of("input"));
        

        System.out.println(solve(4, input));
        System.out.println(solve(14, input));

    }

    public static int solve(int n, String input) {
        return IntStream.range(n, input.length() + 1)
                        .filter(i -> uniqueChars(input.substring(i - n, i)))
                        .findFirst()
                        .orElse(-1);
    }
    
    public static boolean uniqueChars(String str) {
        return str.chars()
                  .distinct()
                  .count() == str.length();
    }
    

}