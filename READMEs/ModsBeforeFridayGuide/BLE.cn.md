# MBF 蓝牙权限补丁指南

如果想将蓝牙心率设备直接连接到游戏，请按照本指南操作。

MBF 有两种打蓝牙权限补丁的方式：第一种是在补丁游戏前设置权限，第二种是在补丁完成后重新补丁权限。

> [!NOTE]
> **配对 vs 权限补丁**  
> 如果仅开启蓝牙权限，模组应该能用——但只支持**已配对**的蓝牙设备。  
> 如果按本指南操作，模组无需配对即可连接心率设备。**这是推荐的设置。**  
> **请将心率设备从 Quest 系统的蓝牙配对列表中取消配对。无需配对。**

如果是全新安装游戏，阅读下一节开始。如果游戏已打过补丁，请跳转到[重新补丁已打的游戏](#重新补丁游戏)。

## 全新安装游戏的设置

在用 MBF 打补丁之前，先开启蓝牙权限开关。MBF 会自动打上蓝牙补丁。

![alt text](image.png)

开启蓝牙权限，然后点击 `Advanced Options` 按钮。

![alt text](image-2.png)

请继续阅读下一节。

### 编辑 XML

点击 `Edit XML`。

![alt text](image-3.png)

点击 `Download Current XML`。

![alt text](image-4.png)

右键点击下载的文件，选择`编辑`。

将以下内容复制到 XML 文件中（见截图）：

```xml
<uses-permission android:name="android.permission.BLUETOOTH_SCAN" android:usesPermissionFlags="65536" />
```

![alt text](image-5.png)

保存 XML 文件，然后点击 `Upload XML`，选择刚才编辑的文件。

![alt text](image-6.png)

然后点击 `Confirm Permission`。

![alt text](image-7.png)

最后，像正常 MBF 流程一样点击 `Mod The App`。

## 重新补丁游戏

可以在这里设置权限：

![alt text](image-8.png)

然后点击 `Edit XML`。流程与上面相同，请参考[编辑 XML](#编辑-xml) 章节。

完成 XML 补丁后，点击 `Repatch Game` 按钮。

![alt text](image-9.png)
