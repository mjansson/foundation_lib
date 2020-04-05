package com.maniccoder.foundation.test;

import android.os.Bundle;
import android.app.NativeActivity;
import android.graphics.Color;
import android.graphics.Point;
import android.widget.TextView;
import android.util.Log;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.view.View;
import android.view.Gravity;
import android.view.Display;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.view.ViewGroup.MarginLayoutParams;

public class TestActivity extends NativeActivity
{
	private TextView textView;
	private boolean displayedTextView = false;

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);

		if (!displayedTextView && hasFocus) {
			displayedTextView = true;

			setContentView(R.layout.main);

			textView = (TextView)findViewById(R.id.logtext);
			textView.setText("");
			textView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {}
            });
            textView.setOnLongClickListener(new View.OnLongClickListener() {
                @Override
                public boolean onLongClick(View v) { return true; }
            });
			((ViewGroup)textView.getParent()).removeView(textView);

			final TestActivity activity = this;

			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					PopupWindow popup = new PopupWindow(activity);

					Display display = getWindowManager().getDefaultDisplay();
					Point size = new Point();
					display.getSize(size);

					popup.setWidth(size.x);
					popup.setHeight(size.y);
					popup.setWindowLayoutMode(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
					popup.setClippingEnabled(false);

					MarginLayoutParams params = new MarginLayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
					params.setMargins(0, 0, 0, 0);

					LinearLayout layout = new LinearLayout(activity);
					layout.setOrientation(LinearLayout.VERTICAL);
					layout.addView(activity.textView, params);

					popup.setContentView(layout);

					final ViewGroup viewGroup = (ViewGroup)((ViewGroup)activity.findViewById(android.R.id.content)).getChildAt(0);

					popup.showAtLocation(viewGroup, Gravity.TOP, 0, 0);
					popup.update();
				}
			});
		}
	}

	public void appendLog( final String msg )
	{
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (textView != null)
					textView.append(msg);
			}
		});
	}
}
