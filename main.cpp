#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <scrnsave.h>
#include <SFML/Graphics.hpp>

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("SFML and Win32 API");

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    //check the available video modes
    //documentation can be found here:
    //http://www.sfml-dev.org/documentation/2.1/classsf_1_1VideoMode.php
    std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
    for (std::size_t i = 0; i < modes.size(); ++i)
    {
        sf::VideoMode mode = modes[i];
        std::cout<<"Mode #"<<i<<": "
        <<mode.width<<"x"<<mode.height<<" - "
        <<mode.bitsPerPixel<<"bpp"<<std::endl;
    }

    HWND hwnd;               /* This is the handle for our window */
    MSG Message;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = ScreenSaverProc;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
    {
        std::cout<<"Class failed to register."<<std::endl;
        return 0;
    }

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                               /* Extended possibilites for variation */
               szClassName,                     /* Classname */
               _T("SFML and Win32 API"),        /* Title Text */
               WS_OVERLAPPEDWINDOW,             /* default window */
               CW_USEDEFAULT,                   /* Windows decides the position */
               CW_USEDEFAULT,                   /* where the window ends up on the screen */
               800,                             /* The programs width */
               600,                             /* and height in pixels */
               HWND_DESKTOP,                    /* The window is a child-window to desktop */
               NULL,                            /* No menu */
               hThisInstance,                   /* Program Instance handler */
               NULL                             /* No Window Creation data */
           );

    //pass the window handle to SFML
    sf::RenderWindow window(hwnd);

    //use sfml to create the screen in fullscreen mode
    //Create a window with the same width, height, and pixel depth as the desktop
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(sf::VideoMode(desktop.width,desktop.height,desktop.bitsPerPixel), "SFML and Win32 API", sf::Style::Fullscreen);
    //create variables for tracking mouse movements
    sf::Mouse::setPosition(sf::Vector2i(400,300), window);
    sf::Vector2i initpos = sf::Mouse::getPosition(window);
    sf::Vector2i movement;
    //fudge factor is to account for mouse sensitivity
    //this eliminates problems with the screensaver
    //exiting at the slightest nudge of the mouse
    sf::Vector2i fudgefactor = sf::Vector2i(4,4);

    //set the cursor inside the window to track movements
    window.setActive(true);

    //////////////////////////////
    //***************************
    //create some SFML drawables
    //***************************
    //////////////////////////////

    //first grab the location of the user's Windows path
    //where the fonts are located - hopefully they have arial.ttf
    //...it's a pretty standard font
    std::string str = getenv("WinDir");
    str += "\\fonts\\arial.ttf";
    std::cout<<str<<std::endl;

    //load the font
    sf::Font font;
    if(!font.loadFromFile(str))
    {
        std::cerr<<"Could not load the font."<<std::endl;
    }

    //create the screensaver message
    sf::Text words;
    words.setFont(font);
    words.setString("Hello Screensaver!");
    words.setOrigin(words.getGlobalBounds().width/2.f, words.getGlobalBounds().height/2.f);
    words.setPosition(window.getSize().x/2.f, window.getSize().y/2.f);

    //handle events in SFML - it's much cleaner
    sf::Event e;
    bool running = true;
    while(running)
    {
        while(window.pollEvent(e))
        {
            if(e.type == sf::Event::Closed)
            {
                //destroy window and unregister the class
                DestroyWindow(hwnd);
                UnregisterClass("SFML and Win32 API", hThisInstance);
                return 0;
            }

            if(e.type == sf::Event::KeyPressed)
            {
                //destroy window and unregister the class
                DestroyWindow(hwnd);
                UnregisterClass("SFML and Win32 API", hThisInstance);
                return 0;
            }

            if(e.type == sf::Event::MouseMoved)
            {
                movement = sf::Mouse::getPosition(window);
                //make sure to use the absolute value of the difference
                //between the initial mouse position and the movement
                //and compare it to the fudge factor
                //if this value is greater exit the screensaver
                if (abs(movement.x - initpos.x) > fudgefactor.x
                    || abs(movement.y - initpos.y) > fudgefactor.y)
                {
                    //destroy window and unregister the class
                    DestroyWindow(hwnd);
                    UnregisterClass("SFML and Win32 API", hThisInstance);
                    return 0;
                }
            }
        }

        //clear the window in SFML
        window.clear();

        //draw stuff
        window.draw(words);

        //display stuff
        window.display();
    }
    //just in case the app makes it to this point
    //destroy window and unregister the class
    DestroyWindow(hwnd);
    UnregisterClass("SFML and Win32 API", hThisInstance);

    return 0;
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK ScreenSaverProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //replaced messages with SFML events in main loop
    //but it is still necessary to return DefWindowProc
    return DefWindowProc (hwnd, message, wParam, lParam);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    return false;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return true;
}
