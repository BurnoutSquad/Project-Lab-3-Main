//
//  main.cpp
//  Lab III
//
//  Created by Jeremiah McCutcheon on 2/12/18.
//  Copyright © 2018 Jeremiah McCutcheon. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <termios.h>

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
//#include <SDL2_ttf/SDL_ttf.h>

#include <opencv2/opencv.hpp>
//#include <opencv2/core.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/features2d.hpp>

#include <curl/curl.h>

using namespace std;

const string SERIAL_PORT_DEVICE = "/dev/tty.Burnout_Squad-RNI-SPP"; // Define Serial Port
const string Flag1 = "/dev/tty.FLAG_1-RNI-SPP"; // Define Serial Port
const string Flag2 = "/dev/tty.RNBT-0BA1-RNI-SPP"; // Define Serial Port
const string Flag3 = "/dev/tty.RNBT-0653-RNI-SPP"; // Define Serial Port
const int baudRate = 9600; // Define Baud Rate

const int width = 800, height = 600;

int fd;
int fd1;
int fd2;
int fd3;

void WriteToSerial(string input);
string ReadFromSerial();
void OpenSerialDevice(string Device);
void SetupSerial(termios &options);
void DownloadImage();
void FindBlob(vector<cv::KeyPoint> &keypoints, cv::Ptr<cv::SimpleBlobDetector> &detector);

