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
	
    //Image Load
    ofImage Glossy;
    
    //Syphon junk
    ofTexture tex;
	ofxSyphonServer mainOutputSyphonServer;
	ofxSyphonServer individualTextureSyphonServer;
	ofxSyphonClient mClient;

    //Camera input
    ofVideoGrabber          vidGrabber;
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
    
    //How many people to track
    int                 trackNum;
    
    //Centroid points mapped to fit fullscreen
    int                 mapCentX;
    int                 mapCentY;
    int                 mapPrevCentX;
    int                 mapPrevCentY;
    
    //Connections
    int                 connectDist[100][100];
    int                 blobConnect[100];
    int                 bigShot;
    int                 connectCount;
    
    //Style
    float               linethick;
    ofTrueTypeFont      Bauer;
    
    //Misc
    int                 tog;
    

};

#endif

