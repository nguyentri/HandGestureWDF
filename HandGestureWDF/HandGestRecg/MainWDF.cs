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
using maincpp;
using ImageTakeNS;
using  System.Windows.Media.Imaging;
using  System.Windows.Media;
using System.IO;

namespace HandGestRecg
{
    public partial class MainWDF : Form
    {

        //static Semaphore newimg_flag = new Semaphore(0,0);

        exmainCpp maincppObj = new exmainCpp();

        ImageTakeCls ImagObj = new ImageTakeCls();



        public MainWDF()
        {
            InitializeComponent();

            diplayLogo();

            Thread maincppT = new Thread(maincpp_func);
            Thread dispImgT = new Thread(DisplayImg);

            maincppT.Priority = ThreadPriority.Highest;
            dispImgT.Priority = ThreadPriority.Normal;
            maincppT.Start();
            dispImgT.Start();

        }

        private void maincpp_func()
        {
            maincppObj.mainCpp(ImagObj);
            Thread.Sleep(10);
        }

        private void UpdateStatus()
        {
            BitmapSource image = (BitmapSource)ImagObj.getImage();
            pictureBox.Image = BitmapFromSource(image);
            pictureBox.Height = (int)image.Height;
            pictureBox.Width = (int)image.Width;
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

        private void buttonChangeImg_Click(object sender, EventArgs e)
        {

        }

        private void buttonPause_Click(object sender, EventArgs e)
        {
            //("pause");
        }

        private void diplayLogo()
        {
            Image image = Image.FromFile("logo_hcmut.png");
            pictureBoxLogo.Image = (image);
            pictureBoxLogo.Height = (int)image.Height;
            pictureBoxLogo.Width = (int)image.Width;
        }

        private void button1_Click(object sender, EventArgs e)
        {

        }


    }
}
