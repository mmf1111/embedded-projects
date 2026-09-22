package com.yunmei.autoopen

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext

class LoginActivity : AppCompatActivity() {

    private lateinit var etPhone: EditText
    private lateinit var etPassword: EditText
    private lateinit var btnLogin: Button
    private lateinit var prefs: Prefs

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_login)
        prefs = Prefs(this)

        etPhone = findViewById(R.id.etPhone)
        etPassword = findViewById(R.id.etPassword)
        btnLogin = findViewById(R.id.btnLogin)

        btnLogin.setOnClickListener { doLogin() }
    }

    private fun doLogin() {
        val phone = etPhone.text.toString().trim()
        val password = etPassword.text.toString()
        if (phone.isEmpty() || password.isEmpty()) {
            Toast.makeText(this, "请输入手机号和密码", Toast.LENGTH_SHORT).show()
            return
        }
        btnLogin.isEnabled = false
        btnLogin.text = "登录中..."
        CoroutineScope(Dispatchers.IO).launch {
            try {
                val info = YunmeiApi.login(phone, password)
                prefs.lockMac = info.mac
                prefs.lockSecret = info.secret
                prefs.serviceUuid = info.serviceUuid
                prefs.charUuid = info.charUuid
                prefs.locationText = info.location
                withContext(Dispatchers.Main) {
                    Toast.makeText(
                        this@LoginActivity,
                        "登录成功: ${info.location}",
                        Toast.LENGTH_LONG
                    ).show()
                    finish()
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    Toast.makeText(
                        this@LoginActivity,
                        "登录失败: ${e.message}",
                        Toast.LENGTH_LONG
                    ).show()
                    btnLogin.isEnabled = true
                    btnLogin.text = "登录"
                }
            }
        }
    }
}
