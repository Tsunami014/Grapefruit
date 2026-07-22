package org.extra.AndrdQtBridge;

import android.app.DatePickerDialog;
import android.content.Context;
import android.widget.DatePicker;

public class DatePick {
    public static void show(Context context, int year, int month, int day) {
        DatePickerDialog dialog = new DatePickerDialog(context,
            new DatePickerDialog.OnDateSetListener() {
                @Override
                public void onDateSet(DatePicker view, int y, int m, int d) {
                    nativeOnDateSet(y, m, d);
                }
            }, year, month, day);
        dialog.setOnCancelListener(di -> nativeOnCancel());
        dialog.show();
    }

    public static native void nativeOnDateSet(int year, int month, int day);
    public static native void nativeOnCancel();
}
