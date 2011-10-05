#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
        
        ofSetVerticalSync(true);
        //ofEnableAlphaBlending();
        ofSetLogLevel(OF_LOG_VERBOSE);
        
        camWidth = 640;
        camHeight = 480;
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(camWidth,camHeight);
        
        colorImg.allocate(camWidth,camHeight);
        grayImg.allocate(camWidth,camHeight);
        grayBg.allocate(camWidth,camHeight);
        grayDiff.allocate(camWidth,camHeight);
        threshold = 120;
        
        ofBackground(0, 0, 0);
    
    ofSetFrameRate(60);
    
    mainOutputSyphonServer.setName("Screen Output");
    
        

	bLearnBakground = true;

}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(100,100,100);

    bool bNewFrame = false;

	#ifdef _USE_LIVE_VIDEO
       vidGrabber.grabFrame();
	   bNewFrame = vidGrabber.isFrameNew();
    #else
        vidPlayer.idleMovie();
        bNewFrame = vidPlayer.isFrameNew();
	#endif

	if (bNewFrame){

		#ifdef _USE_LIVE_VIDEO
            colorImg.setFromPixels(vidGrabber.getPixels(), camWidth,camHeight);
	    #else
            colorImg.setFromPixels(vidPlayer.getPixels(), camWidth,camHeight);
        #endif

        grayImg = colorImg;
		if (bLearnBakground == true){
			grayBg = grayImg;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImg);
		grayDiff.threshold(threshold);

		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayDiff, 20, (camWidth*camHeight)/3, 10, true);	// find holes
	}
    

}

//--------------------------------------------------------------
void testApp::draw(){

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	// draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
    //colorImg.draw(0,0,ofGetWidth(),0.75*ofGetWidth());
    
    for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
        ofNoFill();
        for( int j=0; j<contourFinder.blobs[i].nPts; j++ ) {
            mapCentX = ofMap(contourFinder.blobs[i].centroid.x,0,camWidth,0,ofGetWidth());
            mapCentY = ofMap(contourFinder.blobs[i].centroid.y,0,camHeight,0,.75*ofGetWidth());
            
            ofBeginShape();
            ofVertex(mapCentX, mapCentY);
            ofVertex(mapPrevCentX, mapPrevCentY);
            ofSetLineWidth(6);
            ofEndShape(); 
            ofFill();
            ofCircle(mapCentX, mapCentY, 10);
             
        }
        mapPrevCentX = ofMap(contourFinder.blobs[i].centroid.x,0,camWidth,0,ofGetWidth());
        mapPrevCentY = ofMap(contourFinder.blobs[i].centroid.y,0,camHeight,0,.75*ofGetWidth());
    }  
    
    ofDisableSmoothing();
    for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
        ofNoFill();
        ofBeginShape();
        for( int j=0; j<contourFinder.blobs[i].nPts; j++ ) {
            mappedX=ofMap(contourFinder.blobs[i].pts[j].x,0,camWidth,0,ofGetWidth());
            mappedY=ofMap(contourFinder.blobs[i].pts[j].y,0,camHeight,0,.75*ofGetWidth());
            ofSetLineWidth(1);
            ofVertex(mappedX, mappedY);
            
        }
        ofEndShape();
        
    }  
    ofDrawBitmapString("Threshold: " + ofToString(threshold), 20,20);
     
    
    mainOutputSyphonServer.publishScreen();
    
    
    
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key){
		case ' ':
			bLearnBakground = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
