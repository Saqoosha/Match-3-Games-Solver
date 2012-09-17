//
//  mouseThread.cpp
//  NamecoRobo
//
//  Created by hiko on 2012/09/16.
//
//

#include "mouseThread.h"


void MouseThread::move(int x, int y) {
  lock();
  next.type = MOVE;
  next.x1 = x;
  next.y1 = y;
  unlock();
}


void MouseThread::click(int x, int y) {
  lock();
  next.type = CLICK;
  next.x1 = x;
  next.y1 = y;
  unlock();
}


void MouseThread::drag(int x1, int y1, int x2, int y2) {
  lock();
  next.type = DRAG;
  next.x1 = x1;
  next.y1 = y1;
  next.x2 = x2;
  next.y2 = y2;
  unlock();
}


void MouseThread::threadedFunction() {
  Action action;
  
  while (true) {
    action.type = NO_ACTION;

    lock();
    if (next.type != NO_ACTION) {
      action = next;
      next.type = NO_ACTION;
    }
    unlock();

    switch (action.type) {
      case NO_ACTION: {
        sleep(100);
        break;
      }
      case MOVE: {
        mouseMove(action.x1, action.y1);
        sleep(100);
        break;
      }
      case CLICK: {
        mouseDown(action.x1, action.y1);
        sleep(10);
        mouseUp(action.x1, action.y1);
        sleep(100);
        break;
      }
      case DRAG: {
        mouseDown(action.x1, action.y1);
        for (int i = 0; i < 10; ++i) {
          sleep(10);
          mouseDrag(action.x1 + (action.x2 - action.x1) * i / 10, action.y1 + (action.y2 - action.y1) * i / 10);
        }
        sleep(10);
        mouseUp(action.x2, action.y2);
        sleep(400);
        break;
      }
    }
  }
}


//--------------------------------------------------------------
void MouseThread::postEvent(CGEventType type, int x, int y, CGMouseButton button) {
  CGEventRef event = CGEventCreateMouseEvent(NULL, type, CGPointMake(x, y), button);
  CGEventSetType(event, type);
  CGEventPost(kCGHIDEventTap, event);
  CFRelease(event);
}


//--------------------------------------------------------------
void MouseThread::mouseMove(int x, int y) {
  postEvent(kCGEventMouseMoved, x, y);
}


//--------------------------------------------------------------
void MouseThread::mouseDown(int x, int y) {
  postEvent(kCGEventLeftMouseDown, x, y);
}


//--------------------------------------------------------------
void MouseThread::mouseUp(int x, int y) {
  postEvent(kCGEventLeftMouseUp, x, y);
}


//--------------------------------------------------------------
void MouseThread::mouseDrag(int x, int y) {
  postEvent(kCGEventLeftMouseDragged, x, y);
}
