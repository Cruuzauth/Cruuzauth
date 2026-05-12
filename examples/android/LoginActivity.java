import android.content.Intent;
import android.os.Bundle;
import android.provider.Settings;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;

public class LoginActivity extends AppCompatActivity {
    EditText keyInput;
    Button loginBtn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // setContentView(R.layout.activity_login);

        String androidId = Settings.Secure.getString(
                getContentResolver(),
                Settings.Secure.ANDROID_ID
        );

        CruuzAuth auth = new CruuzAuth(
                "YOUR_APP_NAME",
                "YOUR_OWNER_ID",
                "YOUR_SECRET",
                "1.0",
                "https://cruuzauth.com/api/"
        );

        loginBtn.setOnClickListener(v -> {
            String key = keyInput.getText().toString();

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
                        startActivity(new Intent(this, MainActivity.class));
                        finish();
                    } else {
                        Toast.makeText(this, res.message, Toast.LENGTH_LONG).show();
                    }
                });
            }).start();
        });
    }
}
