#pragma once

class FPS {
public:
	FPS();
	bool secondPassed();
	int getFrames();
	void setFrames(int);
private:
	double currentTime;
	double prevTime;
	int frames;
};