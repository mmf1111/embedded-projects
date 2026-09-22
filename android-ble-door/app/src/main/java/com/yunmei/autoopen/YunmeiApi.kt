package com.yunmei.autoopen

import org.json.JSONArray
import org.json.JSONObject
import java.io.BufferedReader
import java.io.InputStreamReader
import java.net.HttpURLConnection
import java.net.URL
import java.net.URLEncoder
import java.security.MessageDigest

/**
 * 云莓智能登录 & 获取门锁信息。
 * 流程：base登录 → 查学校(拿学校服务器地址+token) → 查门锁(拿蓝牙MAC/密钥/UUID)
 */
object YunmeiApi {

    data class LockInfo(
        val mac: String,
        val secret: String,
        val serviceUuid: String,
        val charUuid: String,
        val location: String
    )

    private fun md5(input: String): String {
        val md = MessageDigest.getInstance("MD5")
        return md.digest(input.toByteArray()).joinToString("") { "%02x".format(it) }
    }

    private fun postRaw(
        urlStr: String,
        params: Map<String, String>,
        token: String? = null,
        userId: String? = null
    ): String {
        val body = params.entries.joinToString("&") {
            "${URLEncoder.encode(it.key, "UTF-8")}=${URLEncoder.encode(it.value, "UTF-8")}"
        }
        val conn = URL(urlStr).openConnection() as HttpURLConnection
        conn.requestMethod = "POST"
        conn.doOutput = true
        conn.connectTimeout = 15000
        conn.readTimeout = 15000
        conn.setRequestProperty("x-requested-with", "XMLHttpRequest")
        conn.setRequestProperty("Content-Type", "application/x-www-form-urlencoded")
        token?.let { conn.setRequestProperty("token_data", it) }
        userId?.let {
            conn.setRequestProperty("token_userId", it)
            conn.setRequestProperty("tokenUserId", it)
        }
        conn.outputStream.use { it.write(body.toByteArray()) }
        val stream = if (conn.responseCode in 200..299) conn.inputStream else conn.errorStream
        val text = BufferedReader(InputStreamReader(stream)).readText()
        conn.disconnect()
        return text
    }

    fun login(phone: String, password: String): LockInfo {
        val baseUrl = "https://base.yunmeitech.com"
        val hashedPwd = md5(password)

        // 第一步：base 登录
        val loginJson = JSONObject(
            postRaw("$baseUrl/login", mapOf("userName" to phone, "userPwd" to hashedPwd))
        )
        if (!loginJson.optBoolean("success")) {
            throw Exception(loginJson.optString("msg", "登录失败"))
        }
        val userData = loginJson.getJSONObject("o")
        val token = userData.getString("token")
        val userId = userData.getString("userId")

        // 第二步：查学校信息（返回 JSON 数组）
        val schoolText = postRaw(
            "$baseUrl/userschool/getbyuserid",
            mapOf("userId" to userId), token, userId
        )
        val schoolArr = JSONArray(schoolText)
        if (schoolArr.length() == 0) throw Exception("学校信息获取失败")
        val schoolData = schoolArr.getJSONObject(0)
        val schoolObj = schoolData.getJSONObject("school")
        val schoolServerUrl = schoolObj.getString("serverUrl").trimEnd('/')
        val schoolToken = schoolData.getString("token")
        val schoolNo = schoolData.getString("schoolNo")

        // 第三步：查门锁信息（返回 JSON 数组）
        val lockText = postRaw(
            "$schoolServerUrl/dormuser/getuserlock",
            mapOf("schoolNo" to schoolNo), schoolToken, userId
        )
        val lockArr = JSONArray(lockText)
        if (lockArr.length() == 0) throw Exception("门锁信息获取失败（可能假期无权限）")
        val lockData = lockArr.getJSONObject(0)

        val buildName = lockData.optString("buildName", "")
        val dormName = lockData.optString("dormName", "")
        return LockInfo(
            mac = lockData.getString("lockNo"),
            secret = lockData.getString("lockSecret"),
            serviceUuid = lockData.getString("lockServiceUuid"),
            charUuid = lockData.getString("lockCharacterUuid"),
            location = "$buildName $dormName"
        )
    }
}
