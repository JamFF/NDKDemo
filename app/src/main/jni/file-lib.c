#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>

#define LOG_D(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG,"JamFF",FORMAT,__VA_ARGS__)
#define LOG_E(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"JamFF",FORMAT,__VA_ARGS__)

// 获取文件大小
long get_file_size(const char *path) {
    FILE *fp = fopen(path, "rb");
    fseek(fp, 0, SEEK_END);
    return ftell(fp);
}

JNIEXPORT void JNICALL
Java_com_jamff_ndk_FileUtils_slice(JNIEnv *env, jclass type, jstring path_, jstring path_slice_,
                                   jint count) {
    const char *path = (*env)->GetStringUTFChars(env, path_, 0);
    const char *path_slice = (*env)->GetStringUTFChars(env, path_slice_, 0);

    // 得到分割之后的子文件的路径列表
    char **paths = malloc(sizeof(char *) * count);
    int i;
    for (i = 0; i < count; i++) {
        paths[i] = malloc(sizeof(char) * 100);
        // 子文件元素赋值
        sprintf(paths[i], path_slice, (i + 1));
        LOG_D("path_%d path:%s", i + 1, paths[i]);
    }

    long fileSize = get_file_size(path);// 总文件大小
    long part = fileSize / count;// 子文件大小
    FILE *fpr = fopen(path, "rb");

    if (fileSize % count == 0) {
        // 整除
        for (i = 0; i < count; i++) {
            // 逐一写入不同的分割子文件中
            FILE *fpw = fopen(paths[i], "wb");
            int j;
            for (j = 0; j < part; j++) {
                // 边读边写
                fputc(fgetc(fpr), fpw);
            }
            fclose(fpw);
        }
    } else {
        //不整除
        for (i = 0; i < count; i++) {
            // 逐一写入不同的分割子文件中
            FILE *fpw = fopen(paths[i], "wb");
            int j;
            if (count == 2) {
                // 最后一个文件，剩余的全部写入
                for (j = 0; j < fileSize - part * 2; j++) {
                    fputc(fgetc(fpr), fpw);
                }
            } else {
                for (j = 0; j < part; j++) {
                    fputc(fgetc(fpr), fpw);
                }
            }
            fclose(fpw);
        }
    }

    //关闭被分割的文件
    fclose(fpr);

    //释放
    i = 0;
    for (; i < count; i++) {
        free(paths[i]);
    }
    free(paths);

    (*env)->ReleaseStringUTFChars(env, path_, path);
    (*env)->ReleaseStringUTFChars(env, path_slice_, path_slice);
}

JNIEXPORT void JNICALL
Java_com_jamff_ndk_FileUtils_merge(JNIEnv *env, jclass type, jstring path_slice_,
                                   jstring path_merge_, jint count) {
    const char *path_pattern = (*env)->GetStringUTFChars(env, path_slice_, 0);
    const char *path_merge = (*env)->GetStringUTFChars(env, path_merge_, 0);

    // 得到分割之后的子文件的路径列表
    char **paths = malloc(sizeof(char *) * count);
    int i;
    for (i = 0; i < count; i++) {
        paths[i] = malloc(sizeof(char) * 100);
        // 子文件元素赋值
        sprintf(paths[i], path_pattern, (i + 1));
        LOG_D("path_%d path:%s", i + 1, paths[i]);
    }

    FILE *fpw = fopen(path_merge, "wb");

    // 把所有的分割文件读取一遍，写入一个总的文件中
    for (i = 0; i < count; i++) {
        // 每个子文件的大小
        long fileSize = get_file_size(paths[i]);
        FILE *fpr = fopen(paths[i], "rb");
        int j = 0;
        for (; j < fileSize; j++) {
            fputc(fgetc(fpr), fpw);
        }
        fclose(fpr);
    }
    fclose(fpw);

    //释放
    i = 0;
    for (; i < count; i++) {
        free(paths[i]);
    }
    free(paths);

    (*env)->ReleaseStringUTFChars(env, path_slice_, path_pattern);
    (*env)->ReleaseStringUTFChars(env, path_merge_, path_merge);
}