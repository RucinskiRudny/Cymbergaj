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
	TextView PrzyspieszenieY;
	Button polacz;
	Button reset;
	Context context;
	EditText adres;
	EditText port;

	float predkoscX;
	float predkoscY;
	byte[] paczka=new byte[8];
	byte[] Xbyte=new byte[4];
	byte[] Ybyte=new byte[4];
	int licznik=0;
	int licznik2=0;
	boolean polaczony=false;

	
	private Socket socket; 
	private static int SERVERPORT;
	private static String SERVER_IP;
	
	
	
	 private SensorEventListener mSensorEventListener = new SensorEventListener() {

		    public void onAccuracyChanged(Sensor sensor, int accuracy) { }

		    public void onSensorChanged(SensorEvent event) {
		    	
		    	PrzyspieszenieX.setText(String.valueOf(event.values[1]));
		    	PrzyspieszenieY.setText(String.valueOf(event.values[2]));
		    	if(event.values[1]<45)
		    		predkoscX=event.values[1];
		    	else
		    		predkoscX=45;
		    	
		    	if(event.values[1]<-45)
		    		predkoscX=-45;
		    	
		    	if(event.values[2]<45)
		    		predkoscY=event.values[2];
		    	else
		    		predkoscY=45;
		    	
		    	if(event.values[2]<-45)
		    		predkoscY=-45;

		    	licznik++;
		    	
		    	try { 
		    						licznik=0;
		    						licznik2++;
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
		PrzyspieszenieY = (TextView) findViewById(R.id.NazwaY1);
		polacz = (Button) findViewById(R.id.Polacz);
		port = (EditText)findViewById(R.id.Port);
		adres = (EditText)findViewById(R.id.Adres);
		mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
		mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION);
		mSensorManager.registerListener(mSensorEventListener, mSensor, SensorManager.SENSOR_DELAY_GAME);
		
		polacz.setOnClickListener(new OnClickListener() {			 
            public void onClick(View v) {
            	if(!polaczony)
            	{
            		polaczony=true;
            		SERVER_IP=adres.getText().toString();
            		String portstring=port.getText().toString();
            		SERVERPORT=Integer.parseInt(portstring);
            		new Thread(new ClientThread()).start();
            	}
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
	

	
	@Override
	public void onBackPressed() {
		/*try {
			socket.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}*/
		System.exit(1);
	    return;
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
