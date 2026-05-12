public class Main {
    public static void main(String[] args) throws Exception {
        CruuzAuth auth = new CruuzAuth(
                "YOUR_APP_NAME",
                "YOUR_OWNER_ID",
                "YOUR_SECRET",
                "1.0",
                "https://cruuzauth.com/api/"
        );

        CruuzAuth.Response init = auth.init();

        if (!init.success) {
            System.out.println("Init failed: " + init.message);
            return;
        }

        CruuzAuth.Response res = auth.license("YOUR_KEY", "YOUR_HWID");

        if (!res.success) {
            System.out.println("License failed: " + res.message);
            return;
        }

        System.out.println("Access granted.");
    }
}
