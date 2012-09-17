#include "testApp.h"
#include "ofxCV.h"
extern "C" {
#include "macGlutfix.h"
#include "windowUtils.h"
}

#define SELECTED 0x67e835
#define WILDCARD_THRESHOLD 5


//--------------------------------------------------------------
void testApp::setup() {
//  ofSetLogLevel(OF_LOG_VERBOSE);
//  ofSetVerticalSync(true);
  ofSetFrameRate(10);
  ofSetWindowShape(400, 400);
  setAlwaysOnTop();
  
  ofDirectory dir;
  dir.listDir("nameko");
  for (int i = 0; i < dir.numFiles(); ++i) {
    ofFile file = dir[i];
    int x = strtol(file.getBaseName().c_str(), NULL, 16);
    ofImage *n = new ofImage();
    n->loadImage(file.getAbsolutePath());
    nameko.insert(make_pair(x, n));
  }
  
  verticalFrame.loadImage("vertical-frame.png");
  horizontalFrame.loadImage("horizontal-frame.png");
  
  mouseThread.startThread(true, false);
}


//--------------------------------------------------------------
void testApp::update() {
  if (origin.y > 0) {
    unsigned char *data = pixelsBelowWindow(origin.x, origin.y, 400, 400);
    cv::Mat argb(400, 400, CV_8UC4, data);
    cv::Mat rgb(400, 400, CV_8UC3);
    int fromTo[] = {1,0, 2,1, 3,2};
    mixChannels(&argb, 1, &rgb, 1, fromTo, 3);
    ofxCv::toOf(rgb, image);
    image.reloadTexture();
    
    getStateFromImage();
    findAnswer();
    
    if (selected.size()) {
      mouseThread.move(origin.x + (selected[0] + (ofRandom(1) < 0.5 ? -1 : 1)) * 50 + 23,
                       origin.y + selected[1] * 50 + 23);
      return;
    }
    
    if (wildcard.size()) {
        mouseThread.click(origin.x + wildcard[0] * 50 + 23, origin.y + wildcard[1] * 50 + 23);
        return;
//      int max_count = 0;
//      unsigned int max_id = 0;
//      for (map<unsigned int, int>::iterator it = count.begin(); it != count.end(); it++) {
//        if (it->second > max_count) {
//          max_count = it->second;
//          max_id = it->first;
//        }
//      }
//      if (max_count > 0) {
//        cout << "max_id: " << max_id << ", count: " << max_count << endl;
//        for (int i = 0; i < wildcard.size(); i += 2) {
//          int x = wildcard[i];
//          int y = wildcard[i + 1];
//          if (state[x][y].id == max_id) {
//            mouseThread.click(origin.x + x * 50 + 23, origin.y + y * 50 + 23);
//            return;
//          }
//        }
//      }
    }
    
    if (answers.size() > 0) {
      Answer ans = answers.at(ofRandom(MIN(answers.size(), 3)));
      mouseThread.drag(origin.x + ans.x1 * 50 + 23,
                       origin.y + ans.y1 * 50 + 23,
                       origin.x + ans.x2 * 50 + 23,
                       origin.y + ans.y2 * 50 + 23);
    }
  }
}


//--------------------------------------------------------------
void testApp::draw() {
  ofSetColor(64);
  ofRect(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
  
  ofEnableAlphaBlending();
  ofSetColor(255);

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      NamekoImages::iterator it = nameko.find(state[x][y].id);
      if (it != nameko.end()) {
        it->second->draw(x * 50, y * 50);
      }
    }
  }
  
  for (Answers::iterator it = answers.begin(); it != answers.end(); ++it) {
    if (it->x1 == it->x2) {
      verticalFrame.draw(it->x1 * 50, it->y1 * 50);
    } else {
      horizontalFrame.draw(it->x1 * 50, it->y1 * 50);
    }
  }
  
//  for (int y = 0; y < 8; y++) {
//    for (int x = 0; x < 8; x++) {
//      ofSetColor(0);
//      ofRect(x * 50, y * 50, 16, 16);
//      ofSetColor(255);
//      ofDrawBitmapString(ofToString(state[x][y].count), x * 50, y * 50 + 16);
//    }
//  }

