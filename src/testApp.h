#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxOpenCv.h"

#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file

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

        #ifdef _USE_LIVE_VIDEO
		  ofVideoGrabber 		vidGrabber;
		#else
		  ofVideoPlayer 		vidPlayer;
		#endif

        ofxCvColorImage			colorImg;

        ofxCvGrayscaleImage 	grayImg;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;
        ofxCvContourFinder      contourFinder;
    
        ofxSyphonServer mainOutputSyphonServer;

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
    
        float               linethick;
    
        int                 tog;


};

