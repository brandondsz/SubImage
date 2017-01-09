
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include<conio.h>   

using namespace std;
using namespace cv;

int topMost, leftMost, rightMost, bottomMost, firstRow, firstCol;
Mat imgGrayscale, imgOriginal;
Scalar intensity;
void moveLeft( int col, int row);
void moveRight( int col, int row);
void moveUp( int col, int row);
void moveDown( int col, int row);
void moveAroundObject(int startCol, int startRow);

//Algorithm
//1 Scan through the image to find a point that is not part of the background
//2 Once found:
//	a Travel around the points linked(the sub image)
//	b Update the topMost, leftMost, rightMost and bottomMost points reached appropriately
//	c Do this until the start point is reached
//3 Create a new image with the found topMost, leftMost, rightMost and bottomMost points
//4 Replace the area on these point on the original image to match the background(so that they are not detected again)
//5 Keep doing 2,3 and 4 until the whole image is scanned

int main() {


	imgOriginal = cv::imread("Player.png");

	imgGrayscale = cv::imread("Player.png", 0);
	
	//Add padding around the image
	copyMakeBorder(imgGrayscale, imgGrayscale, 2, 2, 2, 2, BORDER_CONSTANT, Scalar(255, 255, 255));

	//Smoothen the image so that if there are openings in subimages, they are closed  
	for (int i = 1; i < 4; i = i + 2)
	{
		blur(imgGrayscale, imgGrayscale, Size(i, i), Point(-1, -1));
	}
	
	//Use this when the background is not white
	//adaptiveThreshold(imgGrayscale, imgGrayscale, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 2);
	
	if (imgOriginal.empty()) {
		std::cout << "error: image not read from file\n\n";
		_getch();
		return(0);
	}

	for (int i = 0; i < imgGrayscale.cols; i++) {
		for (int j = 0; j < imgGrayscale.rows; j++) {
			intensity = imgGrayscale.at<uchar>(j, i);
			if (intensity.val[0] < 255) {
				moveAroundObject(i, j);
			}
		}
	}
	cv::waitKey(0);

	return(0);
}

void moveLeft( int col, int row) {
	col--;
	leftMost = (leftMost > col || leftMost == -1) ? col : leftMost;
	//if reached start or can  not move more left 
	if (col <= 0 || (col == firstCol && (row + 1) == firstRow)) {		
		return ;
	}

	if ((intensity = imgGrayscale.at<uchar>(row + 1, col)).val[0] == 255) {
		//down
		moveDown( col, row);
	}
	else if ((intensity = imgGrayscale.at<uchar>(row, col - 1)).val[0] == 255) {
		//left

		moveLeft( col, row);
	}
	else if ((intensity = imgGrayscale.at<uchar>(row - 1, col)).val[0] == 255) {
		//up

		moveUp( col, row);
	}
	else {
		moveRight( col, row);
	}

}


void moveUp( int col, int row) {
	row--;
	topMost = (topMost > row || topMost == -1) ? row : topMost;
	//if left is empty or can  not move more up
	if (row <= 0) {
		moveLeft( col, row);
	}

	if ((intensity = imgGrayscale.at<uchar>(row, col - 1)).val[0] == 255) {
		//then move up and save the value of row
		moveLeft( col, row);
	}
	else if ((intensity = imgGrayscale.at<uchar>(row - 1, col)).val[0] == 255) {
		moveUp( col, row);
	}
	else if ((intensity = imgGrayscale.at<uchar>(row, col + 1)).val[0] == 255) {
		moveRight( col, row);
	}
	else {
		moveDown( col, row);
	}
}


void moveRight( int col, int row) {
	col++;
	rightMost = col > rightMost ? col : rightMost;
	//if up is empty or can not move more right
	if (col >= imgGrayscale.cols) {
		moveUp( col, row);
	}

	if ((intensity = imgGrayscale.at<uchar>(row - 1, col)).val[0] == 255) {
		moveUp( col, row);
	}
	else if ((intensity = imgGrayscale.at<uchar>(row, col + 1)).val[0] == 255) {
		moveRight( col, row);
	}
	else if ((intensity = imgGrayscale.at<uchar>(row + 1, col)).val[0] == 255) {
		moveDown( col, row);
	}
	else {
		moveLeft( col, row);
	}
}


void moveDown( int col, int row) {
	row++;
	bottomMost = bottomMost < row ? row : bottomMost;
	//if right is empty or can  not move more down
	if (row >= imgGrayscale.rows) {
		//add indicator that there was a block
		moveRight( col, row);
	}
	
	// if right is empty move right
	if ((intensity = imgGrayscale.at<uchar>(row, col + 1)).val[0] == 255) {
		moveRight( col, row);

	}//if down is empty move down
	else if ((intensity = imgGrayscale.at<uchar>(row + 1, col)).val[0] == 255) {
		moveDown( col, row);

	}//if left is empty move left
	else if ((intensity = imgGrayscale.at<uchar>(row, col - 1)).val[0] == 255) {
		moveLeft( col, row);
	}
	else {
		moveUp( col, row);
	}
}



void moveAroundObject(int startCol, int startRow) {

	topMost = -1; leftMost = -1; rightMost = -1; bottomMost = -1;
	firstCol = --startCol;
	firstRow = startRow;
	moveDown(startCol, startRow - 1);

	Rect myROI(leftMost - 1, topMost - 1, rightMost - leftMost - 2, bottomMost - topMost - 2);

	string name = "./SubImages/" + std::to_string(startCol) + std::to_string(startRow) + ".png";
	imwrite(name, imgOriginal(myROI));
	rectangle(imgGrayscale, Point(leftMost, topMost), Point(rightMost, bottomMost), Scalar(255, 255, 255), -1, 8);

}






