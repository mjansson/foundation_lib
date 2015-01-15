package com.rampantpixels.foundation.test;

import android.os.Bundle;
import android.app.NativeActivity;
import android.graphics.Color;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

public class TestActivity extends NativeActivity
{
	private TextView textView;

	@Override
	protected void onCreate( Bundle savedInstanceState )
	{
		super.onCreate( savedInstanceState );

		setContentView( R.layout.main );

		textView = (TextView)findViewById( R.id.logtext );
		textView.setText( "Test text view" );
	}
}
