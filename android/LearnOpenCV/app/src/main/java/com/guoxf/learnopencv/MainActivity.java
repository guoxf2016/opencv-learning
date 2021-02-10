package com.guoxf.learnopencv;

import androidx.appcompat.app.AppCompatActivity;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import jp.co.cyberagent.android.gpuimage.GPUImage;
import jp.co.cyberagent.android.gpuimage.filter.GPUImageLookupFilter;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private ImageView mImageView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mImageView = findViewById(R.id.sample_image);
        new MyAsyncTask().execute("test.jpg");
    }

    private class MyAsyncTask extends AsyncTask<String, Void, Bitmap> {

        @Override
        protected Bitmap doInBackground(String... strings) {
            String fileName = strings[0];
            if (fileName == null) {
                Log.w(TAG, "doInBackground, fileName is null");
                return null;
            }
            Bitmap bitmap = null;
            AssetManager assetManager = getAssets();
            InputStream inputStream = null;
            InputStream inputStream1 = null;
            try {
                inputStream = assetManager.open(fileName);
                inputStream1 = assetManager.open("red.png");
                Bitmap inputBitmap = BitmapFactory.decodeStream(inputStream1);
                int length = inputStream.available();
                byte[] buffer = new byte[length];
                int readCount = inputStream.read(buffer);
                if (readCount == -1) {
                    Log.w(TAG, "read file failed");
                    return null;
                }
                ByteBuffer byteBuffer = ByteBuffer.allocateDirect(length).order(ByteOrder.nativeOrder());
                byteBuffer.put(buffer);
                byteBuffer.position(0);
//                Bitmap inputBitmap = BitmapFactory.decodeResource(getResources(), R.drawable.linuxlogo);
                Bitmap inputBitmap2 = BitmapFactory.decodeResource(getResources(), R.drawable.windowslogo);
                long s = System.currentTimeMillis();
//                bitmap = process8(byteBuffer, length, inputBitmap);
                bitmap = gpuImageFilterApply(inputBitmap2, inputBitmap);
                long e = System.currentTimeMillis();
                Log.d(TAG, "process take time " + (e - s));
            } catch (IOException e) {
                Log.w(TAG, "open assert throw ex", e.fillInStackTrace());
            } finally {
                if (inputStream != null) {
                    try {
                        inputStream.close();
                    } catch (IOException e) {
                        Log.w(TAG, "close stream throw ex", e.fillInStackTrace());
                    }
                }
                if (inputStream1 != null) {
                    try {
                        inputStream1.close();
                    } catch (IOException e) {
                        Log.w(TAG, "close stream throw ex", e.fillInStackTrace());
                    }
                }
            }
            return bitmap;
        }

        @Override
        protected void onPostExecute(Bitmap bitmap) {
            if (isFinishing() || isDestroyed()) {
                return;
            }
            if (bitmap == null) {
                Log.w(TAG, "onPostExecute, bitmap is null");
                return;
            }
            mImageView.setImageBitmap(bitmap);
        }
    }



    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native static Bitmap process(String imageFile);

    //decode jpeg byte buffer to cv::Mat and generate a Bitmap
    public native static Bitmap process2(ByteBuffer byteBuffer, int length);

    //apply a kernel to Mat
    public native static Bitmap process3(ByteBuffer byteBuffer, int length);

    //blending
    public native static Bitmap process4(Bitmap inputBitmap, Bitmap inputBitmap2);

    //convertTo, Brightness and contrast adjustments
    //beta control brightness, and alpha control contrast
    //It can occur that playing with the β bias will improve the brightness
    // but in the same time the image will appear with a slight veil as the contrast will be reduced.
    // The α gain can be used to diminue this effect but due to the saturation,
    // we will lose some details in the original bright regions.
    public native static Bitmap process5(ByteBuffer byteBuffer, int length);

    //Gamma correction can be used to correct the brightness of an image by using a non linear transformation
    // between the input values and the mapped output values
    //When γ<1, the original dark regions will be brighter and the histogram will be shifted to the right
    // whereas it will be the opposite with γ>1.
    public native static Bitmap process6(ByteBuffer byteBuffer, int length);

    public native static Bitmap process7(ByteBuffer byteBuffer, int length);

    public native static Bitmap process8(ByteBuffer byteBuffer, int length, Bitmap lut3d);

    private Bitmap gpuImageFilterApply(Bitmap input, Bitmap lookupTable) {
        GPUImage gpuImage = new GPUImage(this);
        GPUImageLookupFilter filter = new GPUImageLookupFilter();
        filter.setBitmap(lookupTable);
        gpuImage.setFilter(filter);
        return gpuImage.getBitmapWithFilterApplied(input);
    }
}
