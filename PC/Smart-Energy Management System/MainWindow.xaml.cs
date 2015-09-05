using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Visifire.Charts;
using System.Windows.Threading;
using System.Data;


namespace Smart_Energy_Management_System
{

 
    
    
    
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    /// 

    public partial class MainWindow : Window
    {



        // Create new DataPoint collection
        //DataPointCollection dpc = new DataPointCollection();
        
        const string SqliteDBPath = "e:\\SmartPlugDB.db3"; //数据库路径

        public DataTable PlugDeviceTable = new DataTable();

        string[] CurveItemsName ={ "P", "V", "I", "Q1", "S", "S1", "PF", "PF1", "SN", "N", "THDI" };

        DispatcherTimer SimulateDataTimer = new DispatcherTimer();
        DispatcherTimer DisplayTimer = new DispatcherTimer();

        DateTime startDateTime = new DateTime(2013, 1, 1, 00, 00, 00);
        DateTime endDateTime = new DateTime(2013, 1, 1, 00, 00, 00);


        public MainWindow()
        {
            InitializeComponent();



            SimulateDataTimer.Interval = TimeSpan.FromSeconds(2);
            DisplayTimer.Interval = TimeSpan.FromSeconds(1);

            SimulateDataTimer.Tick += new EventHandler(SimulateDataTimer_Tick);
            DisplayTimer.Tick += new EventHandler(DisplayTimer_Tick);
            SimulateDataTimer.Start();
            DisplayTimer.Start();

            initBudgettedUsedChart();
            RefreshPlugInfo();


            SelectDatePicker.SelectedDate = DateTime.Now.Date;
            int curhour = DateTime.Now.TimeOfDay.Hours;
            if (curhour < 6)
            {
                ViewComboBox.SelectedIndex = 0;
            }
            else if (curhour < 12)
            {
                ViewComboBox.SelectedIndex = 1;
            }
            else if (curhour < 18)
            {
                ViewComboBox.SelectedIndex = 2;
            }
            else if (curhour < 24)
            {
                ViewComboBox.SelectedIndex = 3;
            }
            
            CurveComboBox.SelectedIndex = 0;


            

        }
        uint uptime = 0;
        private void SimulateDataTimer_Tick(object sender, EventArgs e)
        {
            Random _rand = new Random(DateTime.Now.Millisecond);
            
            PlugRTData plugrtdata = new PlugRTData();

            uptime += 2;

            plugrtdata.PlugID = 3813057585;
            plugrtdata.UpTime = uptime;
            plugrtdata.V = _rand.Next(210, 230);
            plugrtdata.I = _rand.Next(0, 10);
            plugrtdata.P = _rand.Next(0, 2300);
            plugrtdata.Q1 = _rand.Next(0, 100);
            plugrtdata.S = _rand.Next(0, 2400);
            plugrtdata.S1 = _rand.Next(0, 2350);
            plugrtdata.PF = _rand.Next(0, 1);
            plugrtdata.PF1 = _rand.Next(0, 1);
            plugrtdata.SN = _rand.Next(0, 100);
            plugrtdata.N = _rand.Next(0, 100);
            plugrtdata.THDI = _rand.Next(0, 100);
            plugrtdata.ECounter = _rand.Next(0, 10);
            plugrtdata.PCounter = _rand.Next(0, 10);
            plugrtdata.Freq = 50;
            plugrtdata.RelayState = true;
            plugrtdata.TransmitInterval = 2;
            plugrtdata.AddDateTime = DateTime.Now;

            try
            {
                SmartPlugDAL.AddPlugRTData(plugrtdata, SqliteDBPath);
            }
            catch
            {

            }


        }


        private void DisplayTimer_Tick(object sender, EventArgs e)
        {
            /*
            Random _rand = new Random(DateTime.Now.Millisecond);

            DataPoint dataPoint = new DataPoint()
            {
                // Setting XValue as DateTime will create DateTime Axis
                
                XValue = DateTime.Now,
                YValue = _rand.Next(0, 4000)
            };
            // Create new DataPoints in to collection
            dpc.Add(dataPoint);
            */

            // Set the new points collection as DataPoints

            //DisplayChart();

            this.Dispatcher.BeginInvoke(new Action(DisplayChart));
            
        }

