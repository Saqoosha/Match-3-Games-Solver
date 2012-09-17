#include <Cocoa/Cocoa.h>
#include <AppKit/NSOpenGL.h>

void setAlwaysOnTop() {
	NSOpenGLContext *context = [NSOpenGLContext currentContext];
  NSView *view = [context view];
  NSWindow *window = [view window];
  [window setLevel:NSFloatingWindowLevel];
}
