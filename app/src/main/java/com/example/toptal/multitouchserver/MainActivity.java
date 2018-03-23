package com.example.toptal.multitouchserver;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        final Intent intent = new Intent(this, MultiTouchService.class);
        intent.putExtra("serverPort", 9999);
        startService(intent);
        finish();
    }
}
