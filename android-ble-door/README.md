# Android BLE 无感开门

基于 Android BLE 的智能门禁 App：靠近门锁时通过蓝牙低功耗（BLE）自动开门，无需手动操作。

## 功能

- **BLE 扫描**：`BluetoothLeScanner` 持续扫描附近门锁设备
- **RSSI 阈值判断**：按信号强度判断是否靠近，达到阈值触发开门
- **GATT 写入**：连接后通过 `writeCharacteristic` 下发开门指令
- **前台服务驻留**：`AutoOpenService` 前台服务 + WakeLock，后台保持扫描
- **防重复触发**：开门后短时间内不重复触发
- 登录界面 + 主界面（实时显示设备 / RSSI / 状态）

## 技术栈

- Kotlin
- Android BLE（BluetoothGatt / BluetoothLeScanner）
- 前台服务 + 通知 + WakeLock
- 权限：蓝牙、定位、前台服务、通知等（见 `AndroidManifest.xml`）

## 目录结构

```
.
├── app/
│   ├── build.gradle.kts
│   └── src/main/
│       ├── AndroidManifest.xml
│       ├── java/com/yunmei/autoopen/
│       │   ├── MainActivity.kt       # 主界面，RSSI 显示与触发逻辑
│       │   ├── AutoOpenService.kt    # 前台服务：扫描 + 连接 + 开门
│       │   ├── LoginActivity.kt      # 登录
│       │   ├── PacketGenerator.kt    # 指令包构造
│       │   ├── Prefs.kt              # 配置 / 阈值持久化
│       │   └── YunmeiApi.kt          # 后端接口
│       └── res/                      # 布局与资源
├── build.gradle.kts
├── settings.gradle.kts
└── gradle/wrapper/                   # Gradle Wrapper
```

## 构建

用 Android Studio 打开工程，Sync 后构建 APK 即可（`local.properties` 未提交，需按本机 SDK 路径自行生成）。

## 说明

本项目为独立完成，实现 BLE 扫描、RSSI 阈值、GATT 写入、前台服务驻留与防重复触发。
