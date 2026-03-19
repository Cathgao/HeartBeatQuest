#include <cstddef>
#include <jni.h>
#include <cstdio>
#include <cstring>
#include <mutex>
#include "main.hpp"
#include "scotland2/shared/modloader.h"
#include "BeatLeaderRecorder.hpp"
#include "multi_version_compat.hpp"

#include "HeartBeatBLEDex.inl"

#include "data_sources/Bluetooth.hpp"
#include "ModConfig.hpp"

HeartBeat::HeartBeatBleDataSource * bleDataSource;

JNIEnv * env;
jobject bleReader;
jmethodID bleReader_BleToggle;
jmethodID bleReader_IsDeviceSelected;
jmethodID bleReader_BleScanStart;
jmethodID bleReader_BleScanStop;
jmethodID bleReader_AutoConnectStart;
jmethodID bleReader_AutoConnectStop;
jmethodID bleReader_AutoConnectSetPattern;
jmethodID bleReader_OpenSystemLocationSetthings;

JNIEXPORT void JNICALL
Java_top_zxff_nativeblereader_BleReader_OnDeviceData
(JNIEnv *env, jobject thiz, jstring macAddr, jint heartRate, jlong energy){
    //this happens on a background thread, as google documented
    auto chars = env->GetStringUTFChars(macAddr, NULL);
    bleDataSource->OnDataCome(chars, heartRate, energy);
    env->ReleaseStringUTFChars(macAddr, chars);
}
JNIEXPORT jboolean JNICALL
Java_top_zxff_nativeblereader_BleReader_InformNativeDevice(JNIEnv *env, jobject thiz, jstring macAddr, jbyteArray name){
    //Add the ui or do something...
    //bleReader_BleStart call this function in java code
    auto macChar = env->GetStringUTFChars(macAddr, NULL);
    
    jbyte *name_buff = env->GetByteArrayElements(name, NULL);
    jsize name_len = env->GetArrayLength(name);
    std::string name_str((char*)name_buff, (size_t)name_len);

    jboolean ret = bleDataSource->InformNativeDevice(macChar, name_str);
    env->ReleaseStringUTFChars(macAddr, macChar);
    env->ReleaseByteArrayElements(name, name_buff, 0);
    return ret;
}
JNIEXPORT void JNICALL
Java_top_zxff_nativeblereader_BleReader_OnEnergyReset
(JNIEnv *env, jobject thiz){
    bleDataSource->OnEnergyReset();
}
JNIEXPORT void JNICALL
Java_top_zxff_nativeblereader_BleReader_OnAutoConnectStatusChanged
(JNIEnv *env, jobject thiz, jboolean autoConnecting){
    bleDataSource->OnAutoConnectStatusChanged(autoConnecting);
}
JNIEXPORT void JNICALL
Java_top_zxff_nativeblereader_BleReader_OnScanStatusChanged
(JNIEnv *env, jobject thiz, jboolean isScanning){
    bleDataSource->OnScanStatusChanged(isScanning);
}

void StartScanDevice(){
    //also check permissions
    env->CallVoidMethod(bleReader, bleReader_BleScanStart);
    if(env->ExceptionCheck()){
        getLogger().error("Exception occurred in JNI");
        env->ExceptionDescribe();
        return;
    }
}

bool ToggleDevice(std::string macAddr, jboolean selected){
    getLogger().info("Toggle device to:|{}|", macAddr);
    auto ret = env->CallBooleanMethod(bleReader, bleReader_BleToggle, env->NewStringUTF(macAddr.c_str()),selected);
    if(env->ExceptionCheck()){
        getLogger().error("Exception occurred in JNI");
        env->ExceptionDescribe();
        return false;
    }    
    return ret;
}

