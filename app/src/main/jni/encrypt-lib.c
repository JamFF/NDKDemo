#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <android/log.h>
#include "com_jamff_ndk_Encrypt.h"//可以不引入头文件

#define LOG_D(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG,"JamFF",FORMAT,__VA_ARGS__)
#define LOG_E(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"JamFF",FORMAT,__VA_ARGS__)

char password[] = "JamFF";

pthread_t t_encrypt;
pthread_t t_decrypt;

// 为了encrypt传入多个参数，定义的结构体
typedef struct Path {
    const char *in_path;// 输入的地址
    const char *out_path;// 输出的地址
} Path, *PathP;

// 加密线程方法
void *encrypt(void *args) {

    PathP pathP = (PathP) args;

    // 打开文件
    FILE *normal_fp = fopen(pathP->in_path, "rb");
    FILE *crypt_fp = fopen(pathP->out_path, "wb");
    // 一次读取一个字符
    int ch;
    int i = 0; // 循环使用密码中的字母进行异或运算
    size_t pwd_len = strlen(password); //密码的长度
    while ((ch = fgetc(normal_fp)) != EOF) {
        // 写入（异或运算）
        fputc(ch ^ password[i % pwd_len], crypt_fp);
        i++;
    }
    // 关闭
    fclose(crypt_fp);
    fclose(normal_fp);
    return NULL;
}

// 解密线程方法
void *decrypt(void *args) {

    PathP pathP = (PathP) args;

    // 打开文件
    FILE *crypt_fp = fopen(pathP->in_path, "rb");
    FILE *decrypt_fp = fopen(pathP->out_path, "wb");
    // 一次读取一个字符
    int ch;
    int i = 0; //循环使用密码中的字母进行异或运算
    size_t pwd_len = strlen(password); //密码的长度
    while ((ch = fgetc(crypt_fp)) != EOF) {
        // 写入（异或运算）
        fputc(ch ^ password[i % pwd_len], decrypt_fp);
        i++;
    }
    //关闭
    fclose(decrypt_fp);
    fclose(crypt_fp);
    return NULL;
}

// 解密方法，不是线程中的方法，该方法不能传入pthread_create第三个参数
void *decrypt2(const char *crypt_path, const char *decrypt_path) {

    // 打开文件
    FILE *crypt_fp = fopen(crypt_path, "rb");
    FILE *decrypt_fp = fopen(decrypt_path, "wb");
    // 一次读取一个字符
    int ch;
    int i = 0; //循环使用密码中的字母进行异或运算
    size_t pwd_len = strlen(password); //密码的长度
    while ((ch = fgetc(crypt_fp)) != EOF) {
        // 写入（异或运算）
        fputc(ch ^ password[i % pwd_len], decrypt_fp);
        i++;
    }
    //关闭
    fclose(decrypt_fp);
    fclose(crypt_fp);
    return NULL;
}

JNIEXPORT jint JNICALL
Java_com_jamff_ndk_Encrypt_encrypt(JNIEnv *env, jobject instance, jstring normal_path_,
                                   jstring crypt_path_) {

    const char *normal_path = (*env)->GetStringUTFChars(env, normal_path_, 0);
    const char *crypt_path = (*env)->GetStringUTFChars(env, crypt_path_, 0);

    int result;

    Path path = {normal_path, crypt_path};

    // 参数1：要创建的线程的线程指针
    // 参数2：创建线程时的线程属性
    // 参数3：返回值是void类型的指针函数
    // 参数4：运行函数的参数
    result = pthread_create(&t_encrypt, NULL, encrypt, &path);

    // 不可行，传入虽然可以编译运行，但是相当于先执行该函数，再执行pthread_create函数开启线程
    // 会在pthread_join时崩溃
    // int r = pthread_create(&t_encrypt, NULL, encrypt(normal_path, crypt_path), &path);

    if (result == 0) {
        LOG_D("success to create pthread %s", "t_encrypt");
    } else {
        LOG_E("fail to create pthread %s", "t_encrypt");
    }

    if (pthread_join(t_encrypt, NULL) == 0) {
        LOG_D("success to recollect %s", "t_encrypt");
    } else {
        LOG_E("fail to recollect %s", "t_encrypt");
    }

    (*env)->ReleaseStringUTFChars(env, normal_path_, normal_path);
    (*env)->ReleaseStringUTFChars(env, crypt_path_, crypt_path);

    return result;
}

JNIEXPORT jint JNICALL
Java_com_jamff_ndk_Encrypt_decrypt(JNIEnv *env, jobject instance, jstring crypt_path_,
                                   jstring decrypt_path_) {
    const char *crypt_path = (*env)->GetStringUTFChars(env, crypt_path_, 0);
    const char *decrypt_path = (*env)->GetStringUTFChars(env, decrypt_path_, 0);

    int result;

    Path path = {crypt_path, decrypt_path};

    result = pthread_create(&t_encrypt, NULL, decrypt, &path);

    // 不可行，传入虽然可以编译运行，但是相当于先执行该函数，再执行pthread_create函数开启线程，会在执行pthread_join时崩溃
    // result = pthread_create(&t_encrypt, NULL, decrypt2(crypt_path, decrypt_path), &path);

    if (result == 0) {
        LOG_D("success to create pthread %s", "t_encrypt");
    } else {
        LOG_E("fail to create pthread %s", "t_encrypt");
    }

    if (pthread_join(t_encrypt, NULL) == 0) {
        LOG_D("success to recollect %s", "t_encrypt");
    } else {
        LOG_E("fail to recollect %s", "t_encrypt");
    }

    (*env)->ReleaseStringUTFChars(env, crypt_path_, crypt_path);
    (*env)->ReleaseStringUTFChars(env, decrypt_path_, decrypt_path);

    return result;
}