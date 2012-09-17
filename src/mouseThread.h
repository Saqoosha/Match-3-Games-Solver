//
//  mouseThread.h
//  NamecoRobo
//
//  Created by Saqoosha on 2012/09/16.
//
//

#pragma once

#include "ofMain.h"

enum MouseActionType {
  NO_ACTION = 0,
  MOVE,
  CLICK,
  DRAG,
};

typedef struct {
  MouseActionType type;
  int x1, y1, x2, y2;
} Action;


class MouseThread : public ofThread {
 public:
  void move(int x, int y);
  void click(int x, int y);
  void drag(int x1, int y1, int x2, int y2);
  
 protected:
  void threadedFunction();

  void postEvent(CGEventType type, int x, int y, CGMouseButton button = kCGMouseButtonLeft);
  void mouseDown(int x, int y);
  void mouseUp(int x, int y);
  void mouseMove(int x, int y);
  void mouseDrag(int x, int y);
  
  Action next;
};
