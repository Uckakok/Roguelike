using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;
using System.IO;
using System.Windows.Threading;

namespace TestXaml
{
    public partial class MainWindow : Window
    {
        private readonly HoverCallback HoverCallbackDelegate;
        private readonly ShowUseCallback ShowUseCallbackDelegate;
        private readonly LoggerCallback LoggerCallbackDelegate;

        private IntPtr OtherWindow;
        private IntPtr ThisWindow;

        [DllImport("winmm.dll", SetLastError = true, CallingConvention = CallingConvention.Winapi)]
        static extern uint timeSetEvent(uint uDelay, uint uResolution, TimerCallback lpTimeProc, IntPtr dwUser, uint fuEvent);

        [DllImport("winmm.dll", SetLastError = true, CallingConvention = CallingConvention.Winapi)]
        static extern uint timeKillEvent(uint uTimerID);

        private delegate void TimerCallback(uint uTimerID, uint uMsg, ref IntPtr dwUser, ref IntPtr dw1, ref IntPtr dw2);

        private const uint TimerResolution = 1;

        // 16 ms for 60 FPS
        private const uint TimerInterval = 16;

        private uint TimerId;

        private TimerCallback TimerCallbackDelegate;

        public delegate void HoverCallback([MarshalAs(UnmanagedType.BStr)] string Name, int CurrentHP, int MaxHP);

        public delegate void LoggerCallback([MarshalAs(UnmanagedType.BStr)] string Name);

        public delegate void WindowHandleCallback(IntPtr windowHandle);

        public delegate void ShowUseCallback(bool bShow);

        bool bShouldUse = false;
        private bool bIsGameTickRunning = false;

        private int FrameCount = 0;
        private DateTime LastCheckTime = DateTime.Now;
        private int Fps = 0;

        public MainWindow()
        {
            InitializeComponent();


            HoverCallbackDelegate = new HoverCallback(HoverCallbackFunction);
            ShowUseCallbackDelegate = new ShowUseCallback(ShowUseCallbackFunction);
            LoggerCallbackDelegate = new LoggerCallback(LoggerCallbackFunction);
            LoadLanguagesFromLocalizationDirectory();
        }

        private void LoggerCallbackFunction(string NewLog)
        {
            Console.WriteLine(NewLog); 
            AddLogEvent(NewLog);
        }

