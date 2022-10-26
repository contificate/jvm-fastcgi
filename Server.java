
import java.util.Map;

public class Server {
  public static String go(Map<String, String> env) {
    return "<strong>Hello from Java:</strong> " + env.toString();
  } 
}