int main( int argc, char** argv ){
    
    // SERIAL PORT MANAGMENT
    
    struct termios options;
    
    OpenSerialDevice(SERIAL_PORT_DEVICE);
    //OpenSerialDevice(fd1,Flag1);
    //OpenSerialDevice(fd2,Flag2);
    //OpenSerialDevice(fd3,Flag3);
    
    SetupSerial(options);
    
    ////////////////////////////////////////////////////////////////////////////////////
    
    // Create Blob Detector with parameters & KeyPoints Vector
    using namespace cv;
    
    SimpleBlobDetector::Params params;
    
    // Change thresholds
    params.minThreshold = 10;
    params.maxThreshold = 200;
    
    // Filter by Area.
    params.filterByArea = true;
    params.maxArea = 910000;
    params.minArea = 100;
    
    // Filter by Circularity
    params.filterByCircularity = false;
    params.minCircularity = .3;
    
    // Filter by Convexity
    params.filterByConvexity = true;
    params.minConvexity = 0.65;
    
    // Filter by Inertia
    params.filterByInertia = true;
    params.minInertiaRatio = 0.4;
    
    // Filter by Color
    params.filterByColor = true;
    params.blobColor = 255;

    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
    
    // KeyPoints vector for Blob location data
    vector<KeyPoint> keypoints;

    ////////////////////////////////////////////////////////////////////////////////////
    
    // SDL IMPLEMENTATION
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0){ // Initialize all of SDL
        cout<<"Could not initialize SDL!! Error: "<<SDL_GetError()<<endl;
        return 1;
    }
    
    SDL_Window *window = SDL_CreateWindow("Burnout Squad: Flag Finder", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_ALLOW_HIGHDPI);
    
    if(window==NULL){
        cout<<"Could not create SDL Window, Error: "<<SDL_GetError()<<endl;
        return 1;
    }
    
    // Some parts of this section were direct adaptations of 'olevegard' on StackOverflow
    // https://stackoverflow.com/questions/21890627/drawing-a-rectangle-with-sdl2
    
    SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
    
    // Creat rects (bars) at pos ( 50, 50 ) that's 50 pixels wide and 50 pixels high.
    SDL_Rect bar0;
    bar0.x = 0;
    bar0.y = 800;
    bar0.w = 1600;
    bar0.h = 20;
    
    SDL_Rect bar1;
    bar1.x = 790;
    bar1.y = 820;
    bar1.w = 20;
    bar1.h = 380;
    
    // Show live camera image
    if( !( IMG_Init( IMG_INIT_JPG ) & IMG_INIT_JPG ) ){
        cout << "Could not initialize image: " << IMG_GetError( ) << endl;
        return 1;
    }
    
    SDL_Rect imageRect;
    imageRect.x = 160;
    imageRect.y = 40;
    imageRect.w = 1280;
    imageRect.h = 720;
    
    // Primary App Loop
    
    bool moving = false;
    SDL_Event windowEvent;
    
    SDL_Surface *imageSurface;
    SDL_Texture *imageTexture;
    
    string recieved;
    
    bool blob = true;
    
    const Uint8* keystate;
    
    while(true){
        // Request to send and recieve data from MSP430
        
        //recieved = ReadFromSerial(fd);
        
        // KEYSTROKE TEST
        
        keystate = SDL_GetKeyboardState(NULL);
        
        if(keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W]){
            if(moving == false){
                WriteToSerial("w");
                cout<<"Sending Forward"<<endl;
                blob = false;
                moving = true;
            }
        }
        else if(keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D]){
            if(moving == false){
                WriteToSerial("d");
                cout<<"Sending Right"<<endl;
                blob = false;
                moving = true;
            }
        }
        else if(keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S]){
            if(moving == false){
                WriteToSerial("s");
                cout<<"Sending Reverse"<<endl;
                blob = false;
                moving = true;
            }
        }
        else if(keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A]){
            if(moving == false){
                WriteToSerial("a");
                cout<<"Sending Left"<<endl;
                blob = false;
                moving = true;
            }
        }
        else if(keystate[SDL_SCANCODE_1]){
            if(moving == false){
                WriteToSerial("1");
                blob = true;
                cout<<"Servo Arm"<<endl;
            }
        }
        else if(keystate[SDL_SCANCODE_Z]){
            if(moving == false){
                WriteToSerial("z");
                blob = true;
                cout<<"Exiting Manual Mode"<<endl;
            }
        }
        else if(moving == true){
            WriteToSerial("x");
            cout<<"Sending Stop"<<endl;
            moving = false;
        }
        
        SDL_SetRenderDrawColor( renderer, 210, 210, 210, 255 );
        
        SDL_RenderClear( renderer );
        
        SDL_SetRenderDrawColor( renderer, 120, 120, 120, 255 );
        SDL_RenderFillRect( renderer, &bar0 );
        SDL_RenderFillRect( renderer, &bar1 );
        
        DownloadImage();
        
        if(blob)
            FindBlob(keypoints, detector);
        
        imageSurface = IMG_Load("test.jpg");
        imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
        
        if(imageTexture)
            SDL_RenderCopy(renderer, imageTexture, nullptr, &imageRect);
        else
            cout << "Could not find image texture: " << IMG_GetError( ) << endl;
        
        SDL_RenderPresent(renderer);
        
        if(SDL_PollEvent(&windowEvent))
            if(SDL_QUIT == windowEvent.type)
                break;
        SDL_DestroyTexture(imageTexture);
        SDL_FreeSurface(imageSurface);
    }
    
    // Close SDL elements
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(imageTexture);
    SDL_FreeSurface(imageSurface);
    IMG_Quit();
    SDL_Quit();
    
    ////////////////////////////////////////////////////////////////////////////////////
    
    close(fd);
    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}

//
// FUNCTIONS
//


