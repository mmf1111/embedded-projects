package com.yunmei.autoopen

import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.app.Service
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothGatt
import android.bluetooth.BluetoothGattCallback
import android.bluetooth.BluetoothGattCharacteristic
import android.bluetooth.BluetoothManager
import android.bluetooth.le.BluetoothLeScanner
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanFilter
import android.bluetooth.le.ScanResult
import android.bluetooth.le.ScanSettings
import android.content.Context
import android.content.Intent
import android.os.Build
import android.os.Handler
import android.os.IBinder
import android.os.Looper
import android.os.PowerManager
import android.util.Log
import androidx.core.app.NotificationCompat
import java.util.UUID

/**
 * v1.2 前台服务：后台持续扫描门锁蓝牙信号。
 * 离开判定：信号丢失超过 leaveTimeoutSeconds 后标记为"已离开"，
 * 回来时信号达到 rssiThreshold 自动连接并发开锁指令。
 */
class AutoOpenService : Service() {

    companion object {
        private const val CHANNEL_ID = "yunmei_autoopen"
        private const val NOTIF_ID = 1001
        private const val TAG = "AutoOpenService"
        const val ACTION_START = "com.yunmei.autoopen.START"
        const val ACTION_STOP = "com.yunmei.autoopen.STOP"
    }

    private lateinit var prefs: Prefs
    private var bluetoothAdapter: BluetoothAdapter? = null
    private var bleScanner: BluetoothLeScanner? = null
    private var gatt: BluetoothGatt? = null
    private var wakeLock: PowerManager.WakeLock? = null
    private val handler = Handler(Looper.getMainLooper())

    private var lastUnlockTime = 0L
    private var isUnlocking = false
    private var hasLeft = true   // 是否已离开（信号丢失超时后为true，回来时才开门）
    private var firstScan = true // 首次扫描时若已在近处则不触发
    private var lastSeenTime = 0L // 最后一次扫描到门锁的时间

    // 定时检查：信号丢失超过设定秒数后，标记为"已离开"
    private val leaveCheckRunnable = object : Runnable {
        override fun run() {
            if (lastSeenTime > 0 &&
                System.currentTimeMillis() - lastSeenTime > prefs.leaveTimeoutSeconds * 1000L
            ) {
                hasLeft = true
            }
            handler.postDelayed(this, 3000)
        }
    }

    private val scanCallback = object : ScanCallback() {
        override fun onScanResult(callbackType: Int, result: ScanResult) {
            val rssi = result.rssi
            val triggerThreshold = prefs.rssiThreshold
            lastSeenTime = System.currentTimeMillis()

            // 首次扫描：若启动时已经在近处，直接标记为"在室内"，避免启动即开门
            if (firstScan) {
                firstScan = false
                if (rssi >= triggerThreshold) {
                    hasLeft = false
                }
            }

            // 只有"已离开"且信号达到触发阈值时才开门
            if (!isUnlocking && hasLeft && rssi >= triggerThreshold) {
                val now = System.currentTimeMillis()
                if (now - lastUnlockTime >= prefs.cooldownSeconds * 1000L) {
                    hasLeft = false
                    triggerUnlock(result.device)
                }
            }

            val state = if (hasLeft) "已离开" else "在室内"
            updateNotification("监测中 | 信号:${rssi}dBm | ${state}")
        }

        override fun onScanFailed(errorCode: Int) {
            Log.e(TAG, "Scan failed: $errorCode")
            updateNotification("扫描失败: $errorCode")
        }
    }

    private val gattCallback = object : BluetoothGattCallback() {
        override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
            if (newState == BluetoothGatt.STATE_CONNECTED) {
                Log.d(TAG, "Connected, discovering services")
                gatt.discoverServices()
            } else if (newState == BluetoothGatt.STATE_DISCONNECTED) {
                Log.d(TAG, "Disconnected")
                this@AutoOpenService.gatt = null
                isUnlocking = false
                handler.postDelayed({ startScan() }, 2000)
            }
        }

