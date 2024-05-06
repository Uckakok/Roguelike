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

    public enum ItemId
    {
        Empty,
        SuperSword,
        SuperBow,
    }

    public partial class MainWindow : Window
    {
        private IntPtr otherWindow;

        private IntPtr thisWindow;

        // Define a delegate for the button state callback function
        public delegate void HotbarCallback(int Slot, int Id);

        // Define a delegate for the window handle callback function
        public delegate void WindowHandleCallback(IntPtr windowHandle);

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // Start rendering OpenGL content
            InitializeOpenGL();
        }

        private void InitializeOpenGL()
        {
            // Call native code to initialize OpenGL and pass the delegates
            InitializeGame(Marshal.GetFunctionPointerForDelegate(new HotbarCallback(HotbarCallbackFunction)), Marshal.GetFunctionPointerForDelegate(new WindowHandleCallback(WindowCallback)));
        }

        private string GetImagePath(ItemId id)
        {
            switch (id)
            {
                case ItemId.Empty:
                    return "empty.png"; // Example path for empty item
                case ItemId.SuperSword:
                    return "super_sword.png"; // Example path for super sword
                case ItemId.SuperBow:
                    return "super_bow.png"; // Example path for super bow
                default:
                    return null; // Return null if item ID not recognized
            }
        }

        private void SetButtonImage(System.Windows.Controls.Button button, string imagePath)
        {
            if (button != null && !string.IsNullOrEmpty(imagePath))
            {
                button.Content = new Image
                {
                    Source = new BitmapImage(new Uri(imagePath, UriKind.Relative))
                };
            }
        }

        private void HotbarCallbackFunction(int Slot, int Id)
        {
            //System.Windows.MessageBox.Show($"Item Id: {(ItemId)Id}, Slot: {Slot}");

            switch (Slot)
            {
                case 0:
                    SetButtonImage(btn0, GetImagePath((ItemId)(Id)));
                    break;
                case 1:
                    SetButtonImage(btn1, GetImagePath((ItemId)(Id)));
                    break;
                case 2:
                    SetButtonImage(btn2, GetImagePath((ItemId)(Id)));
                    break;
                case 3:
                    SetButtonImage(btn3, GetImagePath((ItemId)(Id)));
                    break;
                case 4:
                    SetButtonImage(btn4, GetImagePath((ItemId)(Id)));
                    break;
                case 5:
                    SetButtonImage(btn5, GetImagePath((ItemId)(Id)));
                    break;
                case 6:
                    SetButtonImage(btn6, GetImagePath((ItemId)(Id)));
                    break;
                case 7:
                    SetButtonImage(btn7, GetImagePath((ItemId)(Id)));
                    break;
                case 8:
                    SetButtonImage(btn8, GetImagePath((ItemId)(Id)));
                    break;
                default:
                    break;
            }
        }


        private void Button_Click(object sender, RoutedEventArgs e)
        {
            //empty
        }

        // Import native methods
        [DllImport("Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void InitializeGame(IntPtr hotbarCallback, IntPtr windowHandleCallback);

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