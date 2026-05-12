import android.provider.Settings;
import android.widget.Toast;

// Ejemplo para usar dentro de una Activity.
// Agrega en AndroidManifest.xml:
// <uses-permission android:name="android.permission.INTERNET" />

public void loginWithCruuzAuth(String key) {
    String androidId = Settings.Secure.getString(
            getContentResolver(),
            Settings.Secure.ANDROID_ID
    );

    CruuzAuth auth = new CruuzAuth(
            "APP_NAME",
            "OWNER_ID",
            "SECRET",
            "1.0",
            "https://cruuzauth.com/api/"
    );

    new Thread(() -> {
        CruuzAuth.Response init = auth.init();

        if (!init.success) {
            runOnUiThread(() ->
                    Toast.makeText(this, init.message, Toast.LENGTH_LONG).show()
            );
            return;
        }

        CruuzAuth.Response res = auth.license(key, androidId);

        runOnUiThread(() -> {
            if (res.success) {
                Toast.makeText(this, "Access granted", Toast.LENGTH_LONG).show();
                // abrir tu panel / activity principal aquí
            } else {
                Toast.makeText(this, res.message, Toast.LENGTH_LONG).show();
            }
        });
    }).start();
}
