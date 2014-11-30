using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.IO;
using System.Diagnostics;

using maincpp;
using ImageTakeNS;
using VideoControlNS;

namespace RockPaperScissorGame
{
    public partial class WForm : Form
    {

        exmainCpp maincppObj = new exmainCpp();

        ImageTakeCls ImagObj = new ImageTakeCls();

        VideoControlCls videoControlObj = new VideoControlCls();

        bool startGame;

        Thread GestureSysT;
        Thread DispT;

        public WForm()
        {
            InitializeComponent();

            startGame = false;

            GestureSysT = new Thread(maincpp_func);
            DispT = new Thread(main_funcgame);

            GestureSysT.Priority = ThreadPriority.Highest;
            DispT.Priority = ThreadPriority.Normal;
            GestureSysT.Start();
            DispT.Start();
        }


        private void maincpp_func()
        {
            maincppObj.mainCpp(ImagObj, videoControlObj);
            Thread.Sleep(10);
        }

        private void UpdateStatus()
        {
            if (videoControlObj.videoOutGesture_ub != 0xff)
            {
                if (videoControlObj.videoOutGesture_ub == 1) // start new game
                {
                    startGame = true;
                }
                else if (videoControlObj.videoOutGesture_ub == 7 || videoControlObj.videoOutGesture_ub == 3)
                {
                    startGame = true;
                    btnClear_Click(null, null);
                }
                else
                {
                    lableStartGame.Text = "Please perform gesture number 1 to start new game OR number 3 to clear the result!";
                }

                if (startGame == true)
                {
                    if (videoControlObj.videoOutGesture_ub == 0)
                    {
                        startGame = false;
                        rockPicturebox_Click(null, null);
                    }
                    else if (videoControlObj.videoOutGesture_ub == 2 || videoControlObj.videoOutGesture_ub == 9)
                    {
                        startGame = false;
                        scissorsPictureBox_Click(null, null);
                    }
                    else if ((videoControlObj.videoOutGesture_ub == 4) || (videoControlObj.videoOutGesture_ub == 5))
                    {
                        startGame = false;
                        paperPicturebox_Click(null, null);
                    }
                    else
                    {
                        lableStartGame.Text = "Please take your choise!";
                    }
                }
            }
            else
            {
                lableStartGame.Text = "Please wave your hand to start game!";
            }       
        }

        delegate void UpdateStatusInvoker();


        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void main_funcgame()
        {
            try
            {
                if (IsHandleCreated)
                {
                    if (InvokeRequired) {
                        for (; ; )
                        {
                    
                            this.Invoke(new UpdateStatusInvoker(UpdateStatus));
                            Thread.Sleep(10);
                        }
                    }
                    else
                    {

                    }
                }
            }
            catch (System.Exception ex)
            {
                Console.WriteLine("\nMessage ---\n{0}", ex.Message);
            }
        }


        private void btnClear_Click(object sender, EventArgs e)
        {
            // Clearing the images
            computerpicturebox.Image = null;
            PlayerPictureBox.Image = null;
            panelPlayer.BackColor = Color.LightGray;
            panelComputer.BackColor = Color.LightGray;
            winnerLabel.Text = "................";
            listBox1.Items.Clear();
        }

        private void rockPicturebox_Click(object sender, EventArgs e)
        {
            Random oRandomGenerator = new Random();
            panelPlayer.BackColor = Color.Cornsilk;
            panelComputer.BackColor = Color.Cornsilk;
            int iComputerChoice;

            PlayerPictureBox.Image = rockPicturebox.Image;

            int iRandomNum = oRandomGenerator.Next(100, 4000);
            iComputerChoice = (iRandomNum % 3) + 1;

            switch (iComputerChoice)
            {
                case 1:

                    computerpicturebox.Image = rockPicturebox.Image;
                    winnerLabel.Text = "TIE";
                    listBox1.Items.Add("TIE");
                    break;

                case 2:

                    computerpicturebox.Image = paperPicturebox.Image;
                    winnerLabel.Text = " Computer wins because paper covers rock ";
                    panelComputer.BackColor = Color.Crimson;
                    listBox1.Items.Add("Won by Computer");
                    break;

                case 3:

                    computerpicturebox.Image = scissorsPictureBox.Image;
                    winnerLabel.Text = " YOU win because rock breaks scissors ";
                    panelPlayer.BackColor = Color.Crimson;
                    listBox1.Items.Add("Won by you");
                    break;
            }
        }

        private void paperPicturebox_Click(object sender, EventArgs e)
        {
            Random oRandomGenerator = new Random();
            panelPlayer.BackColor = Color.Cornsilk;
            panelComputer.BackColor = Color.Cornsilk;
            int iComputerChoice;

            PlayerPictureBox.Image = paperPicturebox.Image;

            int iRandomNum = oRandomGenerator.Next(100, 4000);
            iComputerChoice = (iRandomNum % 3) + 1;

            switch (iComputerChoice)
            {

                case 1:

                    computerpicturebox.Image = rockPicturebox.Image;
                    winnerLabel.Text = " YOU win because paper covers rock ";
                    panelPlayer.BackColor = Color.Crimson;
                    listBox1.Items.Add("Won by you");
                    break;

                case 2:

                    computerpicturebox.Image = paperPicturebox.Image;
                    winnerLabel.Text = "TIE";
                    listBox1.Items.Add("TIE");
                    break;

                case 3:

                    computerpicturebox.Image = scissorsPictureBox.Image;
                    winnerLabel.Text = " Computer wins because scissors cuts paper ";
                    panelComputer.BackColor = Color.Crimson;
                    listBox1.Items.Add("Won by Computer");
                    break;

            }

        }

        private void scissorsPictureBox_Click(object sender, EventArgs e)
        {
            Random oRandomGenerator = new Random();
            panelPlayer.BackColor = Color.Cornsilk;
            panelComputer.BackColor = Color.Cornsilk;
            int iComputerChoice;

            PlayerPictureBox.Image = scissorsPictureBox.Image;

            int iRandomNum = oRandomGenerator.Next(100, 4000);
            iComputerChoice = (iRandomNum % 3) + 1;

            switch (iComputerChoice)
            {

                case 1:

                    computerpicturebox.Image = rockPicturebox.Image;
                    winnerLabel.Text = " Computer wins because rock breaks scissors ";
                    panelComputer.BackColor = Color.Crimson;
                    listBox1.Items.Add("Won by Computer");
                    break;

                case 2:

                    computerpicturebox.Image = paperPicturebox.Image;
                    winnerLabel.Text = " YOU win because scissors cuts paper ";
                    panelPlayer.BackColor = Color.Crimson;
                    listBox1.Items.Add("Won by you");
                    break;

                case 3:

                    computerpicturebox.Image = scissorsPictureBox.Image;
                    winnerLabel.Text = "TIE";
                    listBox1.Items.Add("TIE");
                    break;
            }
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


    }
}
