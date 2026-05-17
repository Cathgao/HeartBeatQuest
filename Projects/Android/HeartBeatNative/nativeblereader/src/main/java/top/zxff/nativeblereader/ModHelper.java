package top.zxff.nativeblereader;

import android.content.Context;

public class ModHelper {
    void OnModExit(){
        if(BleReader.instance != null){
            BleReader.instance.BleScanStop();
        }
        for (MDnsHelper instance : MDnsHelper.instances) {
            instance.Stop();
        }
    }

    static Context GetActivity(){
        try {
            return (Context)Class
                    .forName("com.unity3d.player.UnityPlayer")
                    .getField("currentActivity")
                    .get(null);
        } catch (NoSuchFieldException | IllegalAccessException | ClassNotFoundException e) {
            throw new RuntimeException(e);
        }
    }

}
