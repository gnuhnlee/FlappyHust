#include <gui/screen3_screen/Screen3View.hpp>
#include "cmsis_os.h"

extern osMessageQueueId_t myQueue01Handle;

Screen3View::Screen3View()
{
	birdX = 10;
	birdY = 2200;
	birdVelocityY = 0;
	isDashing = false;
}

void Screen3View::setupScreen()
{
    Screen3ViewBase::setupScreen();
}

void Screen3View::tearDownScreen()
{
    Screen3ViewBase::tearDownScreen();

    uint16_t control;
    while(osMessageQueueGet(myQueue01Handle, &control, NULL, 0) == osOK){}
}

void Screen3View::handleTickEvent()
{
    Screen3ViewBase::handleTickEvent();

    birdImg.invalidate();

    uint16_t control;
    if (osMessageQueueGet(myQueue01Handle, &control, NULL, 0) == osOK){
        if (control == 1 && !isDashing) {
            birdVelocityY = -2.0f;
        }
        else if (control == 2 && !isDashing) {
            isDashing = true;
            birdY = 220;
            birdVelocityY = 0;
        }
    }


    if (isDashing) {

        birdX += 12;


        if (birdX > 240) {
            birdX = 10;
            isDashing = false;
        }
    } else {

        birdVelocityY += 0.1f;
        birdY += birdVelocityY;

        if (birdY > 220) {
            birdY = 220;
            birdVelocityY = 0;
        }
    }


    birdImg.setXY(birdX, (int)birdY);
    birdImg.invalidate();
}
