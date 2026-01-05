/**
 * @file main.cpp
 * @description basic test cases for HexTree
 */

#include <iostream>
#include <string>

#include "hextree.h"

using namespace std;

/*** TEST FUNCTION DECLARATIONS ***/

void TestBuildRender(bool fulldepth, unsigned int maxdepth);
void TestFlipHorizontal();
void TestPrune(double tol);


/*** MAIN FUNCTION PROGRAM ENTRY ***/

int main(int argc, char* argv[]) {

	TestBuildRender(true, 0);
	TestBuildRender(false, 4);
	TestFlipHorizontal();
	TestPrune(0.01);
	TestPrune(0.05);

	return 0;
}


/*** TEST FUNCTION IMPLEMENTATIONS ***/

void TestBuildRender(bool fulldepth, unsigned int maxdepth) {
	cout << "Entered TestBuildRender, fulldepth: " << fulldepth << ", maxdepth: " << maxdepth << endl;

	// read input PNG
	PNG input;
	input.readFromFile("images-orig/malachi-60x87.png");

	cout << "Constructing HexTree from image... ";
	HexTree t(input);
	cout << "done." << endl;

	cout << "Rendering tree to PNG with fulldepth = " << fulldepth << ", maxdepth = " << maxdepth << "... ";
	PNG output = t.Render(fulldepth, maxdepth);
	cout << "done." << endl;

	// write output PNG
	string outfilename = "images-output/malachi-render_" + to_string(fulldepth) + "_" + to_string(maxdepth) + ".png";
	cout << "Writing rendered PNG to file... ";
	output.writeToFile(outfilename);
	cout << "done." << endl;

	cout << "Exiting TestBuildRender.\n" << endl;
}

void TestFlipHorizontal() {
	cout << "Entered TestFlipHorizontal" << endl;

	// read input PNG
	PNG input;
	input.readFromFile("images-orig/malachi-60x87.png");

	cout << "Constructing HexTree from image... ";
	HexTree t(input);
	cout << "done." << endl;

	cout << "Calling FlipHorizontal... ";
	t.FlipHorizontal();
	cout << "done." << endl;

	cout << "Rendering tree to PNG at full depth... ";
	PNG output = t.Render(true, 0);
	cout << "done." << endl;

	// write output PNG
	string outfilename = "images-output/malachi-fliphorizontal_x1-render_true_0.png";
	cout << "Writing rendered PNG to file... ";
	output.writeToFile(outfilename);
	cout << "done." << endl;

	cout << "Calling FlipHorizontal a second time... ";
	t.FlipHorizontal();
	cout << "done." << endl;

	cout << "Rendering tree to PNG at full depth... ";
	output = t.Render(true, 0);
	cout << "done." << endl;

	// write output PNG
	outfilename = "images-output/malachi-fliphorizontal_x2-render_true_0.png";
	cout << "Writing rendered PNG to file... ";
	output.writeToFile(outfilename);
	cout << "done." << endl;

	cout << "Exiting TestFlipHorizontal.\n" << endl;
}

void TestPrune(double tol) {
	cout << "Entered TestPrune, tolerance: " << tol << endl;

	// read input PNG
	PNG input;
	input.readFromFile("images-orig/kkkk_nnkm-256x224.png");

	cout << "Constructing HexTree from image... ";
	HexTree t(input);
	cout << "done." << endl;

	cout << "Tree contains " << t.CountNodes() << " nodes and " << t.CountLeaves() << " leaves." << endl;

	cout << "Calling Prune... ";
	t.Prune(tol);
	cout << "done." << endl;

	cout << "Pruned tree contains " << t.CountNodes() << " nodes and " << t.CountLeaves() << " leaves." << endl;

	cout << "Rendering tree to PNG at full depth... ";
	PNG output = t.Render(true, 0);
	cout << "done." << endl;

	// write output PNG
	string outfilename = "images-output/kkkk_nnkm-256x224-prune_" + to_string(tol) + "-render_true_0.png";
	cout << "Writing rendered PNG to file... ";
	output.writeToFile(outfilename);
	cout << "done." << endl;

	cout << "Exiting TestPrune.\n" << endl;
}