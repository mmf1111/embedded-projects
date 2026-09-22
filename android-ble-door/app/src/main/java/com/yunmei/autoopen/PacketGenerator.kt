package com.yunmei.autoopen

import kotlin.random.Random

/**
 * 生成云莓智能门锁的蓝牙开锁指令包。
 * 格式（与开源项目云莓闪开一致）：
 *   0xD0 | 长度(secret.length+14) | secret字节 | 0xA5 | 6位随机数(低位在前) | "ID01" | 0xA7
 */
object PacketGenerator {
    fun generate(secret: String): ByteArray {
        val list = mutableListOf<Byte>()
        list.add(0xD0.toByte())
        list.add((secret.length + 14).toByte())
        for (c in secret) {
            list.add(c.code.toByte())
        }
        list.add(0xA5.toByte())
        val pw = Random.nextInt(1000000)
        var tmp = pw
        for (i in 0 until 6) {
            list.add((tmp % 10).toByte())
            tmp /= 10
        }
        list.add(0x49) // 'I'
        list.add(0x44) // 'D'
        list.add(0x30) // '0'
        list.add(0x31) // '1'
        list.add(0xA7.toByte())
        return list.toByteArray()
    }
}
