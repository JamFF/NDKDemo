package com.jamff.ndk;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private static final String TAG = "JamFF";

    private Encrypt mEncrypt;

    private String SD_CARD_PATH;

    private Button bt_encrypt;
    private Button bt_slice;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mEncrypt = new Encrypt();
        SD_CARD_PATH = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separatorChar;
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
        bt_encrypt = findViewById(R.id.bt_encrypt);
        bt_encrypt.setOnClickListener(this);
        bt_slice = findViewById(R.id.bt_slice);
        bt_slice.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.bt_encrypt:
                encrypt(getString(R.string.encrypt).contentEquals(bt_encrypt.getText()));
                break;
            case R.id.bt_slice:
                slice(getString(R.string.slice).contentEquals(bt_slice.getText()));
        }
    }

    /**
     * 加密、解密文件
     *
     * @param encrypt true 加密 false 解密
     */
    private void encrypt(boolean encrypt) {
        if (encrypt) {
            bt_encrypt.setEnabled(false);
            bt_encrypt.setText(getString(R.string.encrypting));
            new Thread(new Runnable() {
                @Override
                public void run() {
                    Log.d(TAG, "start encrypt");
                    String normal_path = SD_CARD_PATH + "text.apk";
                    String crypt_path = SD_CARD_PATH + "text_encrypt.apk";
                    int result = mEncrypt.encrypt(normal_path, crypt_path);
                    Log.d(TAG, "encrypt result = " + result);
                    try {
                        Thread.sleep(10 * 1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            bt_encrypt.setText(getString(R.string.decrypt));
                            bt_encrypt.setEnabled(true);
                        }
                    });
                    Log.d(TAG, "end encrypt");
                }
            }).start();
        } else {
            bt_encrypt.setEnabled(false);
            bt_encrypt.setText(getString(R.string.decrypting));
            new Thread(new Runnable() {
                @Override
                public void run() {
                    Log.d(TAG, "start decrypt");
                    String crypt_path = SD_CARD_PATH + "text_encrypt.apk";
                    String decrypt_path = SD_CARD_PATH + "text_decrypt.apk";
                    int result = mEncrypt.decrypt(crypt_path, decrypt_path);
                    Log.d(TAG, "encrypt result = " + result);
                    try {
                        Thread.sleep(10 * 1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            bt_encrypt.setText(getString(R.string.decrypt_ok));
                            bt_encrypt.setEnabled(false);
                        }
                    });
                    Log.d(TAG, "done decrypt");
                }
            }).start();
        }
    }

    /**
     * 拆分、合并文件
     *
     * @param slice true 拆分 false 合并
     */
    private void slice(boolean slice) {
        if (slice) {
            bt_slice.setEnabled(false);
            bt_slice.setText(getString(R.string.slicing));
            new Thread(new Runnable() {
                @Override
                public void run() {
                    Log.d(TAG, "start slice");
                    String path = SD_CARD_PATH + "text.apk";
                    String path_slice = SD_CARD_PATH + "text_%d.apk";
                    FileUtils.slice(path, path_slice, 3);
                    try {
                        Thread.sleep(10 * 1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            bt_slice.setText(getString(R.string.merge));
                            bt_slice.setEnabled(true);
                        }
                    });
                    Log.d(TAG, "end slice");
                }
            }).start();
        } else {
            bt_slice.setEnabled(false);
            bt_slice.setText(getString(R.string.merging));
            new Thread(new Runnable() {
                @Override
                public void run() {
                    Log.d(TAG, "start merge");
                    String path_slice = SD_CARD_PATH + "text_%d.apk";
                    String path_merge = SD_CARD_PATH + "text_merge.apk";
                    FileUtils.merge(path_slice, path_merge, 3);
                    try {
                        Thread.sleep(10 * 1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            bt_slice.setText(getString(R.string.merge_ok));
                            bt_slice.setEnabled(false);
                        }
                    });
                    Log.d(TAG, "end merge");
                }
            }).start();
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
