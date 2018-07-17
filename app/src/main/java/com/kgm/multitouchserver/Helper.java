package com.kgm.multitouchserver;

import android.app.ActivityManager;
import android.content.Context;
import android.util.Log;

import java.util.List;

public class Helper {
    public static boolean isAppRunning(final Context context, final String packageName) {
        final ActivityManager activityManager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        final List<ActivityManager.RunningAppProcessInfo> procInfos = activityManager.getRunningAppProcesses();
        if (procInfos != null)
        {
            int nFound = 0;
            for (final ActivityManager.RunningAppProcessInfo processInfo : procInfos) {
                Log.i("MultiTouchServer", "  " + processInfo.processName);

                if (processInfo.processName.equals(packageName)) {
                    //
                    // True if more than one instance (us and others)
                    //
                    if (++nFound > 1) return true;
                }
            }
        }
        return false;
    }
}

