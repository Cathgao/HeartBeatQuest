package top.zxff.nativeblereader;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Handler;
import android.os.ParcelUuid;
import android.widget.Toast;

import androidx.annotation.Discouraged;
import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Set;
import java.util.UUID;

import dalvik.system.PathClassLoader;

public class BleReader {

    @Discouraged(message = "this method should be rewritten in C++ with JNI to load this java code.")
    public static void LoadJavaLibrary(String path) throws ClassNotFoundException {
        new PathClassLoader(path,
                Class.forName("com.unity3d.player.UnityPlayer").getClassLoader())
            .loadClass("top.zxff.nativeblereader.BleReader");
    }

    public native void InformNativeDevice(String macAddr, byte[] deviceName);
    public native void OnDeviceData(String macAddr, int heartRate, long energy);
    public native void OnEnergyReset();
    public boolean IsDeviceSelected(String macAddr){
        return BleDevices.containsKey(macAddr) && BleDevices.get(macAddr).selected;
    }


    Context context;
    public BleReader(){
        this.context = GetActivity();
        if(this.context == null){
            throw new RuntimeException("The context is nullptr");
        }
    }

    class DeviceStatus{
        boolean selected = false;

        BluetoothDevice dev;
        BluetoothGattCb cb;

        ////////////////////////////////////
        boolean serviceDiscovered = false;
        public DeviceStatus(BluetoothDevice dev){
            this.dev = dev;
        }
        @SuppressLint("MissingPermission")
        public boolean Toggle(boolean selected){
            if(this.selected == selected)
                return false;
            this.selected = selected;
            if(selected){
                //turn on
                cb = new BluetoothGattCb();
                cb.gatt = this.dev.connectGatt(context, true,cb);
            }else{
                //turn off
                cb.close();
                cb = null;
            }
            return true;
        }

        //As Google documented, the Callback happens in a background thread. Good!
        class BluetoothGattCb extends BluetoothGattCallback {
            final static String HEART_UUID = "00002a37-0000-1000-8000-00805f9b34fb";
            final static String CONTROL_POINT_UUID = "00002a39-0000-1000-8000-00805f9b34fb";
            BluetoothGatt gatt;
            @SuppressLint("MissingPermission")
            public void close(){
                serviceDiscovered = false;
                this.gatt.close();
                this.gatt = null;
            }
            /*
            Why we need this variable called useLatestHandleGatt:
                Some device will use the new api : onCharacteristicChanged(gatt, chara, values)
                However, this api is never called in quest 2 device.
                Quest 2 use onCharacteristicChanged(gatt, chara) instead.
                The old api is deprecated in API LEVEL 33, and I'm not sure if it will called in latest device.
                So the stupid variable here to prevent duplicate data.
             */
            boolean useLatestHandleGatt = false;
            @SuppressLint("MissingPermission")
            private void handleGatt(@NonNull BluetoothGatt gatt, @NonNull BluetoothGattCharacteristic characteristic){
                if (HEART_UUID.equals(characteristic.getUuid().toString())) {
                    int flag = characteristic.getProperties();

                    int format = -1;
                    if ((flag & 0x01) != 0) {
                        format = BluetoothGattCharacteristic.FORMAT_UINT16;
                    } else {
                        format = BluetoothGattCharacteristic.FORMAT_UINT8;
                    }
                    int heartRate = characteristic.getIntValue(format, 1);

                    long energy = 0;
                    if((flag & 0x8) != 0){
                        //energy
                        int offset = (flag & 1) + 2;
                        energy = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT16, offset);
                    }
                    OnDeviceData(dev.getAddress(), heartRate, energy);
                    if(energy > 10000){
                        BluetoothGattCharacteristic control_point_cb = characteristic.getService()
                                .getCharacteristic(UUID.fromString(CONTROL_POINT_UUID));
                        if(control_point_cb != null){
                            control_point_cb.setValue(new byte[]{1,1});//reset the energy
                            control_point_cb.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                            gatt.writeCharacteristic(control_point_cb);
                        }
                    }
                }
            }

            @Override
            public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
                super.onCharacteristicChanged(gatt, characteristic);
                if(useLatestHandleGatt)
                    return;
                handleGatt(gatt, characteristic);
            }


            @Override
            public void onCharacteristicChanged(@NonNull BluetoothGatt gatt, @NonNull BluetoothGattCharacteristic characteristic, @NonNull byte[] value) {
                super.onCharacteristicChanged(gatt, characteristic, value);
                useLatestHandleGatt = true;
                handleGatt(gatt, characteristic);
            }

