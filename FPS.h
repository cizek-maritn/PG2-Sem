#pragma once

class FPS {
public:
	FPS();
	bool secondPassed();
	int getFrames();
	int getPrevFrames();
	void setFrames(int);
private:
	double currentTime;
	double prevTime;
	int frames;
	int prevFrames;
};