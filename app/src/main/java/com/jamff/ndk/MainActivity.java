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

    private Button mButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mEncrypt = new Encrypt();
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
        mButton = findViewById(R.id.bt_encrypt);
        mButton.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.bt_encrypt:
                encrypt(getString(R.string.encrypt).contentEquals(mButton.getText()));
                break;
        }
    }

    /**
     * 加密解密
     *
     * @param encrypt true 加密 false 解密
     */
    private void encrypt(boolean encrypt) {
        if (encrypt) {
            mButton.setEnabled(false);
            mButton.setText(getString(R.string.encrypting));
            new Thread(new Runnable() {
                @Override
                public void run() {
                    Log.d(TAG, "start encrypt");
                    String normal_path = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separatorChar + "text.apk";
                    String crypt_path = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separatorChar + "text_encrypt.apk";
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
                            mButton.setText(getString(R.string.decrypt));
                            mButton.setEnabled(true);
                        }
                    });
                    Log.d(TAG, "done encrypt");
                }
            }).start();
        } else {
            mButton.setEnabled(false);
            mButton.setText(getString(R.string.decrypting));
            new Thread(new Runnable() {
                @Override
                public void run() {
                    Log.d(TAG, "start decrypt");
                    String crypt_path = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separatorChar + "text_encrypt.apk";
                    String decrypt_path = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separatorChar + "text_decrypt.apk";
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
                            mButton.setText(getString(R.string.decrypt_ok));
                            mButton.setEnabled(false);
                        }
                    });
                    Log.d(TAG, "done decrypt");
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