            @SuppressLint("MissingPermission")
            @Override
            public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
                super.onConnectionStateChange(gatt, status, newState);
                if (newState == BluetoothProfile.STATE_CONNECTED) {
                    gatt.discoverServices();
                }
            }

            @Override
            public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
                super.onCharacteristicWrite(gatt, characteristic, status);
                if(CONTROL_POINT_UUID.equals(characteristic.getUuid().toString())){
                    if(status == 0){
                        OnEnergyReset();
                    }
                }
            }

            @SuppressLint("MissingPermission")
            @Override
            public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                super.onServicesDiscovered(gatt, status);
                if (gatt != this.gatt)
                    return;
                for (BluetoothGattService serv : gatt.getServices()) {
                    for (BluetoothGattCharacteristic ch : serv.getCharacteristics()) {
                        if (HEART_UUID.equals(ch.getUuid().toString())) {
                            gatt.setCharacteristicNotification(ch, true);
                            BluetoothGattDescriptor descriptor = ch.getDescriptor(UUID.fromString("00002902-0000-1000-8000-00805f9b34fb"));
                            descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
                            gatt.writeDescriptor(descriptor);
                            serviceDiscovered = true;
                        }
                        if(CONTROL_POINT_UUID.equals(ch.getUuid().toString())){
                            //for energy accumulator
                            ch.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                            ch.setValue(new byte[]{1,1});
                            gatt.writeCharacteristic(ch);
                        }
                    }
                }
            }
        }

    }

    /* mac -> DeviceStatus */
    HashMap<String, DeviceStatus> BleDevices = new HashMap<>();
    @SuppressLint("InlinedApi")
    private boolean testIfHavePermissions(boolean requirePermissions){
        LinkedList<String> permissions = new LinkedList<>();
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.S && ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED)
            permissions.add(Manifest.permission.BLUETOOTH_CONNECT);
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.S && ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED)
            permissions.add(Manifest.permission.BLUETOOTH_SCAN);
        if(ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED)
            permissions.add(Manifest.permission.ACCESS_COARSE_LOCATION);
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q &&ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED)
            permissions.add(Manifest.permission.ACCESS_FINE_LOCATION);
        if(Build.VERSION.SDK_INT <= Build.VERSION_CODES.R &&ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_ADMIN) != PackageManager.PERMISSION_GRANTED)
            permissions.add(Manifest.permission.BLUETOOTH_ADMIN);

        if(permissions.isEmpty()){
            return true;
        }
        if(requirePermissions){
            ActivityCompat.requestPermissions((Activity)context, permissions.toArray(new String[0]), 1);
        }
        return false;
    }



    private BluetoothLeScanner leScanner;

    private boolean isScanning = false;

    private Runnable autoConnectCanceler;
    static class AutoConnectPattern {
        public String name, mac;
        public AutoConnectPattern(String name, String mac){
            this.name = name;
            this.mac = mac;
        }
    };
    AutoConnectPattern autoConnectPattern;
    private Handler handler;

    private ScanCallback leScanCallback = new ScanCallback() {
        @SuppressLint("MissingPermission")
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            super.onScanResult(callbackType, result);
            if(!isScanning)
                return;
            BluetoothDevice device = result.getDevice();
            if(!BleDevices.containsKey(device.getAddress())){
                BleDevices.put(device.getAddress(),new DeviceStatus(device));
            }
            InformNativeDevice(device.getAddress(), device.getName().getBytes(StandardCharsets.UTF_8));
        }
    };

    @SuppressLint("MissingPermission")
    public void BleScanStart(){
        if(!testIfHavePermissions(true))
            return;

        if(leScanner == null)
            leScanner = BluetoothAdapter.getDefaultAdapter().getBluetoothLeScanner();

        Set<BluetoothDevice> deviceSet = BluetoothAdapter.getDefaultAdapter().getBondedDevices();
        for (BluetoothDevice bluetoothDevice : deviceSet) {
            if(!BleDevices.containsKey(bluetoothDevice.getAddress())){
                BleDevices.put(bluetoothDevice.getAddress(),
                        new DeviceStatus(bluetoothDevice));
            }
            InformNativeDevice(bluetoothDevice.getAddress(), bluetoothDevice.getName().getBytes(StandardCharsets.UTF_8));
        }


        LinkedList<ScanFilter> filters = new LinkedList<>();
        filters.add(new ScanFilter.Builder()
                .setServiceUuid(new ParcelUuid(UUID.fromString(DeviceStatus.BluetoothGattCb.HEART_UUID)))
                .build());
        ScanSettings settings = new ScanSettings.Builder()
                .build();
        isScanning = true;
        leScanner.startScan(filters, settings, leScanCallback);
    }

    @SuppressLint("MissingPermission")
    public void BleScanStop(){
        if(autoConnectCanceler!=null){
            handler.removeCallbacks(autoConnectCanceler);
            autoConnectCanceler=null;
        }
        if(leScanner != null) {
            leScanner.stopScan(leScanCallback);
            isScanning = false;
        }
    }

    @SuppressLint("MissingPermission")
    public void AutoConnectStart(){
        if(!testIfHavePermissions(false))
            return;
        if(autoConnectCanceler != null){
            handler.removeCallbacks(autoConnectCanceler);
        }
        autoConnectCanceler = ()->{
            autoConnectCanceler = null;
            if(leScanner != null){
                leScanner.stopScan(leScanCallback);
                isScanning = false;
            }
        };
        // we only search the device in 1 mins.
        handler.postDelayed(autoConnectCanceler,1000 * 60);
        BleScanStart();
    }
    public void AutoConnectSetPattern(String macAddress, String deviceName){
        autoConnectPattern = new AutoConnectPattern(deviceName, macAddress);
    }

    void AutoConnectStop(){
        if(autoConnectCanceler != null){
            handler.removeCallbacks(autoConnectCanceler);
            autoConnectCanceler = null;
        }
        if(isScanning)
            BleScanStop();
    }
    public boolean BleToggle(String macAddress, boolean selected){
        return BleDevices.containsKey(macAddress) && BleDevices.get(macAddress).Toggle(selected);
    }

    private static Context GetActivity(){
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
