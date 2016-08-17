using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
//添加这个类就可以使用SerialPort方法 获取可用串口
using System.IO.Ports;
//使用延时函数
using System.Threading;

namespace OV7725调车上位机
{
  
    public partial class Form1 : Form
    {       
        #region  通讯协议
        /**********发送数据类型****************/
        const byte Balance_P = 0x20;
        const byte Balance_I = 0x21;
        const byte Balance_D = 0x22;
        const byte Position_P = 0x23;
        const byte Position_I = 0x24;
        const byte Position_D = 0x25;
        const byte Speed_P = 0x26;
        const byte Speed_I = 0x27;
        const byte Speed_D = 0x28;
        const byte Steer_P = 0x29;
        const byte Steer_I = 0x2A;
        const byte Steer_D = 0x2B;
        const byte Forward = 0x2C;
        const byte Backward = 0x2D;
        const byte TurnLeft = 0x2E;
        const byte TurnRight = 0x2F;
        const byte Firing = 0x30;
        const byte Misfire = 0x31;
        bool GoFlag = false;
        /**********接收数据类型****************/
        const byte Rec_CPUStatus = 0x32;
        const byte Rec_LeftRoom = 0x33;
        const byte Rec_MiddleLine = 0x34;
        const byte Rec_LSpeed = 0x35;
        const byte Rec_RSpeed = 0x36;
        const byte Rec_Pitch = 0x37;
        const byte Rec_Yaw = 0x38;
        const byte Rec_Roll = 0x39;
        const byte Rec_Number1 = 0x3A;
        const byte Rec_Number2 = 0x3B;
        const byte Rec_Number3 = 0x3C;
        const byte Rec_Number4 = 0x3D;
        const byte Rec_Number5 = 0x3E;
        const byte Rec_Number6 = 0x3F;
        #endregion
       public Form1()
        {
            InitializeComponent();
            //线程设置为其他线程可以调用该线程的资源
            Control.CheckForIllegalCrossThreadCalls = false;
        }
        #region  窗口初始化
        private void Form1_Load(object sender, EventArgs e)
        {
            #region PID数据初始化
            textBox1.Text = "0.0";
            textBox2.Text = "0.0";
            textBox3.Text = "0.0";
            textBox4.Text = "0.0";
            textBox5.Text = "0.0";
            textBox6.Text = "0.0";
            textBox7.Text = "0.0";
            textBox8.Text = "0.0";
            textBox9.Text = "0.0";
            textBox10.Text = "0.0";
            textBox11.Text = "0.0";
            textBox12.Text = "0.0";
            #endregion
            #region  波特率初始化
            string[] BaudRate = { "9600", "115200" };
            comboBox2.DropDownStyle = ComboBoxStyle.DropDownList;
            for (int i = 0; i < BaudRate.Length; i++)
            {
                comboBox2.Items.Add(BaudRate[i]);
            }
            comboBox2.Text = "115200";
            serialPort1.BaudRate = 115200;
            #endregion
            #region  串口初始化
            //让下拉框只能选择已有选项，不能输入
            comboBox1.DropDownStyle = ComboBoxStyle.DropDownList;
            //自动搜索并且添加可用串口
            string[] PortName = SerialPort.GetPortNames();//读取
            for (int i = 0; i < PortName.Length; i++)//添加
            {
                comboBox1.Items.Add(PortName[i]);
            }

            try
            {
                comboBox1.SelectedIndex = 0;
            }
            catch
            {
                MessageBox.Show("没有可用端口");
            }
            //串口接受消息响应设置
            serialPort1.DataReceived += serialPort1_DataReceived;
            #endregion
        }
        #endregion
        #region  串口接收
        //串口接收响应方法  相当于单片机中断函数
        void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            //throw new NotImplementedException();
        #region  缓存区数据处理
            byte ReceiveData;
            float usart_temp=0;
            //读取数据
            ReceiveData = (byte)serialPort1.ReadByte();
            if ((global.USART_RX_STA & 0x8000) == 0) //接收未完成
            {
                if ((global.USART_RX_STA & 0x4000) !=0) //接收到了0x0d
                {
                    if (ReceiveData != 0x0a)
                    {
                        global.USART_RX_STA = 0; //接收错误,重新开始
                    }
                    else
                    {
                        global.USART_RX_STA |= 0x8000;	//接收完成了
                    }
                }
                else //还没收到0X0D
                {
                    if (ReceiveData == 0x0d)
                    {
                        global.USART_RX_STA |= 0x4000;
                    }
                    else
                    {
                        global.USART_RX_BUF[global.USART_RX_STA & 0X3FFF] = (byte)ReceiveData;
                        global.USART_RX_STA++;
                        if (global.USART_RX_STA > (global.USART_REC_LEN - 1))
                        {
                            global.USART_RX_STA = 0; //接收数据错误,重新开始接收
                        }
                    }
                }
            }
        #endregion
            #region  接收数据显示
            if ((global.USART_RX_STA & 0x8000)!=0)
            {
                int len;
                len = global.USART_RX_STA & 0x3fff; //得到此次接收到的数据长度
                //检验第一位  确保数据正确
                if (global.USART_RX_BUF[0] == 0xff)
                {
                    if (len == 4)
                    {
                        usart_temp = (float)(global.USART_RX_BUF[2]) + ((float)global.USART_RX_BUF[3]) / 100;
                    }
                    else if(len==3)
                    {
                        usart_temp = (float)global.USART_RX_BUF[2];
                    }
                    switch (global.USART_RX_BUF[1])
                    {
                        case Rec_CPUStatus:
                        label17.Text=usart_temp.ToString()+"%";
                        break;
                        case Rec_LeftRoom:
                        label18.Text = usart_temp.ToString()+"K";
                        break;
                        case Rec_MiddleLine:
                        label25.Text=usart_temp.ToString();
                        break;
                        case Rec_LSpeed:
                        label26.Text = usart_temp.ToString();
                        break;
                        case Rec_RSpeed:
                        label27.Text = usart_temp.ToString();
                        break;
                        case Rec_Pitch:
                        label28.Text = usart_temp.ToString();
                        break;
                        case Rec_Yaw:
                        label29.Text = usart_temp.ToString();
                        break;
                        case Rec_Roll:
                        label30.Text = usart_temp.ToString();
                        break;
                        case Rec_Number1:
                        label37.Text = usart_temp.ToString();
                        break;
                        case Rec_Number2:
                        label38.Text = usart_temp.ToString();
                        break;
                        case Rec_Number3:
                        label39.Text = usart_temp.ToString();
                        break;
                        case Rec_Number4:
                        label40.Text = usart_temp.ToString();
                        break;
                        case Rec_Number5:
                        label41.Text = usart_temp.ToString();
                        break;
                        case Rec_Number6:
                        label42.Text = usart_temp.ToString();
                        break;
                    }
                }
            #endregion
                global.USART_RX_STA = 0;
            }
        }
        #endregion
        #region  打开关闭串口
        private void button1_Click(object sender, EventArgs e)
        {
            //打开串口
            if (serialPort1.IsOpen == false)
            {
                try
                {
                    serialPort1.Open();
                    button1.Text = "关闭";
                }
                catch
                {
                    MessageBox.Show("打开失败");
                }
            }
            //关闭串口
            else
            {
                try
                {
                    serialPort1.Close();
                    button1.Text = "打开";
                }
                catch
                {
                    MessageBox.Show("关闭失败");
                }
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            //设置端口名称
            serialPort1.PortName = comboBox1.Text;
        }

        private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {
            //设置波特率
            serialPort1.BaudRate = int.Parse(comboBox2.Text);
        }
        #endregion
        #region  串口刷新
        private void button2_Click(object sender, EventArgs e)
        {
            //清空之前的选项
            comboBox1.Items.Clear();

            //自动搜索并且添加可用串口
            string[] PortName = SerialPort.GetPortNames();//读取
            for (int i = 0; i < PortName.Length; i++)//添加
            {
                comboBox1.Items.Add(PortName[i]);
            }

            try
            {
                comboBox1.SelectedIndex = 0;
            }
            catch
            {
                MessageBox.Show("没有可用端口");
            }
        }
        #endregion
        #region  串口发送协议
        private void SendData(float data,byte type)
        {
            //发送数据
            byte[] Send_Data = new byte[6] { 0xff, type, 0x00, 0x00, 0x0d, 0x0a };
            //temp_1整数部分  temp_2两位小数
            byte temp_1,temp_2;
            temp_1 = (byte)data;
            Send_Data[2] = temp_1;
            temp_2 = (byte)(100*(data-(float)temp_1));
            Send_Data[3] = temp_2;
            try
            {
                //第零位开始发  发6个
                serialPort1.Write(Send_Data, 0, 6);
            }
            catch
            {
                MessageBox.Show("发送数据失败");
            }
        }
        private void SendData(byte type)
        {
            //发送数据
            byte[] Send_Data = new byte[4] { 0xff, type,  0x0d, 0x0a };
            try
            {
                //第零位开始发  发4个
                serialPort1.Write(Send_Data, 0, 4);
            }
            catch
            {
                MessageBox.Show("发送数据失败");
            }
        }
        #endregion
        #region  速度闭环PID
        private void button3_Click(object sender, EventArgs e)
        {
            float f1=-1,f2=-1,f3=-1;
            try 
            {
                f1 = float.Parse(textBox1.Text);
                f2 = float.Parse(textBox2.Text);
                f3 = float.Parse(textBox3.Text);
            }
            catch
            {
                MessageBox.Show("请输入0~255区间二位小数");
            }

            try
            {
                if ((f1 >= 0) && (f2 >= 0) && (f3 >= 0))
                {
                    SendData(f1, Speed_P);
                    SendData(f2, Speed_I);
                    SendData(f3, Speed_D);
                }
            }
            catch
            {
                MessageBox.Show("发送数据失败");
            }
        }
        #endregion
        #region  位置开环PID
        private void button4_Click(object sender, EventArgs e)
        {
            float f1 = -1, f2 = -1, f3 = -1;
            try
            {
                f1 = float.Parse(textBox6.Text);
                f2 = float.Parse(textBox5.Text);
                f3 = float.Parse(textBox4.Text);
            }
            catch
            {
                MessageBox.Show("请输入0~255区间二位小数");
            }

            try
            {
                if ((f1 >= 0) && (f2 >= 0) && (f3 >= 0))
                {
                    SendData(f1, Position_P);
                    SendData(f2, Position_I);
                    SendData(f3, Position_D);
                }
            }
            catch
            {
                MessageBox.Show("发送数据失败");
            }
        }
        #endregion
        #region  舵机PID
        private void button5_Click(object sender, EventArgs e)
        {
            float f1 = -1, f2 = -1, f3 = -1;
            try
            {
                f1 = float.Parse(textBox9.Text);
                f2 = float.Parse(textBox8.Text);
                f3 = float.Parse(textBox7.Text);
            }
            catch
            {
                MessageBox.Show("请输入0~255区间二位小数");
            }

            try
            {
                if ((f1 >= 0) && (f2 >= 0) && (f3 >= 0))
                {
                    SendData(f1, Steer_P);
                    SendData(f2, Steer_I);
                    SendData(f3, Steer_D);
                }
            }
            catch
            {
                MessageBox.Show("发送数据失败");
            }
        }
        #endregion
        #region 平衡PID
        private void button6_Click(object sender, EventArgs e)
        {
            float f1 = -1, f2 = -1, f3 = -1;
            try
            {
                f1 = float.Parse(textBox12.Text);
                f2 = float.Parse(textBox11.Text);
                f3 = float.Parse(textBox10.Text);
            }
            catch
            {
                MessageBox.Show("请输入0~255区间二位小数");
            }

            try
            {
                if ((f1 >= 0) && (f2 >= 0) && (f3 >= 0))
                {
                    SendData(f1, Balance_P);
                    SendData(f2, Balance_I);
                    SendData(f3, Balance_D);
                }
            }
            catch
            {
                MessageBox.Show("发送数据失败");
            }
        }
        #endregion
        #region 向前
        private void button7_Click(object sender, EventArgs e)
        {
            SendData(Forward);
        }
        #endregion
        #region 向后
        private void button11_Click(object sender, EventArgs e)
        {
            SendData(Backward);
        }
        #endregion
        #region  向左
        private void button8_Click(object sender, EventArgs e)
        {
            SendData(TurnLeft);
        }
        #endregion
        #region 向右
        private void button10_Click(object sender, EventArgs e)
        {
            SendData(TurnRight);
        }
        #endregion
        #region  车启动停止
        private void button9_Click(object sender, EventArgs e)
        {
            if(GoFlag==false)
            {
                SendData(Firing);
                GoFlag = true;
                button9.Text = "停车";
            }
            else
            {
                SendData(Misfire);
                GoFlag = false;
                button9.Text = "发车";
            }
        }
        #endregion  

    }
    #region 全局变量
    public class global
    {
        /**********接收数据缓存****************/
        public static int USART_REC_LEN = 200;
        public static byte[] USART_RX_BUF = new byte[USART_REC_LEN];//接收缓冲,最大200个字节.
        //接收状态
        //bit15，	接收完成标志
        //bit14，	接收到0x0d
        //bit13~0，	接收到的有效字节数目
        public static int USART_RX_STA=0;         		//接收状态标记
    }  
    #endregion
}
