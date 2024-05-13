using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Forms;
using System.Windows.Forms.Integration;
using System.Windows.Media.Imaging;
using System.Windows.Threading;

namespace TestXaml
{
    public partial class MainWindow : Window
    {
        private readonly HoverCallback hoverCallbackDelegate;
        private readonly ShowUseCallback showUseCallbackDelegate;

        private IntPtr otherWindow;
        private IntPtr thisWindow;

        [DllImport("winmm.dll", SetLastError = true, CallingConvention = CallingConvention.Winapi)]
        static extern uint timeSetEvent(uint uDelay, uint uResolution, TimerCallback lpTimeProc, IntPtr dwUser, uint fuEvent);

        [DllImport("winmm.dll", SetLastError = true, CallingConvention = CallingConvention.Winapi)]
        static extern uint timeKillEvent(uint uTimerID);

        // Define the multimedia timer callback delegate
        private delegate void TimerCallback(uint uTimerID, uint uMsg, ref IntPtr dwUser, ref IntPtr dw1, ref IntPtr dw2);

        // Define the multimedia timer resolution (1 ms)
        private const uint TimerResolution = 1;

        // Define the multimedia timer interval (e.g., 16 ms for approximately 60 FPS)
        private const uint TimerInterval = 16;

        // Define the multimedia timer ID
        private uint timerId;

        private TimerCallback timerCallbackDelegate;

        // Define a delegate for the button state callback function
        public delegate void HoverCallback(IntPtr Name, int CurrentHP, int MaxHP);

        // Define a delegate for the window handle callback function
        public delegate void WindowHandleCallback(IntPtr windowHandle);

        public delegate void ShowUseCallback(bool bShow);

        bool bShouldUse = false;
        private bool isGameTickRunning = false;

        private int frameCount = 0;
        private DateTime lastCheckTime = DateTime.Now;
        private int fps = 0;

        public MainWindow()
        {
            InitializeComponent();
            
            
            hoverCallbackDelegate = new HoverCallback(HoverCallbackFunction);
            showUseCallbackDelegate = new ShowUseCallback(ShowUseCallbackFunction);
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            InitializeOpenGL();
            Closing += Window_Closing;
            UseButton.Click += UseButton_Click;

            timerCallbackDelegate = TimerCallbackFunction;

            // Create the multimedia timer
            timerId = timeSetEvent(TimerInterval, TimerResolution, timerCallbackDelegate, IntPtr.Zero, 1);
            if (timerId == 0)
            {
                System.Windows.MessageBox.Show("Failed to create multimedia timer", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Close();
            }
        }

        private void TimerCallbackFunction(uint uTimerID, uint uMsg, ref IntPtr dwUser, ref IntPtr dw1, ref IntPtr dw2)
        {
            if (!isGameTickRunning)
            {
                isGameTickRunning = true;
                // Call the GameTick function
                Dispatcher.Invoke(() =>
                {
                    if (bShouldUse)
                    {
                        bShouldUse = false;
                        UseActivated();
                    }
                    GameTick(Marshal.GetFunctionPointerForDelegate(hoverCallbackDelegate), Marshal.GetFunctionPointerForDelegate(showUseCallbackDelegate));

                    frameCount++;
                    var currentTime = DateTime.Now;
                    var elapsedTime = currentTime - lastCheckTime;
                    if (elapsedTime.TotalSeconds >= 1)
                    {
                        fps = (int)(frameCount / elapsedTime.TotalSeconds);
                        frameCount = 0;
                        lastCheckTime = currentTime;
                        FpsText.Text = "FPS: " + fps.ToString();
                    }
                });
                isGameTickRunning = false;
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Dispatcher.Invoke(() =>
            {
                if (timerId != 0)
                {
                    timeKillEvent(timerId);
                    timerId = 0;
                }

                Environment.Exit(0); // Exit the program when the window is closing
            });
        }

        private void InitializeOpenGL()
        {
            // Call native code to initialize OpenGL and pass the delegate
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

        private void UseButton_Click(object sender, RoutedEventArgs e)
        {
            Dispatcher.Invoke(() =>
            {
                bShouldUse = true; 
            });
        }

        private void HoverCallbackFunction(IntPtr Name, int CurrentHP, int MaxHP)
        {
            Dispatcher.Invoke(() =>
            {
                if (CurrentHP == 0 && MaxHP == 0)
                {
                    HoverInfoText.Text = "";
                }
                else
                {
                    // Marshal the pointer to a C# string
                    string nameString = Marshal.PtrToStringAnsi(Name);

                    // Construct the new text
                    string newText = nameString;
                    if (CurrentHP != 0)
                    {
                        newText += "\nHP: " + CurrentHP + "/" + MaxHP;
                    }

                    // Set the text in HoverInfoText
                    HoverInfoText.Text = newText;
                }
            });
        }

        // Import native methods
        [DllImport("Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void UseActivated();
        [DllImport("Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void InitializeGame(IntPtr windowHandleCallback);

        [DllImport("Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void GameTick(IntPtr hoverCallback, IntPtr useCallback);

        // Callback method to receive the window handle from native code
        private void WindowCallback(IntPtr windowHandle)
        {
            otherWindow = windowHandle;

            thisWindow = new WindowInteropHelper(this).Handle;

            //This bit removes the border of otherWindow and sets thisWindow as parent
            //I actually don't know what flags should be set, but simply setting the WS_VISIBLE flag seems to make window work, however borderless.
            WinHelper.SetWindowLong(otherWindow, WinHelper.GWL_STYLE, WinHelper.winStyle.WS_VISIBLE | WinHelper.winStyle.WS_CHILD);
            WinHelper.SetParent(otherWindow, thisWindow);
            ArrangeWindows();
        }

        private void ArrangeWindows()
        {
            //Moves the otherWindow on top of childPlaceHolder
            Point topLeft = childPlaceholder.TransformToAncestor(this).Transform(new Point(0, 0));
            Point bottomRight = childPlaceholder.TransformToAncestor(this).Transform(new Point(childPlaceholder.ActualWidth, childPlaceholder.ActualHeight));
            WinHelper.MoveWindow(otherWindow, (int)topLeft.X, (int)topLeft.Y, (int)bottomRight.X - (int)topLeft.X, (int)bottomRight.Y - (int)topLeft.Y, true);

            WinHelper.SendWindowToBack(otherWindow);
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
