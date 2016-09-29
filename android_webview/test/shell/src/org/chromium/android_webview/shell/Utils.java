package org.chromium.android_webview.shell;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.widget.EditText;

public class Utils
{
    public static AlertDialog createAlertDialog(Context context, String title,
            String message, DialogInterface.OnClickListener onClickListener,
            int numberOfButtons, String yesButtonText, String noButtonText,
            String cancelButtonText)
    {
        AlertDialog alertDialog = new AlertDialog.Builder(context).create();
        alertDialog.setTitle(title);
        alertDialog.setMessage(message);
        alertDialog.setButton(AlertDialog.BUTTON_POSITIVE, yesButtonText,
                onClickListener);
        if (numberOfButtons > 1)
        {
            alertDialog.setButton(AlertDialog.BUTTON_NEGATIVE, noButtonText,
                    onClickListener);
        }
        if (numberOfButtons > 2)
        {
            alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, cancelButtonText,
                    onClickListener);
        }
        return alertDialog;
    }    

    public static AlertDialog createPromptDialog(Context context, EditText editText, String title,
            String message, DialogInterface.OnClickListener onClickListener,
            int numberOfButtons, String yesButtonText, String noButtonText,
            String cancelButtonText)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder.setView(editText);        
        AlertDialog alertDialog = builder.create();
        alertDialog.setTitle(title);
        alertDialog.setMessage(message);
        alertDialog.setButton(AlertDialog.BUTTON_POSITIVE, yesButtonText,
                onClickListener);
        if (numberOfButtons > 1)
        {
            alertDialog.setButton(AlertDialog.BUTTON_NEGATIVE, noButtonText,
                    onClickListener);
        }
        if (numberOfButtons > 2)
        {
            alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, cancelButtonText,
                    onClickListener);
        }
        return alertDialog;
    }    

    public static String readFromAssets(Context context, String filename)
            throws IOException
    {
        BufferedReader reader = new BufferedReader(new InputStreamReader(context
                .getAssets().open(filename)));

        // do reading, usually loop until end of file reading
        StringBuilder sb = new StringBuilder();
        String mLine = reader.readLine();
        while (mLine != null)
        {
            sb.append(mLine + System.getProperty("line.separator")); // process line
            mLine = reader.readLine();
        }
        reader.close();
        return sb.toString();
    }   
}