bool IsDeviceSelected(std::string macAddr){
    auto ret = env->CallBooleanMethod(bleReader, bleReader_IsDeviceSelected, env->NewStringUTF(macAddr.c_str()));
    if(env->ExceptionCheck()){
        getLogger().error("Exception occurred in JNI");
        env->ExceptionDescribe();
        return false;
    }
    return ret;
}
void StopScanDevice(){
    env->CallVoidMethod(bleReader, bleReader_BleScanStop);
    if(env->ExceptionCheck()){
        getLogger().error("Exception occurred in JNI");
        env->ExceptionDescribe();
        return;
    }
}
void AutoConnectStart(){
    env->CallVoidMethod(bleReader, bleReader_AutoConnectStart);
    if(env->ExceptionCheck()){
        getLogger().error("Exception occurred in JNI");
        env->ExceptionDescribe();
        return;
    }
}
void AutoConnectStop(){
    env->CallVoidMethod(bleReader, bleReader_AutoConnectStop);
    if(env->ExceptionCheck()){
        getLogger().error("Exception occurred in JNI");
        env->ExceptionDescribe();
        return;
    }
}
void AutoConnectSetPattern(const std::string & macAddr, const std::string& devName){
    env->CallVoidMethod(bleReader, bleReader_AutoConnectSetPattern, env->NewStringUTF(macAddr.c_str()), env->NewStringUTF(devName.c_str()));
    if(env->ExceptionCheck()){
        getLogger().error("Exception occurred in JNI");
        env->ExceptionDescribe();
        return;
    }
}
void OpenSystemLocationSetthing(){
    env->CallVoidMethod(bleReader, bleReader_OpenSystemLocationSetthings);
    if(env->ExceptionCheck()){
        getLogger().error("Exception occurred in JNI");
        env->ExceptionDescribe();
        return;
    }
}

