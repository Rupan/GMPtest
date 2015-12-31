package org.gmplib.tests;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import java.util.Random;

public class TestDriver extends Activity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Random r = new Random();
        int int_a = r.nextInt(100);
        int int_b = r.nextInt(100);
        String sum = add(String.valueOf(int_a), String.valueOf(int_b), 10);

        TextView tv = new TextView(this);
        tv.setText("The sum of " + int_a + " and " + int_b + " is " + sum);
        setContentView(tv);
    }

    public native String add(String a, String b, int base);

    static {
        System.loadLibrary("gmp");
        System.loadLibrary("gmp-tests");
    }
}
