import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;

/**
 * CruuzAuth Java SDK
 * Compatible con la API actual de CruuzAuth.
 *
 * Funciones:
 * - init()
 * - license(key, hwid)
 * - login(username, password, hwid)
 * - register(username, password, key, hwid)
 * - download(fileId)
 */
public class CruuzAuth {
    private final String name;
    private final String ownerId;
    private final String secret;
    private final String version;
    private final String apiUrl;

    public Response response = new Response(false, "Not initialized", "");

    public CruuzAuth(String name, String ownerId, String secret, String version, String apiUrl) {
        this.name = name;
        this.ownerId = ownerId;
        this.secret = secret;
        this.version = version;
        this.apiUrl = apiUrl.endsWith("/") ? apiUrl : apiUrl + "/";
    }

    public Response init() {
        response = request("type=init"
                + "&name=" + enc(name)
                + "&ownerid=" + enc(ownerId)
                + "&secret=" + enc(secret)
                + "&ver=" + enc(version));
        return response;
    }

    public Response license(String key, String hwid) {
        response = request("type=license"
                + "&name=" + enc(name)
                + "&ownerid=" + enc(ownerId)
                + "&secret=" + enc(secret)
                + "&key=" + enc(key)
                + "&hwid=" + enc(hwid));
        return response;
    }

    public Response login(String username, String password, String hwid) {
        response = request("type=login"
                + "&name=" + enc(name)
                + "&ownerid=" + enc(ownerId)
                + "&secret=" + enc(secret)
                + "&username=" + enc(username)
                + "&password=" + enc(password)
                + "&hwid=" + enc(hwid));
        return response;
    }

    public Response register(String username, String password, String key, String hwid) {
        response = request("type=register"
                + "&name=" + enc(name)
                + "&ownerid=" + enc(ownerId)
                + "&secret=" + enc(secret)
                + "&username=" + enc(username)
                + "&password=" + enc(password)
                + "&key=" + enc(key)
                + "&hwid=" + enc(hwid));
        return response;
    }

    public byte[] download(String fileId) throws Exception {
        String data = "type=download"
                + "&name=" + enc(name)
                + "&ownerid=" + enc(ownerId)
                + "&secret=" + enc(secret)
                + "&fileid=" + enc(fileId);

        HttpURLConnection conn = openConnection(data);
        int code = conn.getResponseCode();

        InputStream in = (code >= 200 && code < 300) ? conn.getInputStream() : conn.getErrorStream();
        byte[] bytes = readAll(in);
        conn.disconnect();
        return bytes;
    }

    private Response request(String data) {
        try {
            HttpURLConnection conn = openConnection(data);
            int code = conn.getResponseCode();
            InputStream in = (code >= 200 && code < 300) ? conn.getInputStream() : conn.getErrorStream();
            String body = new String(readAll(in), StandardCharsets.UTF_8);
            conn.disconnect();

            boolean success = body.toLowerCase().contains("\"success\":true")
                    || body.toLowerCase().contains("\"success\": true");
            String message = extractJsonString(body, "message");
            if (message.isEmpty()) message = success ? "Success" : body;

            return new Response(success, message, body);
        } catch (Exception e) {
            return new Response(false, e.getMessage(), "");
        }
    }

    private HttpURLConnection openConnection(String data) throws Exception {
        URL url = new URL(apiUrl);
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setRequestMethod("POST");
        conn.setConnectTimeout(15000);
        conn.setReadTimeout(30000);
        conn.setDoOutput(true);
        conn.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
        conn.setRequestProperty("User-Agent", "CruuzAuth-Java-SDK");

        byte[] out = data.getBytes(StandardCharsets.UTF_8);
        conn.setRequestProperty("Content-Length", String.valueOf(out.length));

        try (OutputStream os = conn.getOutputStream()) {
            os.write(out);
        }

        return conn;
    }

    private static byte[] readAll(InputStream in) throws Exception {
        if (in == null) return new byte[0];
        ByteArrayOutputStream buffer = new ByteArrayOutputStream();
        byte[] data = new byte[4096];
        int n;
        while ((n = in.read(data, 0, data.length)) != -1) {
            buffer.write(data, 0, n);
        }
        return buffer.toByteArray();
    }

    private static String enc(String s) {
        try {
            return URLEncoder.encode(s == null ? "" : s, "UTF-8");
        } catch (Exception e) {
            return "";
        }
    }

    private static String extractJsonString(String json, String key) {
        if (json == null) return "";
        String pattern = "\"" + key + "\"";
        int p = json.indexOf(pattern);
        if (p < 0) return "";
        int colon = json.indexOf(':', p);
        if (colon < 0) return "";
        int first = json.indexOf('"', colon + 1);
        if (first < 0) return "";
        int second = json.indexOf('"', first + 1);
        if (second < 0) return "";
        return json.substring(first + 1, second).replace("\\/", "/").replace("\\\"", "\"");
    }

    public static class Response {
        public final boolean success;
        public final String message;
        public final String raw;

        public Response(boolean success, String message, String raw) {
            this.success = success;
            this.message = message;
            this.raw = raw;
        }
    }
}
