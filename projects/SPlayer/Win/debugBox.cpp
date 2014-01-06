#include "debugBox.h"

#include <windows.h>
#include <Winuser.h>

#include <map>
#include <vector>
#include <string>

#include "libxplayer.h"

#include "xMiniGui.h"

class DebugGui : public xMiniGui
{
public:
  int slotId;
 // xWidget * wStatus;
 // xWidget * wStatLine;
 // xWidget * wUrl;
 std::wstring debugStr;
  
public:
 DebugGui(void) 
 {
  slotId = 0; 
  //wStatus = 0;
  //wStatLine = 0;
  //wUrl = 0;

 }//ctor
 
 virtual void gotCommand(int id, int event);
 virtual void winCreated(void);
 virtual LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

 void update(void);

};//classend

 void 
 DebugGui::winCreated(void)
 {
   //slog("debuggui wincreated  \n");

 //wStatus = makeLabel(L"Status",3300, 40,32, 520, 24);
// wUrl = makeLabel(L"Url",3301, 16,40+32, 520, 48);
 //wStatLine = makeLabel(L"Statusline",3302, 16,40+32+64, 520, 92);

  makeButton(L"Play", 3200, 16, 120, 64, 24);
  makeButton(L"Pause", 3201, 16+70, 120, 64, 24);
  makeButton(L"Stop", 3202, 16+70+70, 120, 64, 24);
  makeButton(L"Update", 3203, 16+70+70+70, 120, 64, 24);
 
   update();
 }//wincreated

 
void
DebugGui::update(void)
{
    std::string str;
    std::string temp;
   
    temp = "";

    char* line =  xplayer_API_getstatusline(slotId);
    if (line)
    {
     str = line;
     xplayer_API_freestatusline(line);
    }//endif2

    temp += str;

    str = xplayer_API_geturl(slotId);

    temp += "\nUrl: ";
    temp += str;

     str = "";
     int stat;
     stat = xplayer_API_getstatus(slotId);   

    temp += "\nStatus: ";
   
     if ((stat & STATUS_PLAYER_STARTED) > 0)     { str.append(" Started "); }
     if ((stat & STATUS_PLAYER_INITED) > 0)     { str.append(" Inited "); }
     if ((stat & STATUS_PLAYER_CONNECT) > 0)     { str.append(" Connected "); }
     if ((stat & STATUS_PLAYER_OPENED) > 0)     { str.append(" Opened "); }
     if ((stat & STATUS_PLAYER_ERROR) > 0)     { str.append(" Error "); }
     if ((stat & STATUS_WAIT_FOR_FREE_IMAGE) > 0)     { str.append(" Wait_for_free "); }
     if ((stat & STATUS_PLAYER_PAUSE) > 0)     { str.append(" Paused "); }
     if ((stat & STATUS_PLAYER_PAUSE_IMG) > 0)     { str.append(" Pause_img "); }
     if ((stat & STATUS_PLAYER_SEEK) > 0)     { str.append(" Seek "); }

     temp += str;
        
     debugStr.assign(temp.begin(), temp.end() );


     InvalidateRect(pwnd, NULL, TRUE);
}//update


void 
DebugGui::gotCommand(int id, int event)
{
    //play
  if (id == 3200)
  { xplayer_API_play(slotId); return;}
  
    //pause
  if (id == 3201)
  {   xplayer_API_pause(slotId); return; }

    //stop
  if (id == 3202)
  { xplayer_API_stop(slotId); return; }
 
    //update
  if (id == 3203)
  { 
    update();
    return; 
  }
 
}//gotcommand


static std::string makeString(const char* str, ...)
{
	static char buffer[512];
  std::string ret;

	va_list params;
	va_start(params, str);
		_vsnprintf(buffer, sizeof(buffer), str, params);
	va_end(params);

  ret = buffer;

  return ret;
}//print


static std::map <int , DebugGui *> mapDebugGui;


void makeDebugBox(HWND player, int slotId)
{
  if (player == 0) { return; }

  DebugGui * a;
  a = mapDebugGui[slotId];
  if (a != 0) { delete a; }
  
  a = new DebugGui();
  
  a->slotId = slotId;
  mapDebugGui[slotId] = a;
  
  std::string cname;
  cname = makeString("debClass_%d",slotId);
  std::wstring wcname;
  wcname.assign(cname.begin(), cname.end());  

  a->init(wcname, L"Debug", 50, 50, 640, 280, player);
  
  
}//makedebbox




void destroyDebugBox(int slotId)
{
 DebugGui * a;
  a = mapDebugGui[slotId];
  if (a != 0) { delete a; }
  mapDebugGui.erase(slotId); 
}//destdebugbox

 

LRESULT CALLBACK 
DebugGui::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
  {
    
	//TCHAR szHello[MAX_LOADSTRING];
	//LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

    switch (message)
    {
      //case WM_CREATE:   break;
      case WM_DESTROY:  clear();   break;
      case WM_COMMAND:  gotCommand(LOWORD(wParam), HIWORD(wParam) );    break;

     case WM_PAINT:
            PAINTSTRUCT ps;
	        HDC hdc;
            RECT rt;
            
            //std::string myStr;
          //  myStr = "Hello World";
//            LPSTR s = const_cast<char *>(myStr.c_str());

            std::wstring wstr;
            //wstr = L"Hello World";
            wstr = debugStr;
            LPCWSTR s = wstr.c_str();

			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			    GetClientRect(hWnd, &rt);
			    //DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
                DrawText(hdc, s, wstr.size(), &rt, DT_LEFT|DT_WORDBREAK);
			EndPaint(hWnd, &ps);
			break;

    };//swend
    return DefWindowProc(hWnd, message, wParam, lParam);
  }//wndproc  
  
  
  
  