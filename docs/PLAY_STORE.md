# Play Store listing & release guide

## Listing copy

| Field | Value |
|-------|-------|
| **Title** | Cube Solver |
| **Short description** | Offline Rubik’s Cube solver — free 3×3, premium 4×4 & 5×5. |
| **Category** | Puzzle |
| **Product ID (IAP)** | `premium_unlock` (non-consumable) |

### Full description (draft)

Cube Solver is an offline Rubik’s Cube solver for Android.

**Free**
- Full 3×3 scramble & solve (native CFOP / God’s-algorithm path)

**Premium (`premium_unlock`)**
- Unlock 4×4 and 5×5 reduction solves
- Bound report after nxn solves (stage lengths vs constructive upper bounds)

All solving runs on-device. No account required. Purchases are handled by Google Play Billing.

Exact God’s Number is proven only for 2×2 and 3×3 (n=2,3). For n≥4 the app ships a constructive reduction algorithm that always terminates; exact integer diameters remain open research.

## Content rating

Complete the IARC questionnaire in Play Console. Expected outcomes for this app:

- No user-generated content, no chat, no violence, no gambling
- In-app purchases (one-time premium unlock)
- Likely **Everyone** / low maturity

## Data safety

Declare in Play Console Data safety form:

- **No personal data collected by the app by default** (offline solver; no analytics SDK in this codebase)
- Purchase / entitlement data is processed by **Google Play Billing** (see Google’s policies)
- No account creation, no location, no contacts

Host a privacy policy URL pointing at [PRIVACY.md](PRIVACY.md) (GitHub Pages, site, or raw-friendly host).

## Screenshots & graphics

Suggested captures (phone):

1. Home with size chips (3 free / 4–5 locked) + short onboarding subtitle
2. 3×3 scramble → solution notation
3. Premium unlock / paywall sheet
4. 4×4 or 5×5 solve + `boundReport()` text (premium)
5. Adaptive icon on launcher

Provide a 512×512 high-res icon (export from adaptive foreground on brand green `#1B5E20`). Feature graphic 1024×500 with title “Cube Solver”.

## Manual Play Console steps

1. Create app → package `com.windsorroyal.rubikscubesolver`
2. Set store listing (title, short/full description, screenshots, icon)
3. Complete content rating + data safety + privacy policy URL
4. **Monetize → Products → In-app products** → create non-consumable `premium_unlock`
5. Create a closed testing track; upload an **AAB** from `bundleRelease`
6. Add license testers; verify purchase + restore on a device with Play Store
7. Promote to production when leftoverE / release QA looks good

## Build commands

```bash
# Debug APK (CI + local)
./gradlew assembleDebug

# Release APK (unsigned unless keystore props set)
./gradlew assembleRelease

# Play prefers AAB:
./gradlew bundleRelease
```

Optional signing (do **not** commit keystores or passwords). In `~/.gradle/gradle.properties` or CI secrets:

```properties
KEYSTORE_FILE=/absolute/path/to/upload.jks
KEYSTORE_PASSWORD=...
KEY_ALIAS=...
KEY_PASSWORD=...
```

`app/build.gradle.kts` reads these into `signingConfigs.release` when present.

## Artifacts

- APK: `app/build/outputs/apk/release/`
- AAB: `app/build/outputs/bundle/release/`
- Verify native libs: unzip the AAB/APK and confirm `lib/arm64-v8a/librubikssolver.so` (and armeabi-v7a) exist.

## Notes

- Free tier: size == 3 only (`PremiumStore`)
- Premium gate uses Play Billing Library + local SharedPreferences cache
- Debug bypass: `BuildConfig.DEBUG_PREMIUM` or debug preference unlock