//  ofSetColor(0, 0, 0, 128);
//  ofRect(0, 0, 100, 200);
//  ofSetColor(255);
//  int y = 16;
//  for (map<unsigned int, int>::iterator it = count.begin(); it != count.end(); it++) {
//    ofDrawBitmapString(ofToString(it->first) + ": " + ofToString(it->second), 0, y);
//    y += 20;
//  }
}


//--------------------------------------------------------------
void testApp::keyPressed(int key) {
  switch (key) {
    case ' ': {
      findOrigin();
      break;
    }
  }
}


//--------------------------------------------------------------
void testApp::keyReleased(int key) {
}


//--------------------------------------------------------------
void testApp::findOrigin() {
  int width = ofGetScreenWidth();
  int height = ofGetScreenHeight();
  unsigned char *data = pixelsBelowWindow(0, 0, width, height);
  ofImage banner;
  banner.loadImage("banner.png");
  float start = ofGetElapsedTimef();
  cv::Mat captured(height, width, CV_8UC4, data, 0);
  cv::Mat screen(height, width, CV_8UC3);
  int fromTo[] = {1,0, 2,1, 3,2};
  mixChannels(&captured, 1, &screen, 1, fromTo, 3);
  cv::Mat temp_img = ofxCv::toCv(banner);
  cv::Mat result_img;
  cv::matchTemplate(screen, temp_img, result_img, CV_TM_CCOEFF_NORMED);
  cv::Point max_pt;
  double maxVal;
  cv::minMaxLoc(result_img, NULL, &maxVal, NULL, &max_pt);
  cout << "(" << max_pt.x << ", " << max_pt.y << "), score=" << maxVal << ", time=" << (ofGetElapsedTimef() - start) << endl;
  if (maxVal > 0.9) {
    origin.x = max_pt.x + 20;
    origin.y = max_pt.y + 115 + 63;
    
#if 0
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        cv::Mat name = screen(cv::Rect(origin.x + x * 50, origin.y + y * 50, 46, 46)).clone();
        ofImage nameko;
        ofxCv::toOf(name, nameko);
        nameko.reloadTexture();
        char filename[256];
        sprintf(filename, "nameko/%06x.png", nameko.getColor(23, 2).getHex());
        ofFile file(filename);
        if (!file.exists()) {
          nameko.saveImage(file);
        }
      }
    }
#endif

    ofSetWindowPosition(origin.x - ofGetWindowWidth() - 25, origin.y - 1);
  }
}


//--------------------------------------------------------------
void testApp::getStateFromImage() {
  selected.clear();
  count.clear();
  
  unsigned int id = 0;
  for (int y = 7; y >= 0; --y) {
    for (int x = 0; x < 8; ++x) {
      id = 0;
      for (int i = 4; i >= 0; --i) {
        ofColor c = image.getPixelsRef().getColor(23 + x * 50, y * 50 + i);
        NamekoImages::iterator it = nameko.find(c.getHex());
        if (it != nameko.end()) {
          id = c.getHex();
          break;
        }
      }

      if (id > 0) {
        if (state[x][y].id == id) {
          state[x][y].count = 0;
        } else {
          state[x][y].count++;
        }
        state[x][y].id = id;
        if (id == SELECTED) {
          selected.push_back(x);
          selected.push_back(y);
        }
        count[id]++;
      } else {
        state[x][y].id = 0;
        state[x][y].count = 0;
      }

      if (state[x][y].id == 0) {
        for (int yy = y; yy < 8; ++yy) {
          if (state[x][yy].id != SELECTED) {
            state[x][yy].id = 0;
          }
          state[x][yy].count = 0;
        }
      }
    }
  }
}


//--------------------------------------------------------------
void testApp::findAnswer() {
  wildcard.clear();
  answers.clear();

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {

      if (state[x][y].count > WILDCARD_THRESHOLD) {
        wildcard.push_back(x);
        wildcard.push_back(y);
        continue;
      }
      
      if (x < 7) {
        int score = calcSwappedScore(x, y, x + 1, y);
        if (score) {
          Answer ans;
          ans.score = score;
          ans.x1 = x;
          ans.y1 = y;
          ans.x2 = x + 1;
          ans.y2 = y;
          answers.push_back(ans);
        }
      }
      
      if (y < 7) {
        int score = calcSwappedScore(x, y, x, y + 1);
        if (score) {
          Answer ans;
          ans.score = score;
          ans.x1 = x;
          ans.y1 = y;
          ans.x2 = x;
          ans.y2 = y + 1;
          answers.push_back(ans);
        }
      }
    }
  }
  
  sort(answers.begin(), answers.end(), AnswerSorter());
}