        private void DisplayChart()
        {
            ChartControl.Series[0].DataPoints = GetPlugRTDataPointByDateTime(Convert.ToInt64(PlugDeviceTable.Rows[DeviceList.SelectedIndex]["PlugID"]), startDateTime, endDateTime, CurveItemsName[CurveComboBox.SelectedIndex]);
        }





        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Point p = Mouse.GetPosition(this);
            if (p.Y < 100)
            {
                this.DragMove();
            }
            
        }

        //双击最大化/正常
        private void Window_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            
            Point p= Mouse.GetPosition(this);

            if (p.Y < 100)
            {
                if (this.WindowState != WindowState.Maximized)
                {
                    this.WindowState = WindowState.Maximized;
                    BudgetChartControl.Height = 350;
                    ChartControl.Height = 450;
                }
                else
                {
                    this.WindowState = WindowState.Normal;
                    BudgetChartControl.Height = 230;
                    ChartControl.Height = 350;
                }

            }
        }

        //关闭
        private void Window_Close(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        //最小化
        private void Window_Min(object sender, RoutedEventArgs e)
        {
            this.WindowState = WindowState.Minimized;
        }






        //获取所有plug并显示
        private void RefreshPlugInfo()
        {
            bool success = false;
            PlugDeviceTable = SmartPlugDAL.GetAllPlugDevice(SqliteDBPath, ref success);

            if (success == true)
            {

                int dtrowcount = PlugDeviceTable.Rows.Count;

                AddedDeviceList.Items.Clear();
                DeviceList.Items.Clear();
                DeviceScheList.Items.Clear();

                for (int i = 0; i < dtrowcount; i++)
                {
                    AddedDeviceList.Items.Add(PlugDeviceTable.Rows[i]["PlugName"].ToString());
                    DeviceList.Items.Add(PlugDeviceTable.Rows[i]["PlugName"].ToString());
                    DeviceScheList.Items.Add(PlugDeviceTable.Rows[i]["PlugName"].ToString());
                }
                AddedDeviceList.SelectedIndex = 0;
                DeviceList.SelectedIndex = 0;
                DeviceScheList.SelectedIndex = 0;
            }
            else
            {
                MessageBox.Show("无法获取插座列表", "提示", MessageBoxButton.OK);
            }
        }

         

        //删除插座记录
        private void btnRemoveDev_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (Add_IDTextBox.Text != "")
                {                    
                    Int64 plugid = Convert.ToInt64(Add_IDTextBox.Text);
                    bool success =  SmartPlugDAL.DeletePlugDevice(plugid, SqliteDBPath);

                    if (success == true)
                    {
                        RefreshPlugInfo(); //更新列表
                        MessageBox.Show("成功删除该插座！", "提示", MessageBoxButton.OK);
                        AddedDeviceList.SelectedIndex = 0;
                    }
                    else
                    {
                        MessageBox.Show("无法删除该插座,请检查设置！", "提示", MessageBoxButton.OK);
                    }    
                }
                else
                {
                    MessageBox.Show("请选择要删除的插座！","提示",MessageBoxButton.OK);
                }
            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace);
            }
            catch (Exception ex)
            {
                //Do　any　logging　operation　here　if　necessary  
                throw new Exception(ex.Message);
            } 
        }

        //添加插座
        private void btnAddDev_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                
                if (Add_NameTextBox.Text == "")
                {
                    MessageBox.Show("建议不要将名称设置为空！", "提示", MessageBoxButton.OK);
                }
                if (Add_IDTextBox.Text == "")
                {
                    MessageBox.Show("ID不能为空！", "提示", MessageBoxButton.OK);
                    return;
                }
                if (Add_MACTextBox.Text == "")
                {
                    MessageBox.Show("建议不要将MAC地址设置为空！", "提示", MessageBoxButton.OK);
                }
                if (Add_UserCodeTextBox1.Text == "" || Add_UserCodeTextBox2.Text == "")
                {
                    MessageBox.Show("建议不要将用户码设置为空！", "提示", MessageBoxButton.OK);
                }
                if (Add_VoltageTextBox.Text == "")
                {
                    MessageBox.Show("建议不要将额定电压设置为空！", "提示", MessageBoxButton.OK);
                }
                if (Add_PowerTextBox.Text == "")
                {
                    MessageBox.Show("建议不要将额定功率设置为空！", "提示", MessageBoxButton.OK);
                }

                PlugDevice plugdev = new PlugDevice();
                plugdev.PlugID =Convert.ToInt64(Add_IDTextBox.Text);
                plugdev.PlugName = Add_NameTextBox.Text;
                plugdev.PlugMac = Add_MACTextBox.Text;
                plugdev.UserCode = (UInt16)(256*Convert.ToUInt16(Add_UserCodeTextBox1.Text) + Convert.ToUInt16(Add_UserCodeTextBox2.Text));
                plugdev.Voltage = Convert.ToSingle(Add_VoltageTextBox.Text);
                plugdev.Power = Convert.ToSingle(Add_PowerTextBox.Text);
                plugdev.DayBudget = Convert.ToSingle(Add_BudgettedEnergyTextBox.Text);
                plugdev.Description = Add_DescriptionTextBox.Text;
                plugdev.CreateDateTime = DateTime.Now;

                bool success =  SmartPlugDAL.AddPlugDevice(plugdev, SqliteDBPath);

                if (success == true)
                {
                    RefreshPlugInfo(); //更新列表
                    AddedDeviceList.SelectedIndex = PlugDeviceTable.Rows.Count - 1;
                    MessageBox.Show("成功添加该插座！", "提示", MessageBoxButton.OK);
                }
                else
                {
                    MessageBox.Show("无法添加该插座，请检查设置！", "提示", MessageBoxButton.OK);
                }

            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace);
            }
            catch (Exception ex)
            {
                //Do　any　logging　operation　here　if　necessary  
                throw new Exception(ex.Message);
            } 
        }

        //窗体加载
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            this.WindowState = WindowState.Normal;

        }

        string[] XTimeValue = new string[4] { "1", "2", "3", "4" };
        float[] BudgettedEnergy = new float[4] { 0, 0, 0, 0 };
        float[] UsedEnergy = new float[4] { 0, 0, 0, 0 };



        private void initBudgettedUsedChart()
        {
            for (Int32 j = 0; j < 2; j++)
            {
                // Create a new instance of DataSeries
                DataSeries dataSeries = new DataSeries();

                // Set DataSeries properties
                dataSeries.RenderAs = RenderAs.Column;
                dataSeries.XValueType = ChartValueTypes.Numeric;

                // Create a DataPoint
                DataPoint dataPoint;

                for (int i = 0; i < 4; i++)
                {
                    // Create a new instance of DataPoint
                    dataPoint = new DataPoint();

                    // Set XValue for a DataPoint
                    dataPoint.XValue = XTimeValue[i];

                    // Set YValue for a DataPoint
                    dataPoint.YValue = (j == 0 ? BudgettedEnergy[i] : UsedEnergy[i]);

                    // Add dataPoint to DataPoints collection
                    dataSeries.DataPoints.Add(dataPoint);
                }

                // Add dataSeries to Series collection.

                BudgetChartControl.Series.Add(dataSeries);
            }
            BudgetChartControl.Series[0].Name ="预算使用";
            BudgetChartControl.Series[1].Name = "平均每天使用";
            
        }
        private void UpdateBudgettedUsedChart()
        {
            for (int i = 0; i < 4; i++)
            {
                BudgetChartControl.Series[0].DataPoints[i].YValue = BudgettedEnergy[i];
                BudgetChartControl.Series[1].DataPoints[i].YValue = UsedEnergy[i];
            }
        }


        private void btnUpdateSche_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (NightBudgetTextBox.Text == "" || MorningBudgetTextBox.Text == "" || AfternoonBudgetTextBox.Text == "" || EveningBudgetTextBox.Text == "")
                {
                    MessageBox.Show("请输入预算使用的电能量！", "提示", MessageBoxButton.OK);
                    return;
                }
                int selectindex = DeviceScheList.SelectedIndex;

                if(selectindex != -1)
                {
                    PlugDevice plugdev = new PlugDevice();
                    plugdev.PlugID = Convert.ToInt64(PlugDeviceTable.Rows[selectindex]["PlugID"]);
                    plugdev.NightBudget = Convert.ToSingle(NightBudgetTextBox.Text);
                    plugdev.MorningBudget = Convert.ToSingle(MorningBudgetTextBox.Text);
                    plugdev.AfternoonBudget = Convert.ToSingle(AfternoonBudgetTextBox.Text);
                    plugdev.EveningBudget = Convert.ToSingle(AfternoonBudgetTextBox.Text);
                    plugdev.NightSaving = NightSavingSwitch.IsChecked;
                    plugdev.MorningSaving = MorningSavingSwitch.IsChecked;
                    plugdev.AfternoonSaving = AfternoonSavingSwitch.IsChecked;
                    plugdev.EveningSaving = EveningSavingSwitch.IsChecked;

                    bool success = SmartPlugDAL.UpdatePlugScheduleInfoByPlugID(plugdev, SqliteDBPath);
                    if (success == true)
                    {
                        PlugDeviceTable = SmartPlugDAL.GetAllPlugDevice(SqliteDBPath, ref success);
                        BudgettedEnergy[0] = Convert.ToSingle(NightBudgetTextBox.Text);
                        BudgettedEnergy[1] = Convert.ToSingle(MorningBudgetTextBox.Text);
                        BudgettedEnergy[2] = Convert.ToSingle(AfternoonBudgetTextBox.Text);
                        BudgettedEnergy[3] = Convert.ToSingle(EveningBudgetTextBox.Text);
                        UpdateBudgettedUsedChart();
                        MessageBox.Show("更新成功！", "提示", MessageBoxButton.OK);

                    }
                    else
                    {
                        MessageBox.Show("无法更新，请检查设置！", "提示", MessageBoxButton.OK);
                    }

                }




            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace);
            }
            catch (Exception ex)
            {
                //Do　any　logging　operation　here　if　necessary  
                throw new Exception(ex.Message);
            } 
            
            
        }


        //更新插座表
        private void btnUpdateDev_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (Add_IDTextBox.Text != "")
                {
                    PlugDevice plugdev = new PlugDevice();
                    plugdev.PlugID = Convert.ToInt64(Add_IDTextBox.Text);
                    plugdev.PlugName = Add_NameTextBox.Text;
                    plugdev.PlugMac = Add_MACTextBox.Text;
                    plugdev.UserCode = (UInt16)(256 * Convert.ToUInt16(Add_UserCodeTextBox1.Text) + Convert.ToUInt16(Add_UserCodeTextBox2.Text));
                    plugdev.Voltage = Convert.ToSingle(Add_VoltageTextBox.Text);
                    plugdev.Power = Convert.ToSingle(Add_PowerTextBox.Text);
                    plugdev.DayBudget = Convert.ToSingle(Add_BudgettedEnergyTextBox.Text);
                    plugdev.Description = Add_DescriptionTextBox.Text;

                    bool success = SmartPlugDAL.UpdatePlugBasicInfoByPlugID(plugdev, SqliteDBPath);

                    if (success == true)
                    {
                        RefreshPlugInfo();
                        MessageBox.Show("更新成功！", "提示", MessageBoxButton.OK);
                    }
                    else
                    {
                        MessageBox.Show("更新失败，请检查设置！", "提示", MessageBoxButton.OK);
                    }
                }

            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace);
            }
            catch (Exception ex)
            {
                //Do　any　logging　operation　here　if　necessary  
                throw new Exception(ex.Message);
            } 
        }

        //列表更新
        private void AddedDeviceList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            int selectindex = AddedDeviceList.SelectedIndex;

            if (selectindex != -1)
            {
                Add_NameTextBox.Text = PlugDeviceTable.Rows[selectindex]["PlugName"].ToString();
                Add_IDTextBox.Text = PlugDeviceTable.Rows[selectindex]["PlugID"].ToString();
                Add_MACTextBox.Text = PlugDeviceTable.Rows[selectindex]["PlugMac"].ToString();
                Add_UserCodeTextBox1.Text = Convert.ToString((Convert.ToUInt16(PlugDeviceTable.Rows[selectindex]["UserCode"])) / 256);
                Add_UserCodeTextBox2.Text = Convert.ToString((Convert.ToUInt16(PlugDeviceTable.Rows[selectindex]["UserCode"])) % 256);
                Add_VoltageTextBox.Text = Convert.ToSingle(PlugDeviceTable.Rows[selectindex]["Voltage"]).ToString("#0.000");
                Add_PowerTextBox.Text = Convert.ToSingle(PlugDeviceTable.Rows[selectindex]["Power"]).ToString("#0.000");
                Add_BudgettedEnergyTextBox.Text = Convert.ToSingle(PlugDeviceTable.Rows[selectindex]["DayBudget"]).ToString("#0.000");
                Add_DescriptionTextBox.Text = PlugDeviceTable.Rows[selectindex]["Description"].ToString();
            }
        }

        //计划中设备列表更新
        private void DeviceScheList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            int selectindex = DeviceScheList.SelectedIndex;

            if (selectindex != -1)
            {
                NightBudgetTextBox.Text = Convert.ToSingle(PlugDeviceTable.Rows[selectindex]["NightBudget"]).ToString("#0.000");
                MorningBudgetTextBox.Text = Convert.ToSingle(PlugDeviceTable.Rows[selectindex]["MorningBudget"]).ToString("#0.000");
                AfternoonBudgetTextBox.Text = Convert.ToSingle(PlugDeviceTable.Rows[selectindex]["AfternoonBudget"]).ToString("#0.000");
                EveningBudgetTextBox.Text = Convert.ToSingle(PlugDeviceTable.Rows[selectindex]["EveningBudget"]).ToString("#0.000");

                NightSavingSwitch.IsChecked = Convert.ToBoolean(PlugDeviceTable.Rows[selectindex]["NightSaving"]);
                MorningSavingSwitch.IsChecked = Convert.ToBoolean(PlugDeviceTable.Rows[selectindex]["MorningSaving"]);
                AfternoonSavingSwitch.IsChecked = Convert.ToBoolean(PlugDeviceTable.Rows[selectindex]["AfternoonSaving"]);
                EveningSavingSwitch.IsChecked = Convert.ToBoolean(PlugDeviceTable.Rows[selectindex]["EveningSaving"]);

                BudgettedEnergy[0] = Convert.ToSingle(NightBudgetTextBox.Text);
                BudgettedEnergy[1] = Convert.ToSingle(MorningBudgetTextBox.Text);
                BudgettedEnergy[2] = Convert.ToSingle(AfternoonBudgetTextBox.Text);
                BudgettedEnergy[3] = Convert.ToSingle(EveningBudgetTextBox.Text);
                UpdateBudgettedUsedChart();
            }
        }

        private void DeviceList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            
        }


        // 显示 PlugRTData
        private DataPointCollection GetPlugRTDataPointByDateTime(Int64 plugid, DateTime startDateTime, DateTime endDateTime, string itemStr)
        {
            try
            {
                bool success = false;
                DataTable dt = SmartPlugDAL.GetSelectedPlugRTDataByPlugIDandDateTime(plugid, startDateTime,endDateTime, itemStr+",DateTime", SqliteDBPath, ref success);
                DataPointCollection dpc = new DataPointCollection();
                if (success == true)
                {
                    int dtrowcount = dt.Rows.Count;

                    for (int i = 0; i < dtrowcount; i++)
                    {
                        DataPoint dp = new DataPoint()
                        {
                            XValue =dt.Rows[i]["DateTime"],
                            YValue = Convert.ToDouble(dt.Rows[i][itemStr])
                        };
                        dpc.Add(dp);

                    }
                    return dpc;
                }
                return dpc;        
            }
            catch (Exception ex)
            {
                //Do　any　logging　operation　here　if　necessary  
                throw new Exception(ex.Message);
            } 
        }


        private void SelectDatePicker_SelectedDateChanged(object sender, SelectionChangedEventArgs e)
        {
            
        }


        //选择不同曲线
        private void CurveComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            
            switch (CurveComboBox.SelectedIndex)
            {
                case 0://有功功率P
                    
                    ChartControl.Titles[0].Text="有功功率(W)-时间(hh:mm:ss)";
                    ChartControl.Series[0].RenderAs = RenderAs.Area;
                    ChartControl.Series[0].Color = new SolidColorBrush(Colors.Green);

                    break;


                case 1://电压V

                    ChartControl.Titles[0].Text = "电压(V)-时间(hh:mm:ss)";
                    ChartControl.Series[0].RenderAs = RenderAs.QuickLine;
                    ChartControl.Series[0].LineThickness=2;
                    ChartControl.Series[0].Color = new SolidColorBrush(Colors.Red);

                    break;

                case 2: //电流I
                    ChartControl.Titles[0].Text = "电流(A)-时间(hh:mm:ss)";
                    ChartControl.Series[0].RenderAs = RenderAs.QuickLine;
                    ChartControl.Series[0].LineThickness=2;
                    ChartControl.Series[0].Color = new SolidColorBrush(Colors.SkyBlue);

                    break;

                case 3: //基波无功功率Q1
                    ChartControl.Titles[0].Text = "基波无功功率(var)-时间(hh:mm:ss)";
                    ChartControl.Series[0].RenderAs = RenderAs.QuickLine;
                    ChartControl.Series[0].LineThickness = 2;
                    ChartControl.Series[0].Color = new SolidColorBrush(Colors.SteelBlue);

                    break;

                case 4: //视在功率S
                    ChartControl.Titles[0].Text = "视在功率(VA)-时间(hh:mm:ss)";
                    ChartControl.Series[0].RenderAs = RenderAs.QuickLine;
                    ChartControl.Series[0].LineThickness = 2;
                    ChartControl.Series[0].Color = new SolidColorBrush(Colors.YellowGreen);

                    break;

                case 5: //基波视在功率S1
                    ChartControl.Titles[0].Text = "基波视在功率(VA)-时间(hh:mm:ss)";
                    ChartControl.Series[0].RenderAs = RenderAs.QuickLine;
                    ChartControl.Series[0].LineThickness = 2;
                    ChartControl.Series[0].Color = new SolidColorBrush(Colors.Chocolate);

                    break;

                case 6: //功率因数PF
                    ChartControl.Titles[0].Text = "功率因数-时间(hh:mm:ss)";
                    ChartControl.Series[0].RenderAs = RenderAs.QuickLine;
                    ChartControl.Series[0].LineThickness = 2;
                    ChartControl.Series[0].Color = new SolidColorBrush(Colors.BurlyWood);

                    break;

                case 7: //基波功率因数PF1
                    ChartControl.Titles[0].Text = "基波功率因数-时间(hh:mm:ss)";
                    ChartControl.Series[0].RenderAs = RenderAs.QuickLine;
                    ChartControl.Series[0].LineThickness = 2;
                    ChartControl.Series[0].Color = new SolidColorBrush(Colors.Brown);

                    break;

                case 8: //谐波视在功率
                    ChartControl.Titles[0].Text = "谐波视在功率(VA)-时间(hh:mm:ss)";
                    ChartControl.Series[0].RenderAs = RenderAs.QuickLine;
                    ChartControl.Series[0].LineThickness = 2;
                    ChartControl.Series[0].Color = new SolidColorBrush(Colors.Chartreuse);

                    break;

                case 9: //无功功率N
                    ChartControl.Titles[0].Text = "无功功率(var)-时间(hh:mm:ss)";
                    ChartControl.Series[0].RenderAs = RenderAs.QuickLine;
                    ChartControl.Series[0].LineThickness = 2;
                    ChartControl.Series[0].Color = new SolidColorBrush(Colors.Cyan);

                    break;

                case 10: //总谐波电流失真THDI
                    ChartControl.Titles[0].Text = "总谐波电流失真(%)-时间(hh:mm:ss)";
                    ChartControl.Series[0].RenderAs = RenderAs.QuickLine;
                    ChartControl.Series[0].LineThickness = 2;
                    ChartControl.Series[0].Color = new SolidColorBrush(Colors.DarkGray);

                    break;

            }
            ChartControl.Series[0].DataPoints = GetPlugRTDataPointByDateTime(Convert.ToInt64(PlugDeviceTable.Rows[DeviceList.SelectedIndex]["PlugID"]), startDateTime, endDateTime, CurveItemsName[CurveComboBox.SelectedIndex]);

        }


        //
        private void DateSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            DateTime dt = new DateTime(SelectDatePicker.SelectedDate.Value.Year, SelectDatePicker.SelectedDate.Value.Month, SelectDatePicker.SelectedDate.Value.Day, 0, 0, 0);
            switch (ViewComboBox.SelectedIndex)
            {
                case 0: //00:00-06:00
                    startDateTime = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 0, 0, 0);
                    endDateTime   = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 6, 0, 0);
                    ChartControl.Series[0].XValueType = ChartValueTypes.DateTime;
                    ChartControl.AxesX[0].ValueFormatString = "HH:mm:ss";
                    break;

                case 1: //06:00-12:00
                    startDateTime = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 6, 0, 0);
                    endDateTime   = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 12, 0, 0);
                    ChartControl.Series[0].XValueType = ChartValueTypes.Time;
                    ChartControl.AxesX[0].ValueFormatString = "HH:mm:ss";
                    break;

                case 2: //12:00-18:00
                    startDateTime = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 12, 0, 0);
                    endDateTime   = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 18, 0, 0);
                    ChartControl.Series[0].XValueType = ChartValueTypes.Time;
                    ChartControl.AxesX[0].ValueFormatString = "HH:mm:ss";
                    break;

                case 3: //18:00-24:00
                    
                    startDateTime = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 18, 0, 0);
                    endDateTime   = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 23, 59, 59);
                    ChartControl.Series[0].XValueType = ChartValueTypes.Time;
                    ChartControl.AxesX[0].ValueFormatString = "HH:mm:ss";
                    break;

                case 4: //日
                    startDateTime = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 0, 0, 0);
                    endDateTime   = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 23, 59, 59);
                    ChartControl.Series[0].XValueType = ChartValueTypes.Time;
                    ChartControl.AxesX[0].ValueFormatString = "HH:mm:ss";
                    break;

                case 5: //周
                    dt = dt.AddDays(0 - Convert.ToInt32(dt.DayOfWeek));//本周第一天
                    startDateTime = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 0, 0, 0);
                    dt = dt.AddDays(6 - Convert.ToInt32(dt.DayOfWeek));//本周最后一天
                    endDateTime = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 23, 59, 59);
                    ChartControl.Series[0].XValueType = ChartValueTypes.DateTime;
                    ChartControl.AxesX[0].ValueFormatString = "";
                    break;


                case 6: //月
                    dt = dt.AddDays(1 - dt.Day); //本月第一天
                    startDateTime = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 0, 0, 0);
                    dt = dt.AddDays(1 - dt.Day).AddMonths(1).AddDays(-1); // 本月最后一天
                    endDateTime = new DateTime(dt.Date.Year, dt.Date.Month, dt.Date.Day, 23, 59, 59);
                    ChartControl.Series[0].XValueType = ChartValueTypes.Date;
                    ChartControl.AxesX[0].ValueFormatString = "";
                    break;

                case 7: //年
                    startDateTime = new DateTime(dt.Date.Year, 1, 1, 0, 0, 0);
                    endDateTime   = new DateTime(dt.Date.Year, 12,31, 23, 59, 59);
                    ChartControl.Series[0].XValueType = ChartValueTypes.Date;
                    ChartControl.AxesX[0].ValueFormatString = "";
                    
                    break;
            }
            ChartControl.Series[0].DataPoints = GetPlugRTDataPointByDateTime(Convert.ToInt64(PlugDeviceTable.Rows[DeviceList.SelectedIndex]["PlugID"]), startDateTime, endDateTime, CurveItemsName[CurveComboBox.SelectedIndex == -1 ? 0 : CurveComboBox.SelectedIndex]);
        }



 
 


    }







}