void LoadJavaLibrary(){
    // use jni to load a java library to access bluetooth devices

    auto ret = modloader_jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
    
    if(env == nullptr){
        getLogger().error("JNI Env is nullptr");
        return;
    }
    env->ExceptionClear();

#define CHECK_EXCEPTION()  \
    do{if(env->ExceptionCheck()){\
        getLogger().error("JNI Exception at (line {})", __LINE__);\
        env->ExceptionDescribe();\
        env->ExceptionClear();\
        return;\
    }}while(0)


    /*
    Good, will load my java class

        buffobj = ByteBuffer.allocateDirect(xxx);
        buffobj.put(xxx);

        new dalvik.system.InMemoryDexClassLoader(buffobj, {env->FindClass("com/unity3d/player/UnityPlayer")}.getClassLoader())
            .loadClass("top.zxff.nativeblereader.BleReader")
            
    */
    //auto path_str = env->NewStringUTF(path.c_str());

    // auto unityPlayerClass = env->FindClass("com/unity3d/player/UnityPlayerGameActivity");
    // if(unityPlayerClass == nullptr){
    //     getLogger().error("UnityPlayer class not found");
    //     return;
    // }
    jobject ClassLoader;
    {
        jclass atClass =
            env->FindClass("android/app/ActivityThread");

        jmethodID currentApplication =
            env->GetStaticMethodID(
                atClass,
                "currentApplication",
                "()Landroid/app/Application;");

        jobject application =
            env->CallStaticObjectMethod(atClass, currentApplication);
        jclass contextClass =
            env->FindClass("android/content/Context");

        jmethodID getClassLoader =
            env->GetMethodID(
                contextClass,
                "getClassLoader",
                "()Ljava/lang/ClassLoader;");

        ClassLoader =
            env->CallObjectMethod(application, getClassLoader);

    }
    // auto ClassLoader = env->CallObjectMethod(unityPlayerClass, ClassClass_getClassLoader);
    CHECK_EXCEPTION();

    jobject buffobj;
    {        
        auto arr = env->NewByteArray(sizeof(ble_dex));
        env->SetByteArrayRegion(arr, 0, sizeof(ble_dex), (const jbyte*)&*ble_dex);
        
        CHECK_EXCEPTION();

        auto ByteBufferClass = env->FindClass("java/nio/ByteBuffer");
        auto ByteBufferClass_wrap = env->GetStaticMethodID(ByteBufferClass, "wrap", "([B)Ljava/nio/ByteBuffer;");
        buffobj = env->CallStaticObjectMethod(ByteBufferClass, ByteBufferClass_wrap, arr);
    }

    auto SomeClassLoaderClass = env->FindClass("dalvik/system/InMemoryDexClassLoader");
    if(SomeClassLoaderClass == nullptr){
        getLogger().error("can't find dalvik.system.InMemoryDexClassLoader");
        return;
    }
    auto SomeClassLoaderInit = env->GetMethodID(SomeClassLoaderClass, "<init>", "(Ljava/nio/ByteBuffer;Ljava/lang/ClassLoader;)V");
    if(SomeClassLoaderInit == nullptr){
        getLogger().debug("Enpty LoaderInit");
        return;
    }
    getLogger().debug("newObject");
    auto SomeClassLoader = env->NewObject(SomeClassLoaderClass, SomeClassLoaderInit, buffobj, ClassLoader);
    CHECK_EXCEPTION();
    getLogger().debug("will loadClass()");
    auto LoadClassMethod = env->GetMethodID(SomeClassLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    auto return_value_of_loadClass = env->CallObjectMethod(SomeClassLoader, LoadClassMethod, env->NewStringUTF("top.zxff.nativeblereader.BleReader"));
    
    CHECK_EXCEPTION();

    // We can't use env->FindClass("top.zxff.nativeblereader.BleReader") to find our class
    // Because we use a different class loader, which is a InMemoryDexClassLoader,
    //      and FindClass uses the classloader that ralated to the top of call stack
    auto bleReaderClass = static_cast<jclass>(return_value_of_loadClass);
    if(bleReaderClass == nullptr){
        getLogger().error("class not found");
        return;
    }
    //find class method
    bleReader_BleToggle = env->GetMethodID(bleReaderClass, "BleToggle", "(Ljava/lang/String;Z)Z");
    bleReader_IsDeviceSelected = env->GetMethodID(bleReaderClass, "IsDeviceSelected","(Ljava/lang/String;)Z");
    bleReader_BleScanStart = env->GetMethodID(bleReaderClass, "BleScanStart", "()V");
    bleReader_BleScanStop = env->GetMethodID(bleReaderClass, "BleScanStop", "()V");
    bleReader_AutoConnectStart = env->GetMethodID(bleReaderClass, "AutoConnectStart", "()V");
    bleReader_AutoConnectStop = env->GetMethodID(bleReaderClass, "AutoConnectStop", "()V");
    bleReader_AutoConnectSetPattern = env->GetMethodID(bleReaderClass, "AutoConnectSetPattern", "(Ljava/lang/String;Ljava/lang/String;)V");
    bleReader_OpenSystemLocationSetthings = env->GetMethodID(bleReaderClass, "OpenSystemLocationSetthing", "()V");


    static const JNINativeMethod methods[] = {
        {"OnDeviceData", "(Ljava/lang/String;IJ)V", reinterpret_cast<void*>(Java_top_zxff_nativeblereader_BleReader_OnDeviceData)},
        {"InformNativeDevice", "(Ljava/lang/String;[B)Z", reinterpret_cast<void*>(Java_top_zxff_nativeblereader_BleReader_InformNativeDevice)},
        {"OnEnergyReset","()V",reinterpret_cast<void*>(Java_top_zxff_nativeblereader_BleReader_OnEnergyReset)},
        {"OnAutoConnectStatusChanged","(Z)V",reinterpret_cast<void*>(Java_top_zxff_nativeblereader_BleReader_OnAutoConnectStatusChanged)},
        {"OnScanStatusChanged","(Z)V",reinterpret_cast<void*>(Java_top_zxff_nativeblereader_BleReader_OnScanStatusChanged)},
    };
    int rc = env->RegisterNatives(bleReaderClass, methods, sizeof(methods)/sizeof(JNINativeMethod));
    if (rc != JNI_OK){
        getLogger().error("Failed to register native methods");
        return;
    }


    auto bleReaderCtor = env->GetMethodID(bleReaderClass, "<init>", "()V");
    bleReader = env->NewGlobalRef(env->NewObject(bleReaderClass, bleReaderCtor));

    CHECK_EXCEPTION();

    getLogger().info("Java module loaded {} {} {} {} {} {} {}", 
        (void *)bleReader_BleToggle,
        (void *)bleReader_IsDeviceSelected,
        (void *)bleReader_BleScanStart,
        (void *)bleReader_BleScanStop,
        (void *)bleReader_AutoConnectStart,
        (void *)bleReader_AutoConnectStop,
        (void *)bleReader_AutoConnectSetPattern
    );
}


HeartBeat::HeartBeatBleDataSource::HeartBeatBleDataSource():HeartBeat::DataSource(HeartBeat::DataSourceType::DS_BLE){
    bleDataSource = this;
    
    LoadJavaLibrary();
    StartAutoScan();
}

void HeartBeat::HeartBeatBleDataSource::SetSelectedBleMac(const std::string mac){ 
    ToggleDevice(this->selected_mac, false);
    {
        std::lock_guard<std::mutex> g(this->selected_mac_lock);
        this->selected_mac = mac;
    }
    getModConfig().SelectedBleMac.SetValue(mac, true);

    ToggleDevice(this->selected_mac, true);

    auto it = avaliable_devices.find(this->selected_mac);
    std::lock_guard<std::mutex> g(Recorder::heartDeviceNameLock);
    if(it != avaliable_devices.end()){
        Recorder::heartDeviceName = it->second.name;
    }else{
        Recorder::heartDeviceName = HEART_DEV_NAME_UNK;
    }
}

void HeartBeat::HeartBeatBleDataSource::StartScan(){
    avaliable_devices.clear();
    StartScanDevice();
}
void HeartBeat::HeartBeatBleDataSource::StopScan(){
    StopScanDevice();
}
void HeartBeat::HeartBeatBleDataSource::StartAutoScan(){
    AutoConnectStart();
}
void HeartBeat::HeartBeatBleDataSource::SetAutoConnectPattern(const std::string& macAddr, const std::string& devName){
    AutoConnectSetPattern(macAddr, devName);
}
void HeartBeat::HeartBeatBleDataSource::OpenSystemLocationSetthings(){
    OpenSystemLocationSetthing();
}
bool HeartBeat::HeartBeatBleDataSource::GetData(int& heartbeat){
    heartbeat = this->heartbeat;
    if(has_new_data){
        has_new_data = false;
        return true;
    }
    return false;
}

long long HeartBeat::HeartBeatBleDataSource::GetEnergy(){
    return this->energy.load() + this->persistent_energy.load();
}

bool HeartBeat::HeartBeatBleDataSource::InformNativeDevice(const std::string& macAddr, const std::string& name){
    if(avaliable_devices.find(macAddr) == avaliable_devices.end()){
        avaliable_devices.insert({macAddr, {
            .name = name,
            .mac = macAddr,
            .last_data = 0,
            .last_data_time = 0
        }});
    }

    bool ret;
    {
        std::lock_guard<std::mutex> g(this->selected_mac_lock);

        ret = false;
        if(selected_mac == "" && getModConfig().SelectedBleMac.GetValue() == macAddr){
            selected_mac = macAddr;
            ret = true;
        }else if(selected_mac == macAddr){
            ret = true;
        }
        
    }
    if(ret){
        std::lock_guard<std::mutex> g(Recorder::heartDeviceNameLock);
        Recorder::heartDeviceName = name;
        return true;
    }
    return false;
}
void HeartBeat::HeartBeatBleDataSource::OnDataCome(const std::string& macAddr, int heartRate, long energy){
    this->heartbeat = heartRate;
    this->has_new_data = true;
    this->energy.store(energy); //energy is not work, idk how to read the data from java code. just forget it.
}
void HeartBeat::HeartBeatBleDataSource::OnEnergyReset(){
    this->persistent_energy.fetch_add(this->energy.load());
    this->energy.store(0);
}
void HeartBeat::HeartBeatBleDataSource::OnAutoConnectStatusChanged(bool autoConnecting){
    this->is_auto_connecting = autoConnecting;
}
void HeartBeat::HeartBeatBleDataSource::OnScanStatusChanged(bool isScanning){
    this->is_scanning = isScanning;
}
