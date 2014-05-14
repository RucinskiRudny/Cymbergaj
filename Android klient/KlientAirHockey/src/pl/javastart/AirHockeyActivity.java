package pl.javastart;

import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.SystemClock;
import android.support.v4.app.Fragment;
import android.support.v7.app.ActionBarActivity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;




public class AirHockeyActivity extends ActionBarActivity {

	private SensorManager mSensorManager;
	private Sensor mSensor;
	TextView PrzyspieszenieX;
	TextView PredkoscX;
	TextView PrzyspieszenieY;
	TextView PredkoscY;
	Button polacz;
	Button reset;
	Context context;
	EditText adres;
	EditText port;
	long czas=0;
	long czasPop=0;
	long czasRoz=0;
	float predkoscX;
	float predkoscY;
	byte[] paczka=new byte[8];
	byte[] Xbyte=new byte[4];
	byte[] Ybyte=new byte[4];
	float akcelerometrX;
	float akcelerometrY;
	int licznik=0;
	
	private Socket socket; 
	private static int SERVERPORT;
	private static String SERVER_IP;
	
	
	
	 private SensorEventListener mSensorEventListener = new SensorEventListener() {

		    public void onAccuracyChanged(Sensor sensor, int accuracy) { }

		    public void onSensorChanged(SensorEvent event) {
		    	//string X = Float.toString(event.values[0]);
		    	//if(event.values[0]>0.5||event.values[0]<-0.5)
		    	PrzyspieszenieX.setText(String.valueOf(event.values[0]));
		    	PrzyspieszenieY.setText(String.valueOf(event.values[0]));
		    	//else
		    	//	naazwaX.setText("stoi");
		    	akcelerometrX=event.values[0];
		    	//akcelerometrY=event.values[1];
		    	akcelerometrY=event.values[1];
		    	/*
		    	if(akcelerometrX<0.3&&akcelerometrX>-0.3)
		    		akcelerometrX=0;
		    	if(akcelerometrY<0.3&&akcelerometrY>-0.3)
		    		akcelerometrY=0;*/
		    	
		    	czas=SystemClock.uptimeMillis();
		    	czasRoz=czas-czasPop;
		    	czasPop=czas;
		    	
		    	predkoscX=predkoscX+akcelerometrX*czasRoz*(float)0.001;
		    	PredkoscX.setText(String.valueOf(predkoscX));
		    	
		    	predkoscY=predkoscY+akcelerometrY*czasRoz*(float)0.001;
		    	PredkoscY.setText(String.valueOf(predkoscY));
		    	
		    	//odbs�uga klienta
		    	licznik++;
		    	if(licznik==50)
		    	try {
		    						licznik=0;        
		    			           /* PrintWriter out = new PrintWriter(new BufferedWriter(
		    			                    new OutputStreamWriter(socket.getOutputStream())),
		    			                    true);*/
		    			            BufferedOutputStream out = new BufferedOutputStream(socket.getOutputStream());
		    			            Xbyte=toByteArray(predkoscX);
		    			            Ybyte=toByteArray(predkoscY);
		    			            paczka[0]=Xbyte[3]; paczka[1]=Xbyte[2]; paczka[2]=Xbyte[1]; paczka[3]=Xbyte[0];
		    			            paczka[4]=Ybyte[3]; paczka[5]=Ybyte[2]; paczka[6]=Ybyte[1]; paczka[7]=Ybyte[0];
		    			            out.write(paczka);
		    			            out.flush();
		    			        } catch (UnknownHostException e) {
		    			            e.printStackTrace();
		    			        } catch (IOException e) {
		    			            e.printStackTrace();
		    			        } catch (Exception e) {
		    			            e.printStackTrace();
		    			        }
		    }
		};
		
		public static byte[] toByteArray(float value) {
		    byte[] bytes = new byte[4];
		    ByteBuffer.wrap(bytes).putFloat(value);
		    return bytes;
		}
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_air_hockey);
		context = getApplicationContext();
			
		PrzyspieszenieX = (TextView) findViewById(R.id.NazwaX1);
		PredkoscX = (TextView) findViewById(R.id.NazwaX2);
		PrzyspieszenieY = (TextView) findViewById(R.id.NazwaY1);
		PredkoscY = (TextView) findViewById(R.id.NazwaY2);
		polacz = (Button) findViewById(R.id.Polacz);
		reset = (Button) findViewById(R.id.Reset);
		port = (EditText)findViewById(R.id.Port);
		adres = (EditText)findViewById(R.id.Adres);
		czas=SystemClock.uptimeMillis();
		czasPop=czas;
		mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
		mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
		mSensorManager.registerListener(mSensorEventListener, mSensor, SensorManager.SENSOR_DELAY_FASTEST);
		
		polacz.setOnClickListener(new OnClickListener() {			 
            public void onClick(View v) {
            	SERVER_IP=adres.getText().toString();
            	String portstring=port.getText().toString();
            	SERVERPORT=Integer.parseInt(portstring);
            	new Thread(new ClientThread()).start();
            }
		});
		
		reset.setOnClickListener(new OnClickListener() {			 
            public void onClick(View v) {
            	predkoscX=0;
            	predkoscY=0;
            }
		});

		
		
		
		if (savedInstanceState == null) {
			getSupportFragmentManager().beginTransaction()
					.add(R.id.container, new PlaceholderFragment()).commit();
		}
	}
	
	class ClientThread implements Runnable {		 
		        @Override
		        public void run() {
	 
		            try {
		                InetAddress serverAddr = InetAddress.getByName(SERVER_IP);
	 
		                socket = new Socket(serverAddr, SERVERPORT);
		 
		            } catch (UnknownHostException e1) {
		                e1.printStackTrace();
		            } catch (IOException e1) {
		                e1.printStackTrace();
		            }
		        }
		    }
	


    

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {

		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.air_hockey, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	/**
	 * A placeholder fragment containing a simple view.
	 */
	public static class PlaceholderFragment extends Fragment {

		public PlaceholderFragment() {
		}

		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container,
				Bundle savedInstanceState) {
			View rootView = inflater.inflate(R.layout.fragment_air_hockey,
					container, false);
			return rootView;
		}
	}

}
