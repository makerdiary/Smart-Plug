using System;
using System.Windows;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data;
using System.Data.SQLite;

namespace Smart_Energy_Management_System
{
    public class PlugDevice
    {
        private Int64 plugid;
        private string plugname;
        private string plugmac;
        private UInt16 usercode;
        private float voltage;
        private float power;
        private float daybudget;
        private float nightbudget;
        private float morningbudget;
        private float afternoonbudget;
        private float eveningbudget;
        private bool nightsaving;
        private bool morningsaving;
        private bool afternoonsaving;
        private bool eveningsaving;
        private string description;
        private DateTime createdatetime;

        public Int64 PlugID
        {
            get { return plugid; }
            set { plugid = value; }
        }

        public string PlugName
        {
            get { return plugname; }
            set { plugname = value; }
        }

        public string PlugMac
        {
            get { return plugmac; }
            set { plugmac = value; }
        }

        public UInt16 UserCode
        {
            get { return usercode; }
            set { usercode = value; }
        }

        public float Voltage
        {
            get { return voltage; }
            set { voltage = value; }
        }

        public float Power
        {
            get { return power; }
            set { power = value; }
        }

        public float DayBudget
        {
            get { return daybudget; }
            set { daybudget = value; }
        }

        public float NightBudget
        {
            get { return nightbudget; }
            set { nightbudget = value; }
        }

        public float MorningBudget
        {
            get { return morningbudget; }
            set { morningbudget = value; }
        }

        public float AfternoonBudget
        {
            get { return afternoonbudget; }
            set { afternoonbudget = value; }
        }

        public float EveningBudget
        {
            get { return eveningbudget; }
            set { eveningbudget = value; }
        }

        public bool NightSaving
        {
            get { return nightsaving; }
            set { nightsaving = value; }
        }

        public bool MorningSaving
        {
            get { return morningsaving; }
            set { morningsaving = value; }
        }

        public bool AfternoonSaving
        {
            get { return afternoonsaving; }
            set { afternoonsaving = value; }
        }

        public bool EveningSaving
        {
            get { return eveningsaving; }
            set { eveningsaving = value; }
        }

        public string Description
        {
            get { return description; }
            set { description = value; }
        }

        public DateTime CreateDateTime
        {
            get { return createdatetime; }
            set { createdatetime = value; }
        }
    }

    public class PlugRTData
    {
        private Int64 plugid;
        private uint uptime;
        private float v;
        private float i;
        private float p;
        private float q1;
        private float s;
        private float s1;
        private float pf;
        private float pf1;
        private float sn;
        private float n;
        private float thdi;
        private float ecounter;
        private float pcounter;
        private int freq;
        private bool relay_state;
        private int transmit_interval;
        private DateTime adddatetime;

        public Int64 PlugID
        {
            get { return plugid; }
            set { plugid = value; }
        }
        public uint UpTime
        {
            get { return uptime; }
            set { uptime = value; }
        }
        public float V
        {
            get { return v; }
            set { v = value; }
        }
        public float I
        {
            get { return i; }
            set { i = value; }
        }
        public float P
        {
            get { return p; }
            set { p = value; }
        }
        public float Q1
        {
            get { return q1; }
            set { q1 = value; }
        }
        public float S
        {
            get { return s; }
            set { s = value; }
        }
        public float S1
        {
            get { return s1; }
            set { s1 = value; }
        }
        public float PF
        {
            get { return pf; }
            set { pf = value; }
        }
        public float PF1
        {
            get { return pf1; }
            set { pf1 = value; }
        }
        public float SN
        {
            get { return sn; }
            set { sn = value; }
        }
        public float N
        {
            get { return n; }
            set { n = value; }
          
        }
        public float THDI
        {
            get { return thdi; }
            set { thdi = value; }
        }
        public float ECounter
        {
            get { return ecounter; }
            set { ecounter = value; }
        }
        public float PCounter
        {
            get { return pcounter; }
            set { pcounter = value; }
        }
        public int Freq
        {
            get { return freq; }
            set { freq = value; }
        }
        public bool RelayState
        {
            get { return relay_state; }
            set { relay_state = value; }
        }
        public int TransmitInterval
        {
            get { return transmit_interval; }
            set { transmit_interval = value; }
        }
        public DateTime AddDateTime
        {
            get { return adddatetime; }
            set { adddatetime = value; }
        }
    }


