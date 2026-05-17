package top.zxff.nativeblereader;

import android.content.Context;
import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;

import java.util.LinkedList;

public class MDnsHelper {

    public static LinkedList<MDnsHelper> instances = new LinkedList<>();

    NsdManager nsdManager;
    NsdManager.RegistrationListener registerService = null;

    public native boolean InformRegName(int managerId, String mdnsName);

    public MDnsHelper(){
        nsdManager = (NsdManager) ModHelper.GetActivity().getSystemService(Context.NSD_SERVICE);
        instances.add(this);
    }
    int id = 0;
    public void SetManagerId(int id){
        this.id = id;
    }
    public void SetMdnsName(String name, int port, String serviceType){
        if(registerService != null){
            nsdManager.unregisterService(registerService);
        }
        registerService = new NsdManager.RegistrationListener() {
            @Override
            public void onRegistrationFailed(NsdServiceInfo nsdServiceInfo, int i) {

            }

            @Override
            public void onUnregistrationFailed(NsdServiceInfo nsdServiceInfo, int i) {

            }

            @Override
            public void onServiceRegistered(NsdServiceInfo nsdServiceInfo) {
                InformRegName(id, nsdServiceInfo.getServiceName());
            }

            @Override
            public void onServiceUnregistered(NsdServiceInfo nsdServiceInfo) {

            }
        };

        NsdServiceInfo serviceInfo = new NsdServiceInfo();
        serviceInfo.setServiceName(name);
        serviceInfo.setPort(port);
        serviceInfo.setServiceType(serviceType);

        nsdManager.registerService(serviceInfo, NsdManager.PROTOCOL_DNS_SD, registerService);
    }
    public void Stop(){
        if(registerService != null){
            nsdManager.unregisterService(registerService);
            registerService = null;
        }
    }
}
