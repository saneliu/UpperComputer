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

namespace TEST
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            //线程设置为其他线程可以调用该线程的资源
            Control.CheckForIllegalCrossThreadCalls = false;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            //让下拉框只能选择已有选项，不能输入
            comboBox1.DropDownStyle = ComboBoxStyle.DropDownList;

            //自动搜索并且添加可用串口
            string[] PortName = SerialPort.GetPortNames();//读取
            for(int i=0;i<PortName.Length;i++)//添加
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
        }

        //串口接收响应方法  相当于单片机中断函数
        void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
           // throw new NotImplementedException();
            string ReceiveData;
            //读取数据
            ReceiveData = serialPort1.ReadLine();
            if(ReceiveData[0]==1)
            {
                int i=0x01;
                i++;
            }
        }

        //串口刷新操作
        private void button1_Click(object sender, EventArgs e)
        {
            //清空之前的选项
            comboBox1.Items.Clear();
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
        }

        private void button2_Click(object sender, EventArgs e)
        {
            //发送数据
            byte[] data = new byte[1];
            data[0] = 1;
            try
            {
                //第零位开始发  发一个
                serialPort1.Write(data,0,1);
            }
            catch
            {
                MessageBox.Show("发送数据失败");
            }
        }

        
        private void button3_Click(object sender, EventArgs e)
        {
            //打开串口
            if(serialPort1.IsOpen == false)
            {
                try
                {
                    serialPort1.Open();
                    button3.Text="关闭";
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
                    button3.Text = "打开";
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
    }
}