    public class SmartPlugDAL
    {

        // Add Plug to DataBase
        public static bool AddPlugDevice(PlugDevice plugdev, string dbPath)
        {
            try
            {
                SQLiteConnection conn = new SQLiteConnection("Data Source=" + dbPath + ";");
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                cmd.CommandText = @"INSERT INTO PlugList(PlugID, PlugName, PlugMac, UserCode, Voltage, Power, DayBudget, NightBudget, MorningBudget, AfternoonBudget, EveningBudget,
                                     NightSaving, MorningSaving, AfternoonSaving, EveningSaving, Description, CreateDateTime) 
                                     VALUES(@PlugID1, @PlugName1, @PlugMac1, @UserCode1, @Voltage1, @Power1, @DayBudget1, @NightBudget1, @MorningBudget1, @AfternoonBudget1, @EveningBudget1, 
                                     @NightSaving1, @MorningSaving1, @AfternoonSaving1, @EveningSaving1, @Description1, @CreateDateTime1)";

                cmd.Parameters.Add(new SQLiteParameter("PlugID1", plugdev.PlugID));
                cmd.Parameters.Add(new SQLiteParameter("PlugName1", plugdev.PlugName));
                cmd.Parameters.Add(new SQLiteParameter("PlugMac1", plugdev.PlugMac));
                cmd.Parameters.Add(new SQLiteParameter("UserCode1", plugdev.UserCode));
                cmd.Parameters.Add(new SQLiteParameter("Voltage1", plugdev.Voltage));
                cmd.Parameters.Add(new SQLiteParameter("Power1", plugdev.Power));
                cmd.Parameters.Add(new SQLiteParameter("DayBudget1", plugdev.DayBudget));
                cmd.Parameters.Add(new SQLiteParameter("NightBudget1", plugdev.NightBudget));
                cmd.Parameters.Add(new SQLiteParameter("MorningBudget1", plugdev.MorningBudget));
                cmd.Parameters.Add(new SQLiteParameter("AfternoonBudget1", plugdev.AfternoonBudget));
                cmd.Parameters.Add(new SQLiteParameter("EveningBudget1", plugdev.EveningBudget));
                cmd.Parameters.Add(new SQLiteParameter("NightSaving1", plugdev.NightSaving));
                cmd.Parameters.Add(new SQLiteParameter("MorningSaving1", plugdev.MorningSaving));
                cmd.Parameters.Add(new SQLiteParameter("AfternoonSaving1", plugdev.AfternoonSaving));
                cmd.Parameters.Add(new SQLiteParameter("EveningSaving1", plugdev.EveningSaving));
                cmd.Parameters.Add(new SQLiteParameter("Description1", plugdev.Description));
                cmd.Parameters.Add(new SQLiteParameter("CreateDateTime1", plugdev.CreateDateTime));

                int i = cmd.ExecuteNonQuery();
                return i == 1;
            }
            catch(SQLiteException se)
            {
                MessageBox.Show(se.Message + " \n\n" + se.Source + "\n\n" + se.StackTrace + "\n\n" + se.Data);
                return false;
            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace + "\n\n" + ae.Data);
                return false;
            }
            catch (Exception ex)
            {
                //Do　any　logging　operation　here　if　necessary  
                MessageBox.Show(ex.Message + "\n\n" + ex.Source + "\n\n" + ex.StackTrace + "\n\n" + ex.Data);
                return false;
            }   
        }

        // Update Plug base information by PlugID
        public static bool UpdatePlugBasicInfoByPlugID(PlugDevice plugdev, string dbPath)
        {
            try
            {
                using (SQLiteConnection conn = new SQLiteConnection("Data Source=" + dbPath + ";"))
                {
                    conn.Open();
                    SQLiteCommand cmd = conn.CreateCommand();
                    cmd.CommandText = @"UPDATE PlugList set PlugName=@PlugName1, PlugMac=@PlugMac1, UserCode=@UserCode1, Voltage=@Voltage1, Power=@Power1, DayBudget=@DayBudget1, 
                                        Description=@Description1 where PlugID=@PlugID1;";
                    cmd.Parameters.Add(new SQLiteParameter("PlugID1", plugdev.PlugID));
                    cmd.Parameters.Add(new SQLiteParameter("PlugName1", plugdev.PlugName));
                    cmd.Parameters.Add(new SQLiteParameter("PlugMac1", plugdev.PlugMac));
                    cmd.Parameters.Add(new SQLiteParameter("UserCode1", plugdev.UserCode));
                    cmd.Parameters.Add(new SQLiteParameter("Voltage1", plugdev.Voltage));
                    cmd.Parameters.Add(new SQLiteParameter("Power1", plugdev.Power));
                    cmd.Parameters.Add(new SQLiteParameter("DayBudget1", plugdev.DayBudget));
                    cmd.Parameters.Add(new SQLiteParameter("Description1", plugdev.Description));

                    int i = cmd.ExecuteNonQuery();
                    return i == 1;
                }
            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace);
                return false;
            }
            catch (Exception ex)
            {
                //Do　any　logging　operation　here　if　necessary  
                MessageBox.Show(ex.Message);
                return false;
            } 
        }

        // Update Plug Schedule infor by PlugID
        public static bool UpdatePlugScheduleInfoByPlugID(PlugDevice plugdev, string dbPath)
        {
            try
            {
                using (SQLiteConnection conn = new SQLiteConnection("Data Source=" + dbPath + ";"))
                {
                    conn.Open();
                    SQLiteCommand cmd = conn.CreateCommand();
                    cmd.CommandText = @"UPDATE PlugList set NightBudget=@NightBudget1, MorningBudget=@MorningBudget1, AfternoonBudget=@AfternoonBudget1, EveningBudget=@EveningBudget1, 
                                        NightSaving=@NightSaving1, MorningSaving=@MorningSaving1, AfternoonSaving=@AfternoonSaving1, EveningSaving=@EveningSaving1 where PlugID=@PlugID1;";

                    cmd.Parameters.Add(new SQLiteParameter("PlugID1", plugdev.PlugID));
                    cmd.Parameters.Add(new SQLiteParameter("NightBudget1", plugdev.NightBudget));
                    cmd.Parameters.Add(new SQLiteParameter("MorningBudget1", plugdev.MorningBudget));
                    cmd.Parameters.Add(new SQLiteParameter("AfternoonBudget1", plugdev.AfternoonBudget));
                    cmd.Parameters.Add(new SQLiteParameter("EveningBudget1", plugdev.EveningBudget));
                    cmd.Parameters.Add(new SQLiteParameter("NightSaving1", plugdev.NightSaving));
                    cmd.Parameters.Add(new SQLiteParameter("MorningSaving1", plugdev.MorningSaving));
                    cmd.Parameters.Add(new SQLiteParameter("AfternoonSaving1", plugdev.AfternoonSaving));
                    cmd.Parameters.Add(new SQLiteParameter("EveningSaving1", plugdev.EveningSaving));

                    int i = cmd.ExecuteNonQuery();
                    return i == 1;
                }
            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace);
                return false;
            }
            catch (Exception ex)
            {
                //Do　any　logging　operation　here　if　necessary  
                MessageBox.Show(ex.Message);
                return false;
            } 

        }

        // Delete Plug by PlugID
        public static bool DeletePlugDevice(Int64 plugid, string dbPath)
        {
            try
            {
                using (SQLiteConnection conn = new SQLiteConnection("Data Source=" + dbPath + ";"))
                {
                    conn.Open();
                    SQLiteCommand cmd = conn.CreateCommand();
                    cmd.CommandText = "delete from PlugList where PlugID=@PlugID;";
                    cmd.Parameters.Add(new SQLiteParameter("PlugID", plugid));
                    int i = cmd.ExecuteNonQuery();
                    return i == 1;
                }
            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace);
                return false;
            }
            catch (Exception ex)
            {
                //Do　any　logging　operation　here　if　necessary  
                MessageBox.Show(ex.Message);
                return false;
            } 
        }

        // Get Plug by PlugID
        public static PlugDevice GetPlugDeviceByPlugID(Int64 plugid, string dbPath)
        {
            try
            {
                using (SQLiteConnection conn = new SQLiteConnection("Data Source=" + dbPath + ";"))
                {
                    conn.Open();
                    SQLiteCommand cmd = conn.CreateCommand();
                    cmd.CommandText = "select * from PlugList where PlugID=@PlugID;";
                    cmd.Parameters.Add(new SQLiteParameter("PlugID", plugid));
                    SQLiteDataReader dr = cmd.ExecuteReader();
                    if (dr.Read())
                    {
                        PlugDevice plugdev = new PlugDevice();
                        plugdev.PlugID = dr.GetInt64(0);
                        plugdev.PlugName = dr.GetString(1);
                        plugdev.PlugMac = dr.GetString(2);
                        plugdev.UserCode = (UInt16) dr.GetInt32(3);
                        plugdev.Voltage = dr.GetFloat(4);
                        plugdev.Power = dr.GetFloat(5);
                        plugdev.DayBudget = dr.GetFloat(6);
                        plugdev.NightBudget = dr.GetFloat(7);
                        plugdev.MorningBudget = dr.GetFloat(8);
                        plugdev.AfternoonBudget = dr.GetFloat(9);
                        plugdev.EveningBudget = dr.GetFloat(10);
                        plugdev.NightSaving = dr.GetBoolean(11);
                        plugdev.MorningSaving = dr.GetBoolean(12);
                        plugdev.AfternoonSaving = dr.GetBoolean(13);
                        plugdev.EveningSaving = dr.GetBoolean(14);
                        plugdev.Description = dr.GetString(15);
                        plugdev.CreateDateTime = dr.GetDateTime(16);

                        return plugdev;
                    }
                    else
                        return null;
                }

            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace);
                return null;
            }
            catch (Exception ex)
            {
                //Do　any　logging　operation　here　if　necessary  
                throw new Exception(ex.Message);
            }    
        }

        // Get All PlugDevices
        public static DataTable GetAllPlugDevice(string dbPath, ref bool success)
        {
            DataTable dt = new DataTable();
            success = false;
            try
            {
                SQLiteConnection conn = new SQLiteConnection("Data Source=" + dbPath + ";");
                conn.Open();
                SQLiteCommand cmd = new SQLiteCommand(conn);
                cmd.CommandText = "SELECT * FROM PlugList";
                cmd.CommandType = CommandType.Text;
                SQLiteDataReader dr = cmd.ExecuteReader();
                if (dr.HasRows)
                {
                    dt.Load(dr);
                    success = true;
                }
                else
                {

                }

                dr.Close();
                conn.Close();
            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace + "\n\n" + ae.Data);
            }
            catch (Exception ex)
            {
                //throw new Exception(ex.Message);  
                MessageBox.Show(ex.Message + " \n\n" + ex.Source + "\n\n" + ex.StackTrace + "\n\n" + ex.Data);
            }

            return dt;
        }



        // Add PlugRTData to DataBase-PlugRTData Table
        public static bool AddPlugRTData(PlugRTData plugrtdata, string dbPath)
        {
            try
            {
                SQLiteConnection conn = new SQLiteConnection("Data Source=" + dbPath + ";");
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                cmd.CommandText = @"INSERT INTO PlugRTData(PlugID, UpTime, V, I, P, Q1, S, S1, PF, PF1, SN, N, THDI, ECounter, PCounter, Freq, RelayState, TransmitInternal, DateTime)
                                     VALUES(@_PlugID, @_UpTime, @_V, @_I, @_P, @_Q1, @_S, @_S1, @_PF, @_PF1, @_SN, @_N, @_THDI, @_ECounter, @_PCounter, @_Freq, @_RelayState, @_TransmitInternal, @_DateTime)"; 

                cmd.Parameters.Add(new SQLiteParameter("_PlugID", plugrtdata.PlugID));
                cmd.Parameters.Add(new SQLiteParameter("_UpTime", plugrtdata.UpTime));
                cmd.Parameters.Add(new SQLiteParameter("_V", plugrtdata.V));
                cmd.Parameters.Add(new SQLiteParameter("_I", plugrtdata.I));
                cmd.Parameters.Add(new SQLiteParameter("_P", plugrtdata.P));
                cmd.Parameters.Add(new SQLiteParameter("_Q1", plugrtdata.Q1));
                cmd.Parameters.Add(new SQLiteParameter("_S", plugrtdata.S));
                cmd.Parameters.Add(new SQLiteParameter("_S1", plugrtdata.S1));
                cmd.Parameters.Add(new SQLiteParameter("_PF", plugrtdata.PF));
                cmd.Parameters.Add(new SQLiteParameter("_PF1", plugrtdata.PF1));
                cmd.Parameters.Add(new SQLiteParameter("_SN", plugrtdata.SN));
                cmd.Parameters.Add(new SQLiteParameter("_N", plugrtdata.N));
                cmd.Parameters.Add(new SQLiteParameter("_THDI", plugrtdata.THDI));
                cmd.Parameters.Add(new SQLiteParameter("_ECounter", plugrtdata.ECounter));
                cmd.Parameters.Add(new SQLiteParameter("_PCounter", plugrtdata.PCounter));
                cmd.Parameters.Add(new SQLiteParameter("_Freq", plugrtdata.Freq));
                cmd.Parameters.Add(new SQLiteParameter("_RelayState", plugrtdata.RelayState));
                cmd.Parameters.Add(new SQLiteParameter("_TransmitInternal", plugrtdata.TransmitInterval));
                cmd.Parameters.Add(new SQLiteParameter("_DateTime", plugrtdata.AddDateTime));

                int i = cmd.ExecuteNonQuery();
                return i == 1;
            }
            catch (SQLiteException se)
            {
                MessageBox.Show(se.Message + " \n\n" + se.Source + "\n\n" + se.StackTrace + "\n\n" + se.Data);
                return false;
            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace + "\n\n" + ae.Data);
                return false;
            }
            catch (Exception ex)
            {
                //Do　any　logging　operation　here　if　necessary  
                MessageBox.Show(ex.Message + "\n\n" + ex.Source + "\n\n" + ex.StackTrace + "\n\n" + ex.Data);
                return false;
            }   
        }

        // Get PlugRTData by PlugID
        public static DataTable GetPlugRTDataByPlugID(Int64 plugid, string dbPath, ref bool success)
        {
            DataTable dt = new DataTable();
            success = false;
            try
            {
                SQLiteConnection conn = new SQLiteConnection("Data Source=" + dbPath + ";");
                conn.Open();
                SQLiteCommand cmd = new SQLiteCommand(conn);
                cmd.CommandText = "SELECT * FROM PlugRTData where PlugID=@PlugID;";
                cmd.Parameters.Add(new SQLiteParameter("PlugID", plugid));
                cmd.CommandType = CommandType.Text;
                SQLiteDataReader dr = cmd.ExecuteReader();
                if (dr.HasRows)
                {
                    dt.Load(dr);
                    success = true;
                }
                else
                {

                }

                dr.Close();
                conn.Close();
            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace + "\n\n" + ae.Data);
            }
            catch (Exception ex)
            {
                //throw new Exception(ex.Message);  
                MessageBox.Show(ex.Message + " \n\n" + ex.Source + "\n\n" + ex.StackTrace + "\n\n" + ex.Data);
            }

            return dt;
        }

        // Get PlugRTData by Date
        public static DataTable GetPlugRTDataByDate(DateTime datetime, string dbPath, ref bool success)
        {
            DataTable dt = new DataTable();
            success = false;
            try
            {
                SQLiteConnection conn = new SQLiteConnection("Data Source=" + dbPath + ";");
                conn.Open();
                SQLiteCommand cmd = new SQLiteCommand(conn);
                cmd.CommandText = "SELECT * FROM PlugRTData where date(DateTime)=date(@_DateTime);";
                cmd.Parameters.Add(new SQLiteParameter("_DateTime", datetime));
                cmd.CommandType = CommandType.Text;
                SQLiteDataReader dr = cmd.ExecuteReader();
                if (dr.HasRows)
                {
                    dt.Load(dr);
                    success = true;
                }
                else
                {

                }

                dr.Close();
                conn.Close();
            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace + "\n\n" + ae.Data);
            }
            catch (Exception ex)
            {
                //throw new Exception(ex.Message);  
                MessageBox.Show(ex.Message + " \n\n" + ex.Source + "\n\n" + ex.StackTrace + "\n\n" + ex.Data);
            }

            return dt;
        }

        // Get PlugRTData by PlugID and Date 
        public static DataTable GetPlugRTDataByPlugIDandDate(Int64 plugid, DateTime datetime, string dbPath, ref bool success)
        {
            DataTable dt = new DataTable();
            success = false;
            try
            {
                SQLiteConnection conn = new SQLiteConnection("Data Source=" + dbPath + ";");
                conn.Open();
                SQLiteCommand cmd = new SQLiteCommand(conn);
                cmd.CommandText = "SELECT * FROM PlugRTData where PlugID=@_PlugID and date(DateTime)=date(@_DateTime);";
                cmd.Parameters.Add(new SQLiteParameter("_PlugID", plugid));
                cmd.Parameters.Add(new SQLiteParameter("_DateTime", datetime));
                cmd.CommandType = CommandType.Text;
                SQLiteDataReader dr = cmd.ExecuteReader();
                if (dr.HasRows)
                {
                    dt.Load(dr);
                    success = true;
                }
                else
                {

                }

                dr.Close();
                conn.Close();
            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace + "\n\n" + ae.Data);
            }
            catch (Exception ex)
            {
                //throw new Exception(ex.Message);  
                MessageBox.Show(ex.Message + " \n\n" + ex.Source + "\n\n" + ex.StackTrace + "\n\n" + ex.Data);
            }

            return dt;
        }

        // Get PlugRTData by PlugID and DateTime
        public static DataTable GetALLPlugRTDataByPlugIDandDateTime(Int64 plugid, DateTime startDateTime, DateTime endDateTime, string dbPath, ref bool success)
        {
            DataTable dt = new DataTable();
            success = false;
            try
            {
                SQLiteConnection conn = new SQLiteConnection("Data Source=" + dbPath + ";");
                conn.Open();
                SQLiteCommand cmd = new SQLiteCommand(conn);
                cmd.CommandText = "SELECT * FROM PlugRTData where PlugID=@_PlugID AND datetime(DateTime)>=datetime(@_startDateTime) AND datetime(DateTime)<=datetime(@_endDateTime);";
                cmd.Parameters.Add(new SQLiteParameter("_PlugID", plugid));
                cmd.Parameters.Add(new SQLiteParameter("_startDateTime", startDateTime));
                cmd.Parameters.Add(new SQLiteParameter("_endDateTime", endDateTime));
                cmd.CommandType = CommandType.Text;
                SQLiteDataReader dr = cmd.ExecuteReader();
                if (dr.HasRows)
                {
                    dt.Load(dr);
                    success = true;
                }
                else
                {

                }

                dr.Close();
                conn.Close();
            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace + "\n\n" + ae.Data);
            }
            catch (Exception ex)
            {
                //throw new Exception(ex.Message);  
                MessageBox.Show(ex.Message + " \n\n" + ex.Source + "\n\n" + ex.StackTrace + "\n\n" + ex.Data);
            }

            return dt;
        }

        // Get Selected PlugRTData by PlugID and DateTime
        public static DataTable GetSelectedPlugRTDataByPlugIDandDateTime(Int64 plugid, DateTime startDateTime, DateTime endDateTime, string itemstr, string dbPath, ref bool success)
        {
            DataTable dt = new DataTable();
            success = false;
            try
            {
                SQLiteConnection conn = new SQLiteConnection("Data Source=" + dbPath + ";");
                conn.Open();
                SQLiteCommand cmd = new SQLiteCommand(conn);
                cmd.CommandText = "SELECT " + itemstr + " FROM PlugRTData where PlugID=@_PlugID AND datetime(DateTime)>=datetime(@_startDateTime) AND datetime(DateTime)<=datetime(@_endDateTime);";
                cmd.Parameters.Add(new SQLiteParameter("_PlugID", plugid));
                cmd.Parameters.Add(new SQLiteParameter("_startDateTime", startDateTime));
                cmd.Parameters.Add(new SQLiteParameter("_endDateTime", endDateTime));
                cmd.CommandType = CommandType.Text;
                SQLiteDataReader dr = cmd.ExecuteReader();
                if (dr.HasRows)
                {
                    dt.Load(dr);
                    success = true;
                }
                else
                {

                }

                dr.Close();
                conn.Close();
            }
            catch (ArgumentException ae)
            {
                MessageBox.Show(ae.Message + " \n\n" + ae.Source + "\n\n" + ae.StackTrace + "\n\n" + ae.Data);
            }
            catch (Exception ex)
            {
                //throw new Exception(ex.Message);  
                MessageBox.Show(ex.Message + " \n\n" + ex.Source + "\n\n" + ex.StackTrace + "\n\n" + ex.Data);
            }

            return dt;
        }
    }


}
