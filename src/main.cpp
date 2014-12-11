#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <vector>
#include <iostream>
/* rand example: guess the number */
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>
#include <string.h>
#include <fcntl.h>

using namespace cv;

class Ball
{
	public:
		int i;
		int j;
		double di,dj;
		int edge1, edge2;
		int radius;
		int testRadius;
		int magnitude;

		Ball()
		{
			edge1 = 638;
			edge2=477;
			radius = 32;
			testRadius = 64;
		}

		void set(int aRadius, int aTestRadius)
		{
			i = rand() % 100 + 50;
			j = rand() % 100 + 50;
			di = rand() % 8 + 1;
			dj = rand() % 8 + 1;

			testRadius = aTestRadius;
			radius = aRadius;
		}

		void step()
		{
			if(i>=edge1 || j>=edge2 || i<=0 || j<=0)
				bounce();

			i += di;
			j += dj;
		}

		void speed()
		{
			di += (di<0)? -0.5:0.5;
			dj += (dj<0)? -0.5:0.5;
		}

		void bounce()
		{
			di = (i<=0)*(-di)+(i>=edge1)*(-di)+(i<edge1 &&i>0)*(di);
			dj = (j<=0)*(-dj)+(j>=edge2)*(-dj)+(j<edge2 &&j>0)*(dj);
		}

		bool test(int x, int y)
		{
			magnitude = std::sqrt( (x-i)*(x-i)+(y-j)*(y-j) );

			std::cout<<"x="<<x<<" y="<<y<<" mag="<<magnitude<<"\n";
			return magnitude < testRadius;
		}

};

