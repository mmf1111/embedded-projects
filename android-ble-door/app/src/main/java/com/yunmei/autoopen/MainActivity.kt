package com.yunmei.autoopen

import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.widget.Button
import android.widget.SeekBar
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat

class MainActivity : AppCompatActivity() {

    private lateinit var prefs: Prefs
    private lateinit var tvStatus: TextView
    private lateinit var tvLocation: TextView
    private lateinit var tvRssi: TextView
    private lateinit var seekRssi: SeekBar
    private lateinit var tvCooldown: TextView
    private lateinit var seekCooldown: SeekBar
    private lateinit var tvLeave: TextView
    private lateinit var seekLeave: SeekBar
    private lateinit var btnToggle: Button
    private lateinit var btnLogin: Button
    private lateinit var btnLogout: Button

    private var isServiceRunning = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        prefs = Prefs(this)

        tvStatus = findViewById(R.id.tvStatus)
        tvLocation = findViewById(R.id.tvLocation)
        tvRssi = findViewById(R.id.tvRssi)
        seekRssi = findViewById(R.id.seekRssi)
        tvCooldown = findViewById(R.id.tvCooldown)
        seekCooldown = findViewById(R.id.seekCooldown)
        tvLeave = findViewById(R.id.tvLeave)
        seekLeave = findViewById(R.id.seekLeave)
        btnToggle = findViewById(R.id.btnToggle)
        btnLogin = findViewById(R.id.btnLogin)
        btnLogout = findViewById(R.id.btnLogout)

        requestPermissions()

        // RSSI 阈值: -100 到 -50，默认 -62
        seekRssi.max = 50
        seekRssi.progress = prefs.rssiThreshold - (-100)
        updateRssiLabel()
        seekRssi.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(sb: SeekBar?, progress: Int, fromUser: Boolean) {
                prefs.rssiThreshold = -100 + progress
                updateRssiLabel()
            }
            override fun onStartTrackingTouch(sb: SeekBar?) {}
            override fun onStopTrackingTouch(sb: SeekBar?) {}
        })

        // 冷却时间: 0 到 120 秒
        seekCooldown.max = 120
        seekCooldown.progress = prefs.cooldownSeconds
        updateCooldownLabel()
        seekCooldown.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(sb: SeekBar?, progress: Int, fromUser: Boolean) {
                prefs.cooldownSeconds = progress
                updateCooldownLabel()
            }
            override fun onStartTrackingTouch(sb: SeekBar?) {}
            override fun onStopTrackingTouch(sb: SeekBar?) {}
        })

        // 离开判定时间: 5 到 60 秒，默认 15 秒
        seekLeave.max = 55
        seekLeave.progress = prefs.leaveTimeoutSeconds - 5
        updateLeaveLabel()
        seekLeave.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(sb: SeekBar?, progress: Int, fromUser: Boolean) {
                prefs.leaveTimeoutSeconds = 5 + progress
                updateLeaveLabel()
            }
            override fun onStartTrackingTouch(sb: SeekBar?) {}
            override fun onStopTrackingTouch(sb: SeekBar?) {}
        })

        btnToggle.setOnClickListener {
            if (isServiceRunning) stopService() else startService()
        }
        btnLogin.setOnClickListener {
            startActivity(Intent(this, LoginActivity::class.java))
        }
        btnLogout.setOnClickListener {
            stopService()
            prefs.isLoggedIn = false
            refreshUI()
            Toast.makeText(this, "已退出登录", Toast.LENGTH_SHORT).show()
        }
    }

    override fun onResume() {
        super.onResume()
        refreshUI()
    }

    private fun refreshUI() {
        if (prefs.isLoggedIn) {
            tvLocation.text = "门锁: ${prefs.locationText}"
            btnLogin.text = "重新登录"
            btnLogout.isEnabled = true
            btnToggle.isEnabled = true
        } else {
            tvLocation.text = "未登录，请先登录获取门锁信息"
            btnLogin.text = "登录"
            btnLogout.isEnabled = false
            btnToggle.isEnabled = false
        }
        updateToggleButton()
    }

    private fun updateRssiLabel() {
        tvRssi.text = "触发信号阈值: ${prefs.rssiThreshold} dBm（越大越近，建议-65~-55）"
    }

    private fun updateCooldownLabel() {
        tvCooldown.text = "开锁冷却: ${prefs.cooldownSeconds} 秒（防止反复触发）"
    }

    private fun updateLeaveLabel() {
        tvLeave.text = "离开判定时间: ${prefs.leaveTimeoutSeconds} 秒（出门后信号丢失多久才算离开，回来才开门）"
    }

    private fun updateToggleButton() {
        btnToggle.text = if (isServiceRunning) "停止无感开门" else "启动无感开门"
        tvStatus.text = if (isServiceRunning) "状态：运行中（后台监测中）" else "状态：已停止"
    }

    private fun startService() {
        if (!prefs.isLoggedIn) {
            Toast.makeText(this, "请先登录", Toast.LENGTH_SHORT).show()
            return
        }
        val intent = Intent(this, AutoOpenService::class.java).apply {
            action = AutoOpenService.ACTION_START
        }
        ContextCompat.startForegroundService(this, intent)
        isServiceRunning = true
        updateToggleButton()
        Toast.makeText(this, "已启动，靠近门锁将自动开门", Toast.LENGTH_SHORT).show()
    }

    private fun stopService() {
        val intent = Intent(this, AutoOpenService::class.java).apply {
            action = AutoOpenService.ACTION_STOP
        }
        startService(intent)
        isServiceRunning = false
        updateToggleButton()
    }

    private fun requestPermissions() {
        val perms = mutableListOf<String>()
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            perms.add(Manifest.permission.BLUETOOTH_SCAN)
            perms.add(Manifest.permission.BLUETOOTH_CONNECT)
        }
        perms.add(Manifest.permission.ACCESS_FINE_LOCATION)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            perms.add(Manifest.permission.POST_NOTIFICATIONS)
        }
        val needed = perms.filter {
            ContextCompat.checkSelfPermission(this, it) != PackageManager.PERMISSION_GRANTED
        }
        if (needed.isNotEmpty()) {
            ActivityCompat.requestPermissions(this, needed.toTypedArray(), 100)
        }
    }
}