void FindBlob( vector<cv::KeyPoint> &keypoints, cv::Ptr<cv::SimpleBlobDetector> &detector){
    using namespace cv;
    
    // Read image
    Mat im = imread( "test.jpg", IMREAD_COLOR );
    Mat original = im;
    
    inRange(im, Scalar(70, 90, 10), Scalar(200, 255, 60), im);
    
    detector->detect( im, keypoints);
    
    // Code to draw blobs onto image
    //Mat im_with_keypoints;
    drawKeypoints( im, keypoints, im, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
    imwrite("result.jpg", im);
    
    drawKeypoints( original, keypoints, im, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
    
    // Code to output resultant image
    imwrite("test.jpg", im);
    
    
    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    
    if(keypoints.size()>0){
        cout<<"Blob Detected"<<endl;
        
        for(int i=0; i<keypoints.size();i++){
            cout<<"x value: "<<keypoints[i].pt.x<<endl;
            cout<<"y value: "<<keypoints[i].pt.y<<endl;
            cout<<"size: "<<keypoints[i].size<<endl;
        }
        
        /*
         These functions utilize usleep(u-seconds);
         1/1,000,000 of a second:
         100,000: 1/10 second
         500,000: 1/2 second
         1,000,000: 1 second
        */
        
        // Cases detect blob size
        // blobs size 0-100
        
        int bigBlob = 0;
        
        for(int i=0; i<keypoints.size(); i++){
            if(keypoints[i].size > bigBlob)
                bigBlob = i;
        }
        
        
        if(keypoints[bigBlob].size >= 1 && keypoints[bigBlob].size < 100){
            WriteToSerial("w"); // Forward
            usleep(300000);
            WriteToSerial("x"); // Stop
            if(keypoints[bigBlob].pt.x > 800 && keypoints[bigBlob].pt.x < 1280){
                WriteToSerial("d"); // Right
                usleep(250000);
                WriteToSerial("x"); // Stop
            }
            else if(keypoints[bigBlob].pt.x > 0 && keypoints[bigBlob].pt.x < 480){
                WriteToSerial("a"); // Left
                usleep(250000);
                WriteToSerial("x"); // Stop
            }
            else
                WriteToSerial("x"); // Stop
            usleep(500000); // Wait for image to stabilize
        }
        
        // blobs size 100-200
        else if(keypoints[bigBlob].size >= 100 && keypoints[bigBlob].size < 200){
            WriteToSerial("w"); // Forward
            usleep(250000);
            WriteToSerial("x"); // Stop
            if(keypoints[bigBlob].pt.x > 800 && keypoints[bigBlob].pt.x < 1280){
                WriteToSerial("d"); // Right
                usleep(200000);
                WriteToSerial("x"); // Stop
            }
            else if(keypoints[bigBlob].pt.x > 0 && keypoints[bigBlob].pt.x < 480){
                WriteToSerial("a"); // Left
                usleep(200000);
                WriteToSerial("x"); // Stop
            }
            else
                WriteToSerial("x"); // Stop
            usleep(500000); // Wait for image to stabilize
        }
        
        // blobs size 200-300
        else if(keypoints[bigBlob].size >= 200 && keypoints[bigBlob].size < 300){
            WriteToSerial("w"); // Forward
            usleep(200000);
            WriteToSerial("x"); // Stop
            if(keypoints[bigBlob].pt.x > 800 && keypoints[bigBlob].pt.x < 1280){
                WriteToSerial("d"); // Right
                usleep(150000);
                WriteToSerial("x"); // Stop
            }
            else if(keypoints[bigBlob].pt.x > 0 && keypoints[bigBlob].pt.x < 480){
                WriteToSerial("a"); // Left
                usleep(150000);
                WriteToSerial("x"); // Stop
            }
            else
                WriteToSerial("x"); // Stop
            usleep(500000); // Wait for image to stabilize
        }
        
        // blobs size 300-350
        else if(keypoints[bigBlob].size >= 300 && keypoints[bigBlob].size < 400){
            WriteToSerial("w"); // Forward
            usleep(150000);
            WriteToSerial("x"); // Stop
            if(keypoints[bigBlob].pt.x > 800 && keypoints[bigBlob].pt.x < 1280){
                WriteToSerial("d"); // Right
                usleep(100000);
                WriteToSerial("x"); // Stop
            }
            else if(keypoints[bigBlob].pt.x > 0 && keypoints[bigBlob].pt.x < 480){
                WriteToSerial("a"); // Left
                usleep(100000);
                WriteToSerial("x"); // Stop
            }
            else
                WriteToSerial("x"); // Stop
            usleep(500000); // Wait for image to stabilize
        }
        
        // blobs size 350-400
        else if(keypoints[bigBlob].size >= 400 && keypoints[bigBlob].size < 475){
            WriteToSerial("w"); // Forward
            usleep(100000);
            WriteToSerial("x"); // Stop
            if(keypoints[bigBlob].pt.x > 800 && keypoints[bigBlob].pt.x < 1280){
                WriteToSerial("d"); // Right
                usleep(50000);
                WriteToSerial("x"); // Stop
            }
            else if(keypoints[bigBlob].pt.x > 0 && keypoints[bigBlob].pt.x < 480){
                WriteToSerial("a"); // Left
                usleep(50000);
                WriteToSerial("x"); // Stop
            }
            else
                WriteToSerial("x"); // Stop
            usleep(500000); // Wait for image to stabilize
        }
        
        // blob size > 400
        else if(keypoints[bigBlob].size >= 475 && keypoints[bigBlob].pt.x > 480 && keypoints[bigBlob].pt.x < 800){
            WriteToSerial("1");
            usleep(9000000);
        }
    }
    
}

void DownloadImage(){
    CURL *image;
    CURLcode imgresult;
    FILE *fp;
    
    image = curl_easy_init();
    
    fp = NULL;
    
    if( image ){
        // Open file
        fp = fopen("test.jpg", "wb");
        if( fp == NULL ) cout << "File cannot be opened";
        
        curl_easy_setopt(image, CURLOPT_URL, "http://admin@192.168.1.5/media/?action=snapshot");
        curl_easy_setopt(image, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(image, CURLOPT_WRITEDATA, fp);
        
        
        // Grab image
        imgresult = curl_easy_perform(image);
        if( imgresult ){
            cout << "Cannot grab the image!\n";
        }
    }
    
    // Clean up the resources
    curl_easy_cleanup(image);
    // Close the file
    fclose(fp);
}


void WriteToSerial(string input){
    unsigned char cmd[input.length()];
    
    for(int i=0; i<input.length(); i++){
        cmd[i]=input[i];
    }
    
    write(fd, cmd, input.length());
    
}

string ReadFromSerial(){
    string temp;
    
    unsigned char *buf;
    buf = (unsigned char*)calloc(256, sizeof(buf));
    
    WriteToSerial("r");
    
    usleep(10000);
    
    // *** READ ***
    long n = read( fd, buf , sizeof buf );
    
    // Error Handling
    if (n < 0)
    {
        cout << "Error reading: " << strerror(errno) << endl;
    }
    cout<<n<<endl;
    // Print what I read...
    cout << "Read: " << buf << endl;
    
    for(int i=0; i<sizeof buf; i++){
        temp[i] = buf[i];
    }
    
    return temp;
}

void OpenSerialDevice(string device){
    
    fd = open(device.c_str(),O_RDWR);
    if ( -1 == fd ){
        cerr << "Failed to open the serial port device "
        << device
        << endl ;
        exit(-1);
        }
    else{
        cerr << "Serial port device opened successfully."
        << device
        << endl ;
    }
}

void SetupSerial(termios &options){
    fcntl(fd, F_SETFL, FNDELAY);                    // Open the device in nonblocking mode
    
    // Set parameters
    tcgetattr(fd, &options);                        // Get the current options of the port
    bzero(&options, sizeof(options));               // Clear all the options
    speed_t         Speed;
    switch (baudRate)                               // Set the speed (baudRate)
    {
        case 110  :     Speed=B110; break;
        case 300  :     Speed=B300; break;
        case 600  :     Speed=B600; break;
        case 1200 :     Speed=B1200; break;
        case 2400 :     Speed=B2400; break;
        case 4800 :     Speed=B4800; break;
        case 9600 :     Speed=B9600; break;
        case 19200 :    Speed=B19200; break;
        case 38400 :    Speed=B38400; break;
        case 57600 :    Speed=B57600; break;
        case 115200 :   Speed=B115200; break;
        default : exit(-4);
    }
    cfsetispeed(&options, Speed);                   // Set the baud rate at 9600 bauds
    cfsetospeed(&options, Speed);
    options.c_cflag |= ( CLOCAL | CREAD |  CS8);    // Configure the device : 8 bits, no parity, no control
    options.c_iflag |= ( IGNPAR | IGNBRK );
    options.c_cc[VTIME]=0;                          // Timer unused
    options.c_cc[VMIN]=0;                           // At least on character before satisfy reading
    tcsetattr(fd, TCSANOW, &options);               // Activate the settings
}