        private void LoadLanguagesFromLocalizationDirectory()
        {
            string localizationDirectory = "Resources/Localization";
            if (Directory.Exists(localizationDirectory))
            {
                string[] poFiles = Directory.GetFiles(localizationDirectory, "*.po");
                foreach (string poFile in poFiles)
                {
                    string language = Path.GetFileNameWithoutExtension(poFile);
                    if (language == "en")
                    {
                        AddLanguage(language, true); // Set as default selection
                    }
                    else
                    {
                        AddLanguage(language);
                    }
                }
            }
            else
            {
                System.Windows.MessageBox.Show("Localization directory not found.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void AddLanguage(string language, bool isDefault = false)
        {
            LanguageComboBox.Items.Add(language);

            if (isDefault)
            {
                LanguageComboBox.SelectedItem = language;
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            InitializeOpenGL();
            Closing += Window_Closing;
            UseButton.Click += UseButton_Click;

            TimerCallbackDelegate = TimerCallbackFunction;

            // Create the multimedia timer
            TimerId = timeSetEvent(TimerInterval, TimerResolution, TimerCallbackDelegate, IntPtr.Zero, 1);
            if (TimerId == 0)
            {
                System.Windows.MessageBox.Show("Failed to create multimedia timer", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Close();
            }
        }

        private void TimerCallbackFunction(uint uTimerID, uint uMsg, ref IntPtr dwUser, ref IntPtr dw1, ref IntPtr dw2)
        {
            if (!bIsGameTickRunning)
            {
                bIsGameTickRunning = true;
                // Call the GameTick function
                Dispatcher.Invoke(() =>
                {
                    if (bShouldUse)
                    {
                        bShouldUse = false;
                        UseActivated();
                    }
                    GameTick(Marshal.GetFunctionPointerForDelegate(HoverCallbackDelegate), Marshal.GetFunctionPointerForDelegate(ShowUseCallbackDelegate), Marshal.GetFunctionPointerForDelegate(LoggerCallbackDelegate));

                    FrameCount++;
                    var currentTime = DateTime.Now;
                    var elapsedTime = currentTime - LastCheckTime;
                    if (elapsedTime.TotalSeconds >= 1)
                    {
                        Fps = (int)(FrameCount / elapsedTime.TotalSeconds);
                        FrameCount = 0;
                        LastCheckTime = currentTime;
                        FpsText.Text = "FPS: " + Fps.ToString();
                    }
                });
                bIsGameTickRunning = false;
            }
        }

        private void AddLogEvent(string logEvent)
        {
            LogTextBox.AppendText(Environment.NewLine + logEvent);
            LogTextBox.ScrollToEnd();
            LogScroller.ScrollToBottom();
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Dispatcher.Invoke(() =>
            {
                if (TimerId != 0)
                {
                    timeKillEvent(TimerId);
                    TimerId = 0;
                }

                SavePostExit();
                Environment.Exit(0);
            });
        }

        private void InitializeOpenGL()
        {
            InitializeGame(Marshal.GetFunctionPointerForDelegate(new WindowHandleCallback(WindowCallback)));
        }

        private void ShowUseCallbackFunction(bool bShow)
        {
            Dispatcher.Invoke(() =>
            {
                UseButton.IsEnabled = bShow;
                UseText.Foreground = bShow ? Brushes.Black : Brushes.Gray;
            });
        }

        private void LanguageComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            Dispatcher.Invoke(() =>
            {
                System.Windows.Controls.ComboBox comboBox = (System.Windows.Controls.ComboBox)sender;
                string selectedLanguage = (string)comboBox.SelectedItem;
                //change language in GameEngine
                ChangeLanguage(selectedLanguage);
                //load all translations
                LoadTranslations();
            });
        }

        private void LoadTranslations()
        {
            string translationKey = "use_text";
            IntPtr translationPtr = GetTranslation(translationKey);

            // Convert the IntPtr to a managed string directly
            string translation = Marshal.PtrToStringBSTR(translationPtr);
            Marshal.FreeBSTR(translationPtr);

            UseText.Text = translation;
        }

        private void UseButton_Click(object sender, RoutedEventArgs e)
        {
            Dispatcher.Invoke(() =>
            {
                bShouldUse = true; 
            });
        }

        private void HoverCallbackFunction(string Name, int CurrentHP, int MaxHP)
        {
            Dispatcher.Invoke(() =>
            {
                if (CurrentHP == 0 && MaxHP == 0)
                {
                    HoverInfoText.Text = "";
                }
                else
                {
                    string newText = Name;
                    if (CurrentHP != 0)
                    {
                        newText += "\nHP: " + CurrentHP + "/" + MaxHP;
                    }

                    HoverInfoText.Text = newText;
                }
            });
        }

        // Import native methods
        [DllImport("Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SavePostExit();
        [DllImport("Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void UseActivated();
        [DllImport("Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void InitializeGame(IntPtr windowHandleCallback);

        [DllImport("Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void GameTick(IntPtr hoverCallback, IntPtr useCallback, IntPtr loggerCallback);
        [DllImport("Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr GetTranslation(string Key);
        [DllImport("Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void ChangeLanguage(string Language);

        // Callback method to receive the window handle from native code
        private void WindowCallback(IntPtr windowHandle)
        {
            OtherWindow = windowHandle;

            ThisWindow = new WindowInteropHelper(this).Handle;

            WinHelper.SetWindowLong(OtherWindow, WinHelper.GWL_STYLE, WinHelper.winStyle.WS_VISIBLE | WinHelper.winStyle.WS_CHILD);
            WinHelper.SetParent(OtherWindow, ThisWindow);
            ArrangeWindows();
        }

        private void ArrangeWindows()
        {
            //Moves the otherWindow on top of childPlaceHolder
            Point topLeft = childPlaceholder.TransformToAncestor(this).Transform(new Point(0, 0));
            Point bottomRight = childPlaceholder.TransformToAncestor(this).Transform(new Point(childPlaceholder.ActualWidth, childPlaceholder.ActualHeight));
            WinHelper.MoveWindow(OtherWindow, (int)topLeft.X, (int)topLeft.Y, (int)bottomRight.X - (int)topLeft.X, (int)bottomRight.Y - (int)topLeft.Y, true);

            WinHelper.SendWindowToBack(OtherWindow);
        }
    }

    class WinHelper
    {
        //Sets a window to be a child window of another window
        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int X, int Y, int cx, int cy, uint uFlags);

        [DllImport("user32.dll")]
        public static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

        [DllImport("user32.dll")]
        public static extern bool MoveWindow(IntPtr hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint);

        //Sets window attributes
        [DllImport("user32.dll")]
        public static extern int SetWindowLong(IntPtr hWnd, int nIndex, winStyle dwNewLong);

        public static int GWL_STYLE = -16;

        public static void SendWindowToBack(IntPtr windowHandle)
        {
            // Call SetWindowPos to send the window to the bottom of the Z-order
            SetWindowPos(windowHandle, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }

        private static readonly IntPtr HWND_BOTTOM = new IntPtr(1);
        private const uint SWP_NOMOVE = 0x0002;
        private const uint SWP_NOSIZE = 0x0001;
        private const uint SWP_NOZORDER = 0x0004;
        private const uint SWP_SHOWWINDOW = 0x0040;

        [Flags]
        public enum winStyle : int
        {
            WS_VISIBLE = 0x10000000,
            WS_CHILD = 0x40000000, //child window
            WS_BORDER = 0x00800000, //window with border
            WS_DLGFRAME = 0x00400000, //window with double border but no title
            WS_CAPTION = WS_BORDER | WS_DLGFRAME //window with a title bar
        }
    }
}
