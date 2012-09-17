#pragma once

#include "ofMain.h"
#include "mouseThread.h"


typedef struct {
  int score;
  int x1, y1, x2, y2;
} Answer;
typedef deque<Answer> Answers;


typedef struct {
  unsigned int id;
  int count;
} State;


class testApp : public ofBaseApp {
 public:
  void setup();
  void update();
  void draw();

  void keyPressed(int key);
  void keyReleased(int key);

  void findOrigin();
  void getStateFromImage();
  void findAnswer();
  int calcSwappedScore(int x1, int y1, int x2, int y2);
  int verticalScore(int x, int y1, int y2);
  int horizontalScore(int x1, int x2, int y);
  int rectScore(int x1, int y1, int x2, int y2);
  
  ofPoint origin;
  ofImage image;
  State state[8][8];
  Answers answers;
  vector<int> selected;
  vector<int> wildcard;
  map<unsigned int, int> count;
  typedef map<int, ofImage*> NamekoImages;
  NamekoImages nameko;
  ofImage verticalFrame;
  ofImage horizontalFrame;
  
  MouseThread mouseThread;
};


class AnswerSorter {
 public:
  bool operator()(const Answer &a, const Answer &b) {
    return a.score > b.score;
  }
};

