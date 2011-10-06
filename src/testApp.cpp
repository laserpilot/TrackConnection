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
	//ofSetCircleResolution(4);
	
    ofSetVerticalSync(true);
    //ofEnableAlphaBlending();
    ofSetLogLevel(OF_LOG_VERBOSE);
    glShadeModel (GL_SMOOTH);
    
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
	ofSetWindowTitle("graphics example");
    
	mainOutputSyphonServer.setName("Screen Output");
    
	ofSetFrameRate(60); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.
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
		contourFinder.findContours(grayDiff, 20, (camWidth*camHeight)/3, 10, true);	// find holes
	}

}

//--------------------------------------------------------------
void testApp::draw(){
	
    // Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
    ofEnableSmoothing();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ofNoFill();
    ofSetLineWidth(1);
    

    //Draw Video
    colorImg.draw(0,0,ofGetWidth(),ofGetHeight());
    
    //Get distance between all different centroids
    for(int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
        for (int j=0; j<(int)contourFinder.blobs.size(); j++ ) {
        connectDist[i][j] = ofDist(ofMap(contourFinder.blobs[i].centroid.x,0, camWidth,0,ofGetWidth()),
                                ofMap(contourFinder.blobs[i].centroid.y,0,camHeight,0,.75*ofGetWidth()),
                                ofMap(contourFinder.blobs[j].centroid.x,0, camWidth,0,ofGetWidth()),
                                ofMap(contourFinder.blobs[j].centroid.y,0,camHeight,0,.75*ofGetWidth()));
        }
    }
        
   //Draw Circles and Lines
    for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
        ofSetColor(i*20,i*25,i*20);
            mapCentX = ofMap(contourFinder.blobs[i].centroid.x,0,camWidth,0,ofGetWidth());
            mapCentY = ofMap(contourFinder.blobs[i].centroid.y,0,camHeight,0,.75*ofGetWidth());
            for (int j=0; j<(int)contourFinder.blobs.size(); j++ ){
                //Only draw lines if the distance between points is within that range
                if (connectDist [i][j] < 200) {
                    ofLine(ofMap(contourFinder.blobs[i].centroid.x,0, camWidth,0,ofGetWidth()),
                           ofMap(contourFinder.blobs[i].centroid.y,0,camHeight,0,.75*ofGetWidth()),
                           ofMap(contourFinder.blobs[j].centroid.x,0, camWidth,0,ofGetWidth()),
                           ofMap(contourFinder.blobs[j].centroid.y,0,camHeight,0,.75*ofGetWidth()));
                    //ofDrawBitmapString(ofToString(connectDist [i][j]), mapCentX,mapCentY);
            }
        }
             
        ofFill();
        ofCircle(mapCentX, mapCentY, 10);
        ofNoFill;
               
    }  
    /*
    ofMesh mesh;
    ofVec3f meshpoint = (0, 0, 0);
    ofVec3f prevMeshpoint = (4, 4, 0);
    
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    for(int j = 0; j < (int)contourFinder.blobs.size(); j++) {
        ofColor cur(255, 255, 0, 255);
        mesh.addColor(cur);
        mesh.addVertex(meshpoint );
        mesh.addVertex(meshpoint + ofVec3f(3, 0, 0));
        mesh.addColor(cur);
        mesh.addVertex(prevMeshpoint);
        mesh.addVertex(prevMeshpoint - ofVec3f(3, 0, 0));
    }
    
    mesh.draw();
     */
    
    ofDrawBitmapString("Threshold: " + ofToString(threshold), 20,20);
   
	// Syphon Stuff
    
    ofSetColor(255, 255, 255);

	//mainOutputSyphonServer.publishScreen();
    
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
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

