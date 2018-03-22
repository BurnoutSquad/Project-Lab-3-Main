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
#include <SDL2_ttf/SDL_ttf.h>

#include <curl/curl.h>


using namespace std;

const string SERIAL_PORT_DEVICE = "/dev/tty.Burnout_Squad-RNI-SPP"; // Define Serial Port
const int baudRate = 9600;                      // Define Baud Rate

const int width = 800, height = 600;


void WriteToSerial(string input, int &FileDescriptor);
void OpenSerialDevice(int &FileDescriptor);
void SetupSerial(termios &options, int &fd);
void DownloadImage();

int main( int argc, char** argv ){
    // SERIAL PORT MANAGMENT
    int fd;
    struct termios options;
    
    //OpenSerialDevice(fd);
    
    SetupSerial(options, fd);
    
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
    
    // Set render color to light grey ( background will be rendered in this color )
    //SDL_SetRenderDrawColor( renderer, 210, 210, 210, 255 );
    
    // Clear winow
    //SDL_RenderClear( renderer );
    
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
    
    // Set render color to Grey ( rects will be rendered in this color )
    //SDL_SetRenderDrawColor( renderer, 120, 120, 120, 255 );
    
    // Render rects
    //SDL_RenderFillRect( renderer, &bar0 );
    //SDL_RenderFillRect( renderer, &bar1 );
    
    // Render the rect to the screen
    //SDL_RenderPresent(renderer);
    
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
    
    bool moving = false;
    
    // Primary App Loop
    SDL_Event windowEvent;
    while(true){
        
        //KEYSTROKE TEST
        
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        
        if(keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W]){
            if(moving == false){
                //WriteToSerial("s",fd);
                cout<<"Sending Forward"<<endl;
                moving = true;
            }
        }
        else if(keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D]){
            if(moving == false){
                //WriteToSerial("s",fd);
                cout<<"Sending Right"<<endl;
                moving = true;
            }
        }
        else if(keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S]){
            if(moving == false){
                //WriteToSerial("s",fd);
                cout<<"Sending Reverse"<<endl;
                moving = true;
            }
        }
        else if(keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A]){
            if(moving == false){
                //WriteToSerial("s",fd);
                cout<<"Sending Left"<<endl;
                moving = true;
            }
        }
        else if(moving == true){
            //WriteToSerial("x",fd);
            cout<<"Sending Stop"<<endl;
            moving = false;
        }

        
        
        
        
        
        
        SDL_SetRenderDrawColor( renderer, 210, 210, 210, 255 );
        
        SDL_RenderClear( renderer );
        
        SDL_SetRenderDrawColor( renderer, 120, 120, 120, 255 );
        SDL_RenderFillRect( renderer, &bar0 );
        SDL_RenderFillRect( renderer, &bar1 );
        
        //DownloadImage();
        
        SDL_Surface *imageSurface = IMG_Load("test.jpg");
        SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
        
        if(imageTexture)
            SDL_RenderCopy(renderer, imageTexture, nullptr, &imageRect);
        else
            cout << "Could not find image texture: " << IMG_GetError( ) << endl;
        
        SDL_RenderPresent(renderer);
        
        if(SDL_PollEvent(&windowEvent))
            if(SDL_QUIT == windowEvent.type)
                break;
    }
    
    SDL_DestroyWindow(window);
    // SDL_DestroyTexture(imageTexture);
    // SDL_FreeSurface(imageSurface);
    IMG_Quit();
    SDL_Quit();
    
    ////////////////////////////////////////////////////////////////////////////////////
    
    /*
    
     SDL Keystroke test code.. didnt work
    
    while(1){
    
    SDL_Event event;
    
    SDL_PollEvent(&event);
    
    if(event.type == SDL_KEYDOWN)
    {
        // Move centerpoint of rotation for one of the trees:
        switch(event.key.keysym.sym)
        {
            case SDLK_UP:
                WriteToSerial("w",fd);
                cout<<"Sending Forward"<<endl;
                break;
            case SDLK_DOWN:
                WriteToSerial("s",fd);
                cout<<"Sending Reverse"<<endl;
                break;
            case SDLK_LEFT:
                WriteToSerial("a",fd);
                cout<<"Sending Left"<<endl;
                break;
            case SDLK_RIGHT:
                WriteToSerial("d",fd);
                cout<<"Sending Right"<<endl;
                break;
            case SDLK_w:
                WriteToSerial("w",fd);
                cout<<"Sending Forward"<<endl;
                break;
            case SDLK_a:
                WriteToSerial("a",fd);
                cout<<"Sending Right"<<endl;
                break;
            case SDLK_s:
                WriteToSerial("s",fd);
                cout<<"Sending Left"<<endl;
                break;
            case SDLK_d:
                WriteToSerial("d",fd);
                cout<<"Sending Reverse"<<endl;
                break;
            case SDLK_SPACE:
                WriteToSerial("x",fd);
                cout<<"Sending Stop"<<endl;
            case SDLK_ESCAPE:
                cout<<"Exiting Program!"<<endl;
                close(fd);
                return 0;
            default:
                break;
        }
    }
    
    }
     */
    /*
     
     Read code: Should work.. not fast enough??
     Need to implement call for read on bluesmirf device
     
    unsigned char buf[256];
    memset (&buf, '\0', sizeof buf);
     
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

     */
    
    close(fd);
    
    return 0;
}

//
// FUNCTIONS
//

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


void WriteToSerial(string input, int &FileDescriptor){
    unsigned char cmd[input.length()];
    
    for(int i=0; i<input.length(); i++){
        cmd[i]=input[i];
    }
    
    write(FileDescriptor, cmd, input.length());
    
}

void OpenSerialDevice(int &FileDescriptor){
    
    FileDescriptor = open(SERIAL_PORT_DEVICE.c_str(),O_RDWR);
    if ( -1 == FileDescriptor ){
        cerr << "Failed to open the serial port device "
        << SERIAL_PORT_DEVICE
        << endl ;
        exit(-1);
        }
    else{
        cerr << "Serial port device opened successfully."
        << endl ;
    }
}

void SetupSerial(termios &options, int &fd){
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
