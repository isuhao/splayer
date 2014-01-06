
#import "aboutBoxMac.h"


#include <Cocoa/Cocoa.h>

#include <string>

#include "../Version.h"



@interface MyClass : NSObject

@property (strong) NSWindow * mwin;
@property (strong) NSMenu * theMenu;
@property (strong) NSButton * bn;


- (void) openBox;
- (void) closeBox;
- (void) createMenu;

@end


__strong static MyClass * abox = 0;


@implementation MyClass

@synthesize mwin;
@synthesize theMenu;
@synthesize bn;


- (void) closeBox
{
    [NSApp abortModal];

}

- (void) closeClicked:(NSNotification *)notification
{
 
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [NSApp abortModal];

    NSLog(@"Closed");
    
}


- (void) openBox
{
    NSRect frame = NSMakeRect(0, 0, 300, 140);
    mwin  = [[[[NSWindow alloc] initWithContentRect:frame
                                         styleMask:NSClosableWindowMask |NSTitledWindowMask
                                           backing:NSBackingStoreBuffered
                                             defer:NO]retain] autorelease];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(closeClicked:)
                                                 name:NSWindowWillCloseNotification
                                               object:mwin];
 
    /*bn = [[[NSButton alloc] initWithFrame:NSMakeRect(10, 10, 100, 20) ] autorelease];
    
    [bn setButtonType:NSMomentaryPushInButton];
    [bn setTitle:@"Ok"];
    [bn setTarget:self];
    [bn setAction:@selector(closeBox)];
    
    [[mwin contentView] addSubview:bn];
*/
    
    NSBundle *splayerBundle = [NSBundle bundleWithIdentifier: @"com.FireBreath.SPlayer"];
    NSImage *image = [splayerBundle imageForResource: @"caelogo.png"]; //do not release or autorelease
    
    NSImageView * iview = [[[NSImageView alloc] initWithFrame:NSMakeRect(10,20,140,100)] autorelease] ;
    
    [iview setImage:image];
    
    [[mwin contentView] addSubview:iview];
    
    NSTextField * field;
    field = [[[NSTextField alloc] initWithFrame:NSMakeRect(160, 20, 100, 60)]autorelease];
    [field setSelectable:false];
    [field setEditable:false];
    [field setBezeled:false];
    [field setDrawsBackground:false];
    
    std::string aboutStr;
    std::string verStr;
    verStr = (SPLAYER_VERSION);
    aboutStr = "SPlayer " + verStr + " by CAE 2013";
    [field setStringValue:[NSString stringWithUTF8String:aboutStr.c_str()]];
    [[mwin contentView] addSubview:field];
    
    [NSApp runModalForWindow:mwin];
}//openbox


- (void) createMenu
{
    NSMenuItem * item;
    
    theMenu = [[[NSMenu alloc] initWithTitle:@"Contextual Menu"] autorelease];
    [theMenu insertItemWithTitle:@"Learningspace SPlayer" action:@selector(nope:) keyEquivalent:@"" atIndex:0];
    [theMenu insertItem:[NSMenuItem separatorItem] atIndex:1];
    item = [theMenu insertItemWithTitle:@"About" action:@selector(openBox)                   keyEquivalent:@"" atIndex:2 ];
    
    [item setTarget:self ];
    
    [theMenu popUpMenuPositioningItem:nil atLocation:[NSEvent mouseLocation] inView:nil];

}//createmenu
@end


void createMenu(void)
{
        abox = 0;
        abox = [[MyClass alloc] autorelease];
    
        [abox createMenu];
    
}//createmenu
