plugins {
    id("com.android.application")
}

android {
    namespace = "top.zxff.nativeblereader"
    //noinspection GradleDependency
    compileSdk = 32

    defaultConfig {
        applicationId = "top.zxff.nativeblereader"
        minSdk = 24
        //noinspection ExpiredTargetSdkVersion
        targetSdk = 32
        versionCode = 1
        versionName = "1.0"
        multiDexEnabled = false
    }

    buildTypes {
        release {
            isMinifyEnabled = true
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_18
        targetCompatibility = JavaVersion.VERSION_18
    }
}
tasks.withType<JavaCompile> {
    options.compilerArgs.add("-Xlint:deprecation")
}
dependencies {
    implementation("androidx.appcompat:appcompat:1.5.0")

}