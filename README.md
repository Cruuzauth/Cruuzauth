# CruuzAuth

<p align="center">
  <b>Official CruuzAuth SDKs and examples for C++, Java and Android.</b>
</p>

<p align="center">
  <a href="https://cruuzauth.com">Website</a> ·
  <a href="https://github.com/Cruuzauth/Cruuzauth">GitHub</a> ·
  <a href="https://cruuzauth.com/api_document.php">API Docs</a>
</p>

<p align="center">
  <img alt="C++ SDK" src="https://img.shields.io/badge/C++-SDK-blue">
  <img alt="Java SDK" src="https://img.shields.io/badge/Java-SDK-red">
  <img alt="Android Example" src="https://img.shields.io/badge/Android-Example-green">
  <img alt="Version" src="https://img.shields.io/badge/version-1.0.0-purple">
</p>

## What is CruuzAuth?

CruuzAuth is a licensing and authentication platform for modern software.

Use it to:

- Create and validate license keys.
- Register users with username, password and license.
- Login with username and password.
- Protect downloads using File IDs.
- Manage apps, users, subscriptions and managers from a web dashboard.

## Repository structure

```text
Cpp/
  Auth.hpp
  Auth.cpp

Java/
  CruuzAuth.java

Android/
  AndroidExample.java

examples/
  cpp/main.cpp
  java/Main.java
  android/LoginActivity.java

assets/
  screenshots/
```

## C++ quick start

```cpp
#include "Cpp/Auth.hpp"

std::string name = "YOUR_APP_NAME";
std::string ownerid = "YOUR_OWNER_ID";
std::string secret = "YOUR_SECRET";
std::string version = "1.0";
std::string url = "https://cruuzauth.com/api/";

CruuzAuth::api CruuzauthApp(name, ownerid, secret, version, url);

int main() {
    CruuzauthApp.init();

    if (!CruuzauthApp.response.success) {
        return 0;
    }

    CruuzauthApp.license("YOUR_KEY");

    if (!CruuzauthApp.response.success) {
        return 0;
    }

    // Access granted
}
```

## Java quick start

```java
CruuzAuth auth = new CruuzAuth(
    "YOUR_APP_NAME",
    "YOUR_OWNER_ID",
    "YOUR_SECRET",
    "1.0",
    "https://cruuzauth.com/api/"
);

CruuzAuth.Response init = auth.init();

if (!init.success) {
    System.out.println(init.message);
    return;
}

CruuzAuth.Response res = auth.license("YOUR_KEY", "YOUR_HWID");

if (res.success) {
    System.out.println("Access granted");
}
```

## Android quick start

Add this to `AndroidManifest.xml`:

```xml
<uses-permission android:name="android.permission.INTERNET" />
```

Use `ANDROID_ID` as HWID:

```java
String androidId = Settings.Secure.getString(
    getContentResolver(),
    Settings.Secure.ANDROID_ID
);
```

Then call:

```java
CruuzAuth.Response res = auth.license("YOUR_KEY", androidId);
```

## Downloads

The SDK supports protected downloads using File IDs.

Example C++:

```cpp
auto bytes = CruuzauthApp.download("YOUR_FILE_ID");
```

Example Java:

```java
byte[] bytes = auth.download("YOUR_FILE_ID");
```

## Releases

Recommended first release:

```text
v1.0.0
```

Release title:

```text
CruuzAuth SDK v1.0.0
```

Release description:

```text
Initial release with C++, Java and Android SDK examples.
```

Attach:

```text
CruuzAuth_GitHub_Repo_Ready.zip
```

## Security

Do not expose your private server credentials.

For client apps, keep your app credentials limited to the selected CruuzAuth application and rotate secrets if needed.

## License

MIT License.
