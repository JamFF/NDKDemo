package com.jamff.ndk;

/**
 * 描述：文件加密解密
 * 作者：JamFF
 * 创建时间：2018/4/2 22:20
 */
public class Encrypt {

    // 文件加密
    public native int encrypt(String normal_path, String crypt_path);

    // 文件解密
    public native int decrypt(String crypt_path, String decrypt_path);

    static {
        System.loadLibrary("encrypt-lib");
    }
}
