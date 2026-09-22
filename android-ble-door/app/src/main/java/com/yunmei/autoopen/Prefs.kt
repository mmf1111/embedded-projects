package com.yunmei.autoopen

import android.content.Context
import android.content.SharedPreferences

class Prefs(context: Context) {
    private val sp: SharedPreferences =
        context.getSharedPreferences("yunmei_autoopen", Context.MODE_PRIVATE)

    var isLoggedIn: Boolean
        get() = sp.getString("lockMac", "")?.isNotEmpty() == true
        set(v) { if (!v) sp.edit().clear().apply() }

    var lockMac: String
        get() = sp.getString("lockMac", "") ?: ""
        set(v) = sp.edit().putString("lockMac", v).apply()

    var lockSecret: String
        get() = sp.getString("lockSecret", "") ?: ""
        set(v) = sp.edit().putString("lockSecret", v).apply()

    var serviceUuid: String
        get() = sp.getString("serviceUuid", "") ?: ""
        set(v) = sp.edit().putString("serviceUuid", v).apply()

    var charUuid: String
        get() = sp.getString("charUuid", "") ?: ""
        set(v) = sp.edit().putString("charUuid", v).apply()

    var locationText: String
        get() = sp.getString("locationText", "") ?: ""
        set(v) = sp.edit().putString("locationText", v).apply()

    var rssiThreshold: Int
        get() = sp.getInt("rssiThreshold", -62)
        set(v) = sp.edit().putInt("rssiThreshold", v).apply()

    var cooldownSeconds: Int
        get() = sp.getInt("cooldownSeconds", 30)
        set(v) = sp.edit().putInt("cooldownSeconds", v).apply()

    var leaveTimeoutSeconds: Int
        get() = sp.getInt("leaveTimeoutSeconds", 15)
        set(v) = sp.edit().putInt("leaveTimeoutSeconds", v).apply()
}
