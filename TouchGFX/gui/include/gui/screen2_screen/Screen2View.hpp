#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>

class Screen2View : public Screen2ViewBase
{
public:
    Screen2View();
    virtual ~Screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void ExitFromScreen2();
    virtual void handleTickEvent();

protected:
    int birdX;
    int birdY;
    float birdVelocityY;

    int scrollSpeed;
    int backgroundX;
    float cloudX;
    int cloudY;

    int pipeX;
    int pipeGapY;
    int pipeGapSize;
    bool hasPassedPipe;

    int coinX;
    int coinY;
    bool isCoinActive;

    bool isDashing;
    int dashEnergy;
    int dashTimer;
    const int MAX_DASH_TIME = 150;
    int graceTimer;

    int score;
    bool isGameOver;
    int gameOverTimer;
    int countdownTimer;
    int soundCooldown;
};

#endif