        override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
            if (status != BluetoothGatt.GATT_SUCCESS) {
                Log.e(TAG, "Service discovery failed: $status")
                gatt.disconnect()
                return
            }
            val service = gatt.getService(UUID.fromString(prefs.serviceUuid))
            if (service == null) {
                Log.e(TAG, "Service not found: ${prefs.serviceUuid}")
                gatt.disconnect()
                return
            }
            val characteristic = service.getCharacteristic(UUID.fromString(prefs.charUuid))
            if (characteristic == null) {
                Log.e(TAG, "Characteristic not found: ${prefs.charUuid}")
                gatt.disconnect()
                return
            }
            val packet = PacketGenerator.generate(prefs.lockSecret)
            characteristic.value = packet
            val written = gatt.writeCharacteristic(characteristic)
            Log.d(TAG, "Write submitted: $written, packet len=${packet.size}")
            if (written) {
                lastUnlockTime = System.currentTimeMillis()
                updateNotification("已发送开门指令 ✓")
            }
            // 1.5秒后断开，让扫描恢复
            handler.postDelayed({ gatt.disconnect() }, 1500)
        }

        override fun onCharacteristicWrite(
            gatt: BluetoothGatt,
            characteristic: BluetoothGattCharacteristic,
            status: Int
        ) {
            Log.d(TAG, "onCharacteristicWrite status: $status")
        }
    }

    override fun onCreate() {
        super.onCreate()
        prefs = Prefs(this)
        val pm = getSystemService(Context.POWER_SERVICE) as PowerManager
        wakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "YunmeiAutoOpen::WakeLock")
        wakeLock?.acquire()

        val bluetoothManager = getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        bluetoothAdapter = bluetoothManager.adapter
        createNotificationChannel()
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        if (intent?.action == ACTION_STOP) {
            stopSelf()
            return START_NOT_STICKY
        }
        startForeground(NOTIF_ID, buildNotification("正在启动..."))
        startScan()
        return START_STICKY
    }

    private fun startScan() {
        if (isUnlocking) return
        firstScan = true  // 每次重新扫描时重置首次扫描标记
        handler.removeCallbacks(leaveCheckRunnable)
        handler.post(leaveCheckRunnable)  // 启动信号丢失检测
        val adapter = bluetoothAdapter
        if (adapter == null || !adapter.isEnabled) {
            updateNotification("蓝牙未开启，请开启蓝牙")
            return
        }
        bleScanner = adapter.bluetoothLeScanner
        val filter = ScanFilter.Builder().setDeviceAddress(prefs.lockMac).build()
        val settings = ScanSettings.Builder()
            .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
            .setReportDelay(0)
            .build()
        try {
            bleScanner?.startScan(listOf(filter), settings, scanCallback)
            updateNotification("监测中 | 等待靠近门锁 (${prefs.lockMac})")
        } catch (e: SecurityException) {
            updateNotification("缺少蓝牙权限，请在设置中授予")
        } catch (e: Exception) {
            Log.e(TAG, "startScan error", e)
        }
    }

    private fun stopScan() {
        try {
            bleScanner?.stopScan(scanCallback)
        } catch (_: Exception) {
        }
    }

    private fun triggerUnlock(device: BluetoothDevice) {
        isUnlocking = true
        stopScan()
        updateNotification("正在连接门锁...")
        try {
            gatt = device.connectGatt(this, false, gattCallback)
        } catch (e: SecurityException) {
            updateNotification("缺少蓝牙权限")
            isUnlocking = false
            startScan()
        }
    }

    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(
                CHANNEL_ID, "云莓无感开门", NotificationManager.IMPORTANCE_LOW
            )
            channel.description = "后台监测门锁距离并自动开门"
            val nm = getSystemService(NotificationManager::class.java)
            nm.createNotificationChannel(channel)
        }
    }

    private fun buildNotification(text: String): Notification {
        val pendingIntent = PendingIntent.getActivity(
            this, 0, Intent(this, MainActivity::class.java),
            PendingIntent.FLAG_IMMUTABLE or PendingIntent.FLAG_UPDATE_CURRENT
        )
        return NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle("云莓无感开门")
            .setContentText(text)
            .setSmallIcon(android.R.drawable.ic_menu_mylocation)
            .setContentIntent(pendingIntent)
            .setOngoing(true)
            .build()
    }

    private fun updateNotification(text: String) {
        val nm = getSystemService(NotificationManager::class.java)
        nm.notify(NOTIF_ID, buildNotification(text))
    }

    override fun onDestroy() {
        super.onDestroy()
        handler.removeCallbacks(leaveCheckRunnable)
        stopScan()
        try {
            gatt?.disconnect()
        } catch (_: Exception) {
        }
        gatt = null
        wakeLock?.release()
    }

    override fun onBind(intent: Intent?): IBinder? = null
}
