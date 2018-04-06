package com.jamff.ndk;

/**
 * 描述：文件拆分合并
 * 作者：JamFF
 * 创建时间：2018/4/6 21:27
 */
public class FileUtils {

    // 文件加密
    public static native void slice(String path, String path_slice, int count);

    // 文件解密
    public static native void merge(String path_slice, String path_merge, int count);

    static {
        System.loadLibrary("file-lib");
    }
}
