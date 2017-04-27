#import <Cocoa/Cocoa.h>

void enableTransparentTitleBar(NSView* view) {
    NSWindow* window = [view window];
    window.titlebarAppearsTransparent = true;
    window.titleVisibility = NSWindowTitleVisibility::NSWindowTitleHidden;
    window.styleMask |= NSFullSizeContentViewWindowMask;
}
