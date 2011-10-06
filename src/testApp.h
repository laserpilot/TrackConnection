#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxOpenCv.h"

class testApp : public ofBaseApp{
	
public:
	
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);	
	
	float 	counter;
	bool	bSmooth;
	
    ofTexture tex;
    
	ofxSyphonServer mainOutputSyphonServer;
	ofxSyphonServer individualTextureSyphonServer;
	
	ofxSyphonClient mClient;
    
    ofVideoGrabber 		vidGrabber;

    
    ofxCvColorImage			colorImg;
    
    ofxCvGrayscaleImage 	grayImg;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvContourFinder      contourFinder;
    
    int 				threshold;
    bool				bLearnBakground;
    
    int                 keyPress;
    
    int                 camWidth;
    int                 camHeight;
    float               mappedX;
    float               mappedY;
    int                 ptAvg;
    int                 ptSum;
    
    int                 mapCentX;
    int                 mapCentY;
    int                 mapPrevCentX;
    int                 mapPrevCentY;
    
    int                 connectDist[20][20];
    
    float               linethick;
    
    int                 tog;
    

};

#endif