int main(int, char**)
{
    VideoCapture cap(0);
    if(!cap.isOpened())
	{
    	std::cout<<" Error: unable to open video stream \n";
    	return -1;
	}

    Mat frame, edges,hsv,greenmask;

    namedWindow("BkRV", CV_WINDOW_AUTOSIZE);
    //void cvResizeWindow( const char* name, int width, int height );

//    cvResizeWindow("BkRV",600,600);

    double count;
    double imean;
    double jmean;

    double speedTrigger=50000;
    double frameCount=1;
    unsigned long frameTotalCount = 0;

    unsigned int i,j;
    char k;

#ifdef ZED
    //http://falsinsoft.blogspot.co.il/2012/11/access-gpio-from-linux-user-space.html
    FILE * fd;
    char buf[20];
    int gpio = 19  +54;

    fd = fopen("/sys/class/gpio/export", "w");

    sprintf(buf, "%d", gpio);

    fwrite(buf,  sizeof(char), sizeof(buf),fd);

    fclose(fd);

    // set the direction
    char buf1[50];
    sprintf(buf1, "/sys/class/gpio/gpio%d/direction", gpio);

    fd = fopen(buf1, "w");

    // Set out direction
    //write(fd, "out", 3);
    // Set in direction
    fwrite("in", sizeof(char), sizeof("in"),fd);

    fclose(fd);

    char buf2[30];
    char value;
    //read out
//    sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
//
//    fd = open(buf, O_WRONLY);
//
//    // Set GPIO high status
//    write(fd, "1", 1);
//    // Set GPIO low status
//    write(fd, "0", 1);
//
//    close(fd);

#endif

    Ball ball[5];

	for(int i=0; i<5; i++)
		ball[i].set(20,54);

    bool gameOver = false;
    bool gameStart = false;

    char score[300];

    for(;;)
    {
        cap >> frame;

        k=cvWaitKey(1);

#ifdef ZED


        sprintf(buf2, "/sys/class/gpio/gpio%d/value", gpio);

        fd = fopen(buf2, "r");

        fread( &value,sizeof(char), sizeof(1),fd);

        if(value == '1')
        {
             k=' ';
        }
        fclose(fd);

#endif

        if(gameStart && !gameOver)
        {
			cvtColor(frame,hsv, CV_BGR2HSV);

			inRange(hsv, Scalar(47, 80, 100), Scalar(68, 200, 255), greenmask );

			count=0;imean=0;jmean=0;

			for(i=0;i<greenmask.rows;i++)
				for(j=0;j<greenmask.cols;j++)
					if(greenmask.at<uchar>(i,j)>0)
					{
						imean+=j;
						jmean+=i;
						count++;
					}

			imean/=count;
			jmean/=count;

			if(count>500)
			{
				//std::cout<<" i="<<imean<<"  j="<<jmean<<" count="<<count<<"\n";
				circle(frame, Point(imean,jmean), 32.0, Scalar( 0, 255, 0 ), 5, 8 );
			}

			for( i=0; i<5; i++)
			{
				if(ball[i].test(imean,jmean))
					gameOver=true;

				if(frameCount==0)
					ball[i].speed();

				ball[i].step();

				circle(frame, Point(ball[i].i,ball[i].j), 32.0, Scalar( 0, 0, 255 ), 5, 8 );
			}

			frameCount = (frameCount==speedTrigger)? 0 : frameCount+1;

			frameTotalCount++;
        }
        else if(!gameStart && !gameOver)
        {
        	putText(frame," BT: Ball Play",Point(20,100),CV_FONT_HERSHEY_COMPLEX, 2,Scalar(255, 255, 255), 3, 8 );

        	putText(frame," Press Space To Start",Point(50,250),CV_FONT_HERSHEY_COMPLEX, 1,Scalar(i, 255, 0), 2, 8 );

        	if(k==' ')
        	{
        		std::cout<<"pressed Space Bar 1\n";

        		gameStart = true;
        		gameOver  = false;
        		frameTotalCount = 0;
        		frameCount = 0;

        		for(int i=0; i<5; i++)
        			ball[i].set(20,54);
        	}


        }
        else if(gameOver)
        {
        	putText(frame," Game Over",Point(50,100),CV_FONT_HERSHEY_COMPLEX, 2,Scalar(i, i, 255), 2, 8 );

        	sprintf(score,"score:%lu ",frameTotalCount/20);

        	putText(frame,score,Point(50,200),CV_FONT_HERSHEY_COMPLEX, 1,Scalar(255, 255, 255), 2, 8 );

        	putText(frame," Press Space To Start",Point(50,250),CV_FONT_HERSHEY_COMPLEX, 1,Scalar(i, 255, 0), 2, 8 );

        	if(k==' ')
        	{
        		std::cout<<"pressed Space Bar  2\n";

        		gameStart = true;
        		gameOver  = false;
        		frameTotalCount = 0;
        		frameCount = 0;

        		for(int i=0; i<5; i++)
        			ball[i].set(20,54);
        	}


        }

        imshow("BkRV", frame);



        if(k==27)
        	break;

    }

    std::cout<<"Program Complete \n";


#ifdef ZED
    fd = fopen("/sys/class/gpio/unexport", "w");

    sprintf(buf, "%d", gpio);

    fwrite(buf, sizeof(char), sizeof(buf),fd);

    fclose(fd);
#endif

    return 0;
}
//
//
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/"
//#include "opencv2/opencv.hpp"
//#include <vector>
//#include <iostream>
//
//
//using namespace cv;
//
//int main(int, char**)
//{
//    VideoCapture cap(0);
//    if(!cap.isOpened())
//	{
//    	std::cout<<" Error: unable to open video stream \n"
//    	return -1;
//	}
//
//    Mat frame, edges,imgHSV,imgThresh;
//
//    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC3);
//
//    std::vector<cv::KeyPoint> keypoints;
//
//    namedWindow("BkRV", CV_WINDOW_AUTOSIZE); //1);
//
//    cv::Point min_loc, max_loc;
//
//    CBlobResult blobs;
//    CBlob *currentBlob;
//    CvPoint pt1, pt2;
//    CvRect cvRect;
//
//    for(;;)
//    {
//        cap >> frame;
//
//    	cvCvtColor( frame, imgHSV, CV_BGR2HSV );
//
//        cv::minMaxLoc(your_mat, &min, &max, &min_loc, &max_loc);
//
//        cvInRangeS( imgHSV,
//                    cvScalar( 104, 178, 70  ),
//                    cvScalar( 130, 240, 124 ),
//                    imgThresh );
//
//        blobs = CBlobResult( imgThresh, NULL, 0 );
//
////        cvtColor(frame, edges, COLOR_BGR2GRAY);
////        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
////        Canny(edges, edges, 0, 30, 3);
////        imshow("edges", edges);
//        if(waitKey(30) >= 0) break;
//    }
//    return 0;
//}
