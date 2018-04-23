package com.mkdinteractive.avara_engine;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
//    static {
//        //System.loadLibrary("native-lib");
//        System.loadLibrary("ae");
//    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText("Yo");
        //tv.setText(stringFromJNI());
        //tv.setText(Java_com_mkdinteractive_avara_1engine_MainActivity_stringFromJNI());
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    //public native String stringFromJNI();
    //public native String Java_com_mkdinteractive_avara_1engine_MainActivity_stringFromJNI();
}
