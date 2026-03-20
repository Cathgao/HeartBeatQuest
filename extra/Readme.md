The `HeartBeatBLEReader.dex` is compiled from `/AndroidProject/HeartBeatNative`, and extract from
`AndroidProject\HeartBeatNative\nativeblereader\build\outputs\apk\release\nativeblereader-release.apk`

1. build the AndroidProject/HeartBeatNative via android sdk
2. extract the apk file, copy the `classes.dex` and rename it to `HeartBeatBLEReader.dex` here.
3. in the project's root directory, run `python update_resources.py`.
4. recompile the program.
