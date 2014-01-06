
#pragma once

#include <map>
#include <vector>
#include <string>


class xWidget
{
public:
  HWND hwnd;
public:
  xWidget(void);
  void setText(std::string str);
  void setPos(int x, int y, int w, int h);
};//xwidget

  
xWidget::xWidget(void) { hwnd = 0; }
   
void 
xWidget::setText(std::string str)
  {
     if (hwnd == 0) { return; }  
      std::wstring wstr;
      wstr.assign(str.begin(), str.end() );
      //LPCWSTR s = wstr.c_str();
      //LPSTR s = const_cast<char *>(str.c_str());
        //crashes for some reason
      //SetWindowText(hwnd, wstr.c_str());
     //SetWindowTextW(hwnd, L"MyText");
  }//settext

void 
xWidget::setPos(int x, int y, int w, int h)
  {
    if (hwnd == 0) { return; } 
     MoveWindow(hwnd, x,y, w, h, true);
  }//setpos


  
  
  
class xMiniGui
{
public:
  HWND pwnd;
  std::wstring wclass;
  std::vector <xWidget *> vecWidget;
public:
  xMiniGui(void);
  ~xMiniGui(void);
  void clear(void);

  void init(std::wstring cname, std::wstring title, int wx = 50, int wy =50, int ww = 320, int wh = 240, HWND parent = 0); 
  
  xWidget * makeButton(std::wstring caption, int id, int x, int y, int w=100, int h=24);
  xWidget * makeLabel(std::wstring caption, int id, int x, int y, int w=100, int h=24);
  
  
public:
  virtual void gotCommand(int id, int event) {}
  virtual void winCreated(void) {}
  virtual LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
  static std::map <HWND, xMiniGui *> sMapGui;
  static LRESULT CALLBACK s_wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};//classend


std::map <HWND, xMiniGui *> 
xMiniGui::sMapGui;

LRESULT CALLBACK 
xMiniGui::s_wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
  {

    xMiniGui * gui;
    gui = sMapGui[hWnd];
    if (gui != 0) { return gui->wndProc(hWnd, message, wParam, lParam); }

	  return DefWindowProc(hWnd, message, wParam, lParam);
  }//s_wndproc


 
xMiniGui::xMiniGui(void) { pwnd = 0; }
xMiniGui::~xMiniGui(void) { clear();    /* slog("mini dtor \n"); */ }

void 
xMiniGui::clear(void)
  {
    if (pwnd != 0) {  
      sMapGui.erase(pwnd); 
      DestroyWindow(pwnd);
      UnregisterClass(wclass.c_str(), GetModuleHandle(NULL));
    
      //all our widgets are destroyed in destroywindow
      //so also destroy all widget objects
      std::vector <xWidget *> ::iterator it;
      for (it = vecWidget.begin(); it != vecWidget.end(); it++)
      { (*it)->hwnd = 0; delete (*it); }

      vecWidget.clear();   
    }//endif     
    pwnd = 0;
  }//clear
  
  
  
xWidget *    
xMiniGui::makeButton(std::wstring caption, int id, int x, int y, int w, int h)
  {
    xWidget * a;

    if (pwnd == 0) { return 0 ;}

    a = new xWidget();
    vecWidget.push_back(a);

      a->hwnd = CreateWindowEx(NULL, L"BUTTON",		caption.c_str(),		WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		  x,		y,		w,		h,		pwnd,		(HMENU)id,		GetModuleHandle(NULL),		NULL);

    return a;
  }//makebutton


xWidget * 
xMiniGui::makeLabel(std::wstring caption, int id, int x, int y, int w, int h)
  {
      xWidget * a;

    if (pwnd == 0) { return 0 ;}

    a = new xWidget();
    vecWidget.push_back(a);

      a->hwnd = CreateWindowEx(NULL, L"STATIC",		caption.c_str(),		WS_VISIBLE|WS_CHILD,
		  x,		y,		w,		h,		pwnd,		(HMENU)id,		GetModuleHandle(NULL),		NULL);

    return 0;
  }//makelabel

  
  
  
void 
xMiniGui::init(std::wstring cname, std::wstring title, int wx, int wy, int ww, int wh, HWND parent) 
  {
    if (pwnd != 0) { return; }

    wclass = cname;
    	
    //WNDCLASSEX wcex;
    WNDCLASS wc;
    wc.style         = CS_HREDRAW | CS_VREDRAW; // | CS_OWNDC;
    wc.lpfnWndProc   = s_wndproc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground =  (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = cname.c_str();
      
       RegisterClass(&wc); 


   // pwnd = CreateWindow(cname.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW,  
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, 	GetModuleHandle(NULL), NULL);
   
    pwnd = CreateWindow(cname.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW,  
      wx, wy,ww, wh, parent, NULL, 	GetModuleHandle(NULL), NULL);
   

    ShowWindow(pwnd, SW_SHOW);
    UpdateWindow(pwnd);
    InvalidateRect(pwnd, NULL, TRUE);

    sMapGui[pwnd] = this;

    winCreated();
  }//init

  
 
LRESULT CALLBACK 
xMiniGui::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
  {
     
    switch (message)
    {
      //case WM_CREATE:   break;
      case WM_DESTROY:  clear();   break;
      case WM_COMMAND:  gotCommand(LOWORD(wParam), HIWORD(wParam) );    break;
    };//swend
    return DefWindowProc(hWnd, message, wParam, lParam);
  }//wndproc  
  
  
  
  