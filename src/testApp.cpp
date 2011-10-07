#include "testApp.h"

const int width = 800;
const int height = 600;

GLfloat lightOnePosition[] = {40.0, 40, 100.0, 0.0};
GLfloat lightOneColor[] = {0.99, 0.99, 0.99, 1.0};

GLfloat lightTwoPosition[] = {-40.0, 40, 100.0, 0.0};
GLfloat lightTwoColor[] = {0.99, 0.99, 0.99, 1.0};

//--------------------------------------------------------------
void testApp::setup(){
	counter = 0;
	//ofSetCircleResolution(30);
    
    //Font names
    ofTrueTypeFont::setGlobalDpi(72);
    Bauer.loadFont("BauerBodoniStd-Bold.otf", 20, true, true);
	
    ofSetVerticalSync(true);
    ofSetFrameRate(60); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.
    //ofEnableAlphaBlending();
    ofSetLogLevel(OF_LOG_VERBOSE);
    glShadeModel (GL_SMOOTH);
    
    Glossy.loadImage("GlossySphere.png");
    
    camWidth = 640;
    camHeight = 480;
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(camWidth,camHeight);
    
    colorImg.allocate(camWidth,camHeight);
    grayImg.allocate(camWidth,camHeight);
    grayBg.allocate(camWidth,camHeight);
    grayDiff.allocate(camWidth,camHeight);
    threshold = 120;
    
    /* initialize lighting */
    glLightfv (GL_LIGHT0, GL_POSITION, lightOnePosition);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, lightOneColor);
    glEnable (GL_LIGHT0);
    glLightfv (GL_LIGHT1, GL_POSITION, lightTwoPosition);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, lightTwoColor);
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHTING);
    glColorMaterial (GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable (GL_COLOR_MATERIAL);
    
	
    bSmooth = false;
	
    //How many people do you want to track?
    trackNum = 30;
    
	//mainOutputSyphonServer.setName("Screen Output");
    

}

//--------------------------------------------------------------
void testApp::update(){
	counter = counter + 0.033f;
    
    ofBackground(100,100,100);
    
    bool bNewFrame = false;

    vidGrabber.grabFrame();
    bNewFrame = vidGrabber.isFrameNew();

    
	if (bNewFrame){
        
        colorImg.setFromPixels(vidGrabber.getPixels(), camWidth,camHeight);

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
		contourFinder.findContours(grayDiff, 20, (camWidth*camHeight)/3, trackNum, true);	// find holes
	}

}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
	
    // Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ofNoFill();
    ofSetLineWidth(1);
    

    //Draw Video
    colorImg.draw(0,0,ofGetWidth(),ofGetHeight());


    
    //Get distance between all different centroids, store distances in an array
    for(int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
        for (int j=0; j<(int)contourFinder.blobs.size(); j++ ) {
        connectDist[i][j] = ofDist(ofMap(contourFinder.blobs[i].centroid.x,0, camWidth,0,ofGetWidth()),
                                ofMap(contourFinder.blobs[i].centroid.y,0,camHeight,0,.75*ofGetWidth()),
                                ofMap(contourFinder.blobs[j].centroid.x,0, camWidth,0,ofGetWidth()),
                                ofMap(contourFinder.blobs[j].centroid.y,0,camHeight,0,.75*ofGetWidth()));
            blobConnect[i]=0;
        }
    }
        
   //Draw Lines
    for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
        ofSetColor(i*20,i*25,i*20, 127);
            mapCentX = ofMap(contourFinder.blobs[i].centroid.x,0,camWidth,0,ofGetWidth());
            mapCentY = ofMap(contourFinder.blobs[i].centroid.y,0,camHeight,0,.75*ofGetWidth());
            for (int j=0; j<(int)contourFinder.blobs.size(); j++ ){
                
                //Only draw lines if the distance between points is within that range
                if (connectDist [i][j] < 200) {
                    //ofMap(sin(ofGetElapsedTimef()), -1, 1, 50, 400)
                    
                    ofSetLineWidth(ofMap(connectDist [i][j], 0, ofMap(sin(ofGetElapsedTimef()), -1, 1, 50, 400), 10.0, 0.1));
                    blobConnect[i]++;
                    ofLine(ofMap(contourFinder.blobs[i].centroid.x,0, camWidth,0,ofGetWidth()),
                           ofMap(contourFinder.blobs[i].centroid.y,0,camHeight,0,.75*ofGetWidth()),
                           ofMap(sin(ofGetElapsedTimef()), 
                                 -1, 
                                  1, 
                                  ofMap(contourFinder.blobs[i].centroid.x,0, camWidth,0,ofGetWidth()),
                                  ofMap(contourFinder.blobs[j].centroid.x,0, camWidth,0,ofGetWidth())),
                           ofMap(sin(ofGetElapsedTimef()), 
                                 -1, 
                                 1, 
                                 ofMap(contourFinder.blobs[i].centroid.y,0, camWidth,0,ofGetWidth()),
                                 ofMap(contourFinder.blobs[j].centroid.y,0, camWidth,0,ofGetWidth())));
                    
                    //ofDrawBitmapString(ofToString(connectDist [i][j]), mapCentX,mapCentY);
                    
            }
        }                
    }  
    
    //Draw circles and highlight which one has the most connections
    bigShot = 0;
    connectCount = 0;
    for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
        mapCentX = ofMap(contourFinder.blobs[i].centroid.x,0,camWidth,0,ofGetWidth());
        mapCentY = ofMap(contourFinder.blobs[i].centroid.y,0,camHeight,0,.75*ofGetWidth());
        
        //Which blob has the most connections to the rest?
        if(blobConnect[bigShot] < blobConnect[i]){  
            bigShot = i;
            ofSetColor(255, 0, 0);
        }
        else {
            
            ofSetColor(i*5,i*4,i*6, 60);
        }
        
        connectCount = (connectCount + blobConnect[i]);
        
        ofFill();
        ofCircle(
                 mapCentX, 
                 mapCentY, 
                 ofMap(blobConnect[i], 0, 10, 2, 20));
        ofNoFill;
        Glossy.draw(mapCentX, mapCentY, ofMap(blobConnect[i], 0, 40, 10, 100),ofMap(blobConnect[i], 0, 40, 10, 100));
        ofSetColor(255, 255, 255, 255);
        Bauer.drawString(ofToString(blobConnect[i]), mapCentX-5,mapCentY+5);  
    }
    
    //Information display
    ofSetColor(255, 255, 255, 127);
    ofRect(0, ofGetHeight()-100, 300,200);
    ofSetColor(0, 0, 0, 255);
    Bauer.drawString("People: " + ofToString(contourFinder.blobs.size()), 10, ofGetHeight()-80 );
    Bauer.drawString("Inter-connections: " + ofToString(connectCount-contourFinder.blobs.size()), 10, ofGetHeight()-60 );
    Bauer.drawString("Most Popular Person: " + ofToString(bigShot), 10, ofGetHeight()-40 );
    Bauer.drawString("Threshold: " + ofToString(threshold), 20,20);
   
	// Syphon Stuff
    
    ofSetColor(255, 255, 255);

	//mainOutputSyphonServer.publishScreen();
    
    
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
    
    switch (key){
		case 'p':
			threshold +=10;
			if (threshold > 255) threshold = 255;
			break;
		case 'o':
			threshold -=10;
			if (threshold < 0) threshold = 0;
			break;
        case OF_KEY_RETURN:
            ofToggleFullscreen();
        case ' ' :
            bLearnBakground == true;
            break;
	}
    keyPress=key;
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

