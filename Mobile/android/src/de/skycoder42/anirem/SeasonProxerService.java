package de.skycoder42.anirem;

import android.app.NotificationManager;
import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.BitmapFactory;
import android.os.IBinder;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.NotificationCompat.BigTextStyle;
import android.support.v4.content.WakefulBroadcastReceiver;

import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtService;

public class SeasonProxerService extends QtService {
	private static final int PROGRESS_NOT_KEY = 13;
	private static final int STATUS_NOT_KEY = 42;
	private static final int OPEN_INTENT_ID = 10;
	public static final String GROUP_KEY = "de.skycoder42.seasonproxer.NotificationGroup";

	private NotificationCompat.Builder progressBuilder = null;
	private Intent startIntent = null;

	public static native void quitApp();

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		super.onStartCommand(intent, flags, startId);
		startIntent = intent;
		return Service.START_NOT_STICKY;
	}

	@Override
	public void onDestroy() {
		if(startIntent != null) {
			WakefulBroadcastReceiver.completeWakefulIntent(startIntent);
			startIntent = null;
		}
		super.onDestroy();
		quitApp();
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	public void showProgressNotification() {
		progressBuilder = new NotificationCompat.Builder(this)
			.setContentTitle(getResources().getString(R.string.update_progress_title))
			.setContentText(getResources().getString(R.string.update_progress_text))
			.setContentInfo(getResources().getString(R.string.app_name))
			.setCategory(NotificationCompat.CATEGORY_PROGRESS)
			.setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setOngoing(true)
			.setProgress(0, 0, true)
			.setGroup(GROUP_KEY);

		startForeground(PROGRESS_NOT_KEY, progressBuilder.build());
	}

	public void updateProgress(int current, int max)
	{
		NotificationManager manager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
		progressBuilder.setProgress(max, current, false)
			.setContentText(current + " / " + max);
		manager.notify(PROGRESS_NOT_KEY, progressBuilder.build());
	}

	public void showUpdateNotification(boolean success, String title, String message) {
		stopForeground(true);

		NotificationManager manager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);

		Intent openIntent = new Intent(this, QtActivity.class);
		PendingIntent pending = PendingIntent.getActivity(this,
			OPEN_INTENT_ID,
			openIntent,
			PendingIntent.FLAG_UPDATE_CURRENT);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(this)
			.setContentTitle(title)
			.setContentText(message)
			.setStyle(new NotificationCompat.BigTextStyle()
				.bigText(message))
			.setContentIntent(pending)
			.setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setAutoCancel(true)
			.setCategory(success ? NotificationCompat.CATEGORY_EVENT : NotificationCompat.CATEGORY_ERROR)
			.setDefaults(Notification.DEFAULT_ALL)
			.setGroup(GROUP_KEY);

		manager.notify(STATUS_NOT_KEY, builder.build());
	}
}