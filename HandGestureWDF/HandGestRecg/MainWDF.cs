using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using System.Windows.Media.Imaging;
using System.Windows.Media;
using System.IO;
using AForge.Video.FFMPEG;

using maincpp;
using ImageTakeNS;
using VideoControlNS;

namespace HandGestRecg
{
    public partial class MainWDF : Form
    {

        //static Semaphore newimg_flag = new Semaphore(0,0);

        exmainCpp maincppObj = new exmainCpp();

        ImageTakeCls ImagObj = new ImageTakeCls();

        VideoControlCls videoControlObj = new VideoControlCls();

        VideoFileWriter writer = new VideoFileWriter();

        public MainWDF()
        {
            InitializeComponent();

            Thread maincppT = new Thread(maincpp_func);
            Thread dispImgT = new Thread(DisplayImg);

            maincppT.Priority = ThreadPriority.Highest;
            dispImgT.Priority = ThreadPriority.Normal;
            maincppT.Start();
            dispImgT.Start();

        }

        private void maincpp_func()
        {
            maincppObj.mainCpp(ImagObj, videoControlObj);
            Thread.Sleep(10);
        }

        private void UpdateStatus()
        {
            

            BitmapSource image = (BitmapSource)ImagObj.getImage();

            Bitmap img = BitmapFromSource(image);

            /*Update to picture box */
            if (img != null)
            {
                pictureBox.Image = img;
                pictureBox.Height = (int)image.Height;
                pictureBox.Width = (int)image.Width;

                /*Check to record video */
                //if (writer.IsOpen)
                //{
                //    writer.WriteVideoFrame(img);
                //}
            }

        }

        delegate void UpdateStatusInvoker();

        private void DisplayImg()
        {
            for (; ; ) { 
                // Construct an image object from a file in the local directory.
                // ... This file must exist in the solution.
                if (ImagObj.ImgValid() != false && ImagObj.imgIsUpdated_b == true)
                {
                    ImagObj.imgIsUpdated_b = false;
                    // Set the PictureBox image property to this image.
                    // ... Then, adjust its height and width properties.
                    this.Invoke(new UpdateStatusInvoker(UpdateStatus));
                }
                Thread.Sleep(1);
             }
        }

        private System.Drawing.Bitmap BitmapFromSource(BitmapSource bitmapsource)
        {
            if (bitmapsource != null)
            {
                System.Drawing.Bitmap bitmap;
                using (MemoryStream outStream = new MemoryStream())
                {
                    BitmapEncoder enc = new BmpBitmapEncoder();
                    enc.Frames.Add(BitmapFrame.Create(bitmapsource));
                    enc.Save(outStream);
                    bitmap = new System.Drawing.Bitmap(outStream);
                }
                return bitmap;
            }
            else
            {
                return null;
            }
        }



        private void buttonRGBImg_Click(object sender, EventArgs e)
        {
            videoControlObj.VideoControl = 99; // "c"
        }


        private void buttonDepImage_Click(object sender, EventArgs e)
        {
            videoControlObj.VideoControl = 100; // "d"
        }

        private void buttonRecord_Click(object sender, EventArgs e)
        {
            //videoControlObj.VideoControl = 114; // "r"
            int width = 640;
            int height = 480;

            writer.Open("video.avi", width, height, 25, VideoCodec.MPEG4, 1000000);
        }

        private void buttonTraining_Click(object sender, EventArgs e)
        {
            videoControlObj.VideoControl = 116; // "t"
        }

        private void buttonStopRec_Click(object sender, EventArgs e)
        {
           writer.Close();
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            base.OnFormClosing(e);

            if (e.CloseReason == CloseReason.WindowsShutDown) return;

            // Confirm user wants to close
            switch (MessageBox.Show(this, "Are you sure you want to close?", "Closing", MessageBoxButtons.YesNo))
            {
                case DialogResult.No:
                    e.Cancel = true;
                    break;
                default:
                    // The user wants to exit the application. Close everything down.
                    videoControlObj.VideoControl = 27;//ESC
                    Thread.Sleep(500);
                    //Application.Exit(e);
                    Environment.Exit(0);
                    break;
            }
        }

        private void buttonSnapshot_Click(object sender, EventArgs e)
        {
           // BitmapSource image = (BitmapSource)ImagObj.getImage();
            //write image
            videoControlObj.VideoControl = 111;// allow to capture image
        }

    }
}
