package com.kgm.multitouchserver;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

public class MainActivity extends AppCompatActivity {
    static int instances = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i("MultiTouchServer", "  Instance " + instances);
        if (++instances > 1)
        {
            Log.i("MultiTouchServer", "  Already Running");
        }
        else {
            final Intent intent = new Intent(this, MultiTouchService.class);
            intent.putExtra("serverPort", 9999);
            startService(intent);
        }
        finish();
        instances--;
    }
}