//--------------------------------------------------------------
int testApp::calcSwappedScore(int x1, int y1, int x2, int y2) {
  if (state[x1][y1].id == 0 ||
      state[x1][y1].count > WILDCARD_THRESHOLD ||
      state[x2][y2].id == 0 ||
      state[x2][y2].count > WILDCARD_THRESHOLD ||
      state[x1][y1].id == state[x2][y2].id) {
    return 0;
  }
  
  // swap
  State tmp = state[x1][y1];
  state[x1][y1] = state[x2][y2];
  state[x2][y2] = tmp;
  
  int score = 0;
  
  if (x1 == x2) { // swap vertical
    score = verticalScore(x1, MAX(y1 - 2, 0), y1)
            + horizontalScore(MAX(x1 - 2, 0), MIN(x1 + 2, 7), y1)
            + horizontalScore(MAX(x1 - 2, 0), MIN(x1 + 2, 7), y2)
            + verticalScore(x1, y2, MIN(y2 + 2, 7))
            + rectScore(MAX(x1 - 1, 0), MAX(y1 - 1, 0), x1, y1)
            + rectScore(x1, MAX(y1 - 1, 0), MIN(x1 + 1, 7), y1)
            + rectScore(MAX(x1 - 1, 0), y2, x1, MIN(y2 + 1, 7))
            + rectScore(x1, y2, MIN(x1 + 1, 7), MIN(y2 + 1, 7));
    
  } else { // swap horizontal
    score = horizontalScore(MAX(x1 - 2, 0), x1, y1)
            + verticalScore(x1, MAX(y1 - 2, 0), MIN(y1 + 2, 7))
            + verticalScore(x2, MAX(y1 - 2, 0), MIN(y1 + 2, 7))
            + horizontalScore(x2, MIN(x2 + 2, 7), y1)
            + rectScore(MAX(x1 - 1, 0), MAX(y1 - 1, 0), x1, y1)
            + rectScore(MAX(x1 - 1, 0), y1, x1, MIN(y1 + 1, 7))
            + rectScore(x2, MAX(y1 - 1, 0), MAX(x2 + 1, 7), y1)
            + rectScore(x2, y1, MIN(x2 + 1, 7), MIN(y1 + 1, 7));
  }

  // restore
  tmp = state[x1][y1];
  state[x1][y1] = state[x2][y2];
  state[x2][y2] = tmp;
  
  return score;
}


//--------------------------------------------------------------
int testApp::verticalScore(int x, int y1, int y2) {
  int max_score = 0;
  int score = 0;
  unsigned int prev = 0;

  for (int y = y1; y <= y2; y++) {
    if (state[x][y].id && prev == state[x][y].id) {
      score++;
    } else {
      if (score > max_score) {
        max_score = score + 1;
      }
      score = 0;
    }
    prev = state[x][y].id;
  }
  
  max_score = MAX(max_score, score + 1);
  return max_score < 3 ? 0 : max_score;
}


//--------------------------------------------------------------
int testApp::horizontalScore(int x1, int x2, int y) {
  int max_score = 0;
  int score = 0;
  unsigned int prev = 0;

  for (int x = x1; x <= x2; x++) {
    if (state[x][y].id && prev == state[x][y].id) {
      score++;
    } else {
      if (score > max_score) {
        max_score = score + 1;
      }
      score = 0;
    }
    prev = state[x][y].id;
  }
  
  max_score = MAX(max_score, score + 1);
  return max_score < 3 ? 0 : max_score;
}


//--------------------------------------------------------------
int testApp::rectScore(int x1, int y1, int x2, int y2) {
  if (x1 == x2 || y1 == y2) {
    return 0;
  }
  if (state[x1][y1].id > 0 &&
      state[x1][y1].id == state[x1][y2].id &&
      state[x1][y1].id == state[x2][y1].id &&
      state[x1][y1].id == state[x2][y2].id) {
    return 100;
  } else {
    return 0;
  }
}


