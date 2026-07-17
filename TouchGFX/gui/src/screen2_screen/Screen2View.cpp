#include <gui/screen2_screen/Screen2View.hpp>
#include <gui/common/FrontendApplication.hpp>
#include <BitmapDatabase.hpp>
#include <touchgfx/Unicode.hpp>

#include "main.h"
#include "cmsis_os.h"
#include <cstdlib>
#include <cstdio>

extern osMessageQueueId_t myQueue01Handle;

uint32_t xorshift32(void) {
    static uint32_t x = 314159265;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

Screen2View::Screen2View()
{
	carX = 50;
	carY = 160;
	birdVelocity = 0;

	pipeX = 320;
	gapY = 160;
	gapSize = 100;

	coinX = pipeX + 175;
	coinY = 160;
	isCoinActive = true;

	cloudX = 240;
	cloudY = 20;

	bgX = 0;
	speed = 3;
	score = 0;
	highScore = 0;
	gameOver = false;
	endGameDelay = 0;

	isDashing = false;
	dashPoints = 0;
	dashTimer = 0;

	graceTimer = 0;
	countdownTimer = 240;
}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();

    image1.setVisible(false);
    pipeUpper.setVisible(false);
    pipeLower.setVisible(false);
    coin.setVisible(false);
    dashBar.setVisible(false);
    txtScore.setVisible(false);
    imgDashing.setVisible(false);
    imgYouLose.setVisible(false);
    imgCloud.setVisible(false);
    imgCountdown.setVisible(true);


    int cloudType = xorshift32() % 3;
    if (cloudType == 0) imgCloud.setBitmap(touchgfx::Bitmap(BITMAP_MAY_1_ID));
    else if (cloudType == 1) imgCloud.setBitmap(touchgfx::Bitmap(BITMAP_MAY_2_ID));
    else imgCloud.setBitmap(touchgfx::Bitmap(BITMAP_MAY_3_ID));

    if (xorshift32() % 2 == 0) {
		image1.setBitmap(touchgfx::Bitmap(BITMAP_CHIM_MY_ID));
	} else {
		image1.setBitmap(touchgfx::Bitmap(BITMAP_CHIM_NHUNG_ID));
	}
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

void Screen2View::ExitFromScreen2(){
	if(score > gHighScore) gHighScore = score;

	carX = 50;
	carY = 160;
	birdVelocity = 0;

	pipeX = 320;
	gapY = 160;
	gapSize = 80;

	coinX = pipeX + 175;
	coinY = 160;
	isCoinActive = true;

	cloudX = 240;
	cloudY = 20 + (xorshift32() % 60);

	bgX = 0;
	speed = 3;
	score = 0;
	gameOver = false;

	isDashing = false;
	dashPoints = 0;
	dashTimer = 0;

	graceTimer = 0;
	countdownTimer = 240;

	imgCountdown.setVisible(true);
	imgDashing.setVisible(false);
	imgYouLose.setVisible(false);
	pipeUpper.setVisible(false);
	pipeLower.setVisible(false);
	image1.setVisible(false);
    coin.setVisible(false);
    dashBar.setVisible(false);
    txtScore.setVisible(false);
    imgCloud.setVisible(false);

	if (xorshift32() % 2 == 0) {
		image1.setBitmap(touchgfx::Bitmap(BITMAP_CHIM_MY_ID));
	} else {
		image1.setBitmap(touchgfx::Bitmap(BITMAP_CHIM_NHUNG_ID));
	}

	invalidate();

	uint16_t control;
	while(osMessageQueueGet(myQueue01Handle, &control, NULL, 0) == osOK){}
}

void Screen2View::handleTickEvent()
{
	Screen2ViewBase::handleTickEvent();

    if (gameOver){
        endGameDelay++;
        if (endGameDelay >= 180) {
            application().gotoScreen1ScreenNoTransition();
        }
        return;
    }

    if (countdownTimer > 0) {
        countdownTimer--;

        uint16_t control;
        while(osMessageQueueGet(myQueue01Handle, &control, NULL, 0) == osOK){}
n
        if (countdownTimer > 180) {
            imgCountdown.setBitmap(touchgfx::Bitmap(BITMAP_COUNT3_ID));
        } else if (countdownTimer > 120) {
            imgCountdown.setBitmap(touchgfx::Bitmap(BITMAP_COUNT2_ID));
        } else if (countdownTimer > 60) {
            imgCountdown.setBitmap(touchgfx::Bitmap(BITMAP_COUNT1_ID));
        } else {
            imgCountdown.setBitmap(touchgfx::Bitmap(BITMAP_START_ID));
        }

        imgCountdown.setWidth(touchgfx::Bitmap(imgCountdown.getBitmap()).getWidth());
		imgCountdown.setHeight(touchgfx::Bitmap(imgCountdown.getBitmap()).getHeight());

		int screenWidth = 240;
		int screenHeight = 320;

		int centeredX = (screenWidth - imgCountdown.getWidth()) / 2;
		int centeredY = (screenHeight - imgCountdown.getHeight()) / 2;

		imgCountdown.setXY(centeredX, centeredY);

        imgCountdown.invalidate();
        background1.invalidate();

        if (countdownTimer == 0) {
            imgCountdown.setVisible(false);

            image1.setVisible(true);
            pipeUpper.setVisible(true);
            pipeLower.setVisible(true);
            if(isCoinActive) coin.setVisible(true);
            dashBar.setVisible(true);
            txtScore.setVisible(true);
            imgCloud.setVisible(true);

            invalidate();
        }

        return;
    }

    image1.invalidate();
    pipeUpper.invalidate();
    pipeLower.invalidate();
    background1.invalidate();
    background2.invalidate();
    imgCloud.invalidate();
    dashBar.invalidate();
    if (isCoinActive) {
        coin.invalidate();
    }
    imgDashing.invalidate();

    uint16_t control;
    if (osMessageQueueGet(myQueue01Handle, &control, NULL, 0) == osOK){
        if (control == 1) {
            if (!isDashing) {
                birdVelocity = -2.0f;
            }
        }
        else if (control == 2) {
            if (dashPoints >= 8 && !isDashing && graceTimer <= 0) {
                isDashing = true;
                dashTimer = MAX_DASH_TIME;
                dashPoints = 0;

                int dashType = xorshift32() % 3;
				if (dashType == 0) imgDashing.setBitmap(touchgfx::Bitmap(BITMAP_DASHING_1_ID));
				else if (dashType == 1) imgDashing.setBitmap(touchgfx::Bitmap(BITMAP_DASHING_2_ID));
				else imgDashing.setBitmap(touchgfx::Bitmap(BITMAP_DASHING_3_ID));

				imgDashing.setWidth(touchgfx::Bitmap(imgDashing.getBitmap()).getWidth());
				int dashCenteredX = (240 - imgDashing.getWidth()) / 2;
				imgDashing.setXY(dashCenteredX, 60);
				imgDashing.setVisible(true);
            }
        }
    }


    if (isDashing) {
        carY = 160;
        birdVelocity = 0;
    } else {
        birdVelocity += 0.1f;
        carY += (int)birdVelocity;
    }

    if (carY < 0) {
        carY = 0;
        birdVelocity = 0;
    }

    if (carY > 280) {
        if (graceTimer <= 0) {
            gameOver = true;
            buzzer_state = BEEP_DEAD;
        } else {
            carY = 280;
        }
    }
    image1.setXY(carX, carY);

    int currentSpeed = speed;
    if (isDashing) {
        currentSpeed = 12;
        dashTimer--;

        int percent = (dashTimer * 100) / MAX_DASH_TIME;
        dashBar.setValue(percent);

        bool isBlink = (dashTimer % 10 < 5);
        pipeUpper.setVisible(isBlink);
        pipeLower.setVisible(isBlink);
        coin.setVisible(isBlink);

        if (dashTimer <= 0) {
            isDashing = false;
            graceTimer = 30;

            imgDashing.setVisible(false);
            pipeUpper.setVisible(true);
            pipeLower.setVisible(true);
            coin.setVisible(isCoinActive);
        }
    }
    else if (graceTimer > 0) {
        graceTimer--;

        image1.setVisible(graceTimer % 10 < 5);

        if (graceTimer <= 0) {
            image1.setVisible(true);
        }

        dashBar.setValue(0);
    }
    else {
        int percent = (dashPoints * 100) / 8;
        if (percent > 100) percent = 100;
        dashBar.setValue(percent);
    }

    bgX -= currentSpeed;
    if (bgX <= -240) bgX = 0;

    background1.setX(bgX);
    background2.setX(bgX + 240);

    cloudX -= (currentSpeed * 0.5f);
	if (cloudX < -60) {
		cloudX = 240;
		cloudY = 10 + (xorshift32() % 100);

		int cloudType = xorshift32() % 3;
		if (cloudType == 0) imgCloud.setBitmap(touchgfx::Bitmap(BITMAP_MAY_1_ID));
		else if (cloudType == 1) imgCloud.setBitmap(touchgfx::Bitmap(BITMAP_MAY_2_ID));
		else imgCloud.setBitmap(touchgfx::Bitmap(BITMAP_MAY_3_ID));
	}
	imgCloud.setXY((int)cloudX, cloudY);

    pipeX -= currentSpeed;
    coinX -= currentSpeed;

    if (pipeX < -30){
        pipeX = 320;
        gapY = 80 + (xorshift32() % 160);
        gapSize = 90 + (xorshift32() % 30);

        if (isDashing) {
            score += 2;
        } else {
            score += 1;
            if (dashPoints < 8) dashPoints += 1;
        }

        if (score % 5 == 0 && speed < 8) speed++;
        if (score > gHighScore) gHighScore = score;
    }

    if (coinX < -32) {
        coinX = pipeX + 175;
        coinY = 60 + (xorshift32() % 180);

        isCoinActive = true;
        coin.setVisible(true);
    }

    int upperY = gapY - (gapSize / 2) - 200;
    int lowerY = gapY + (gapSize / 2);

    pipeUpper.setXY(pipeX, upperY);
    pipeLower.setXY(pipeX, lowerY);

    if (isCoinActive) {
        coin.setXY(coinX, coinY);
    }


    int birdWidth = 35;
    int birdHeight = 32;
    int pipeWidth = 30;
    int pipeHeight = 200;
    int coinWidth = 32;
    int coinHeight = 32;

    bool hitTopPipe = (image1.getX() < pipeUpper.getX() + pipeWidth &&
                       image1.getX() + birdWidth > pipeUpper.getX() &&
                       image1.getY() < pipeUpper.getY() + pipeHeight &&
                       image1.getY() + birdHeight > pipeUpper.getY());

    bool hitBottomPipe = (image1.getX() < pipeLower.getX() + pipeWidth &&
                          image1.getX() + birdWidth > pipeLower.getX() &&
                          image1.getY() < pipeLower.getY() + pipeHeight &&
                          image1.getY() + birdHeight > pipeLower.getY());

    if(!isDashing && graceTimer <= 0 && (hitTopPipe || hitBottomPipe))
    {
        buzzer_state = BEEP_DEAD;
        gameOver = true;
    }

    if (isCoinActive) {
        bool hitCoin = (image1.getX() < coin.getX() + coinWidth &&
                        image1.getX() + birdWidth > coin.getX() &&
                        image1.getY() < coin.getY() + coinHeight &&
                        image1.getY() + birdHeight > coin.getY());

        if (hitCoin) {
            score += 4;
            if (!isDashing) {
                dashPoints += 4;
                if (dashPoints > 8) dashPoints = 8;
            }
            isCoinActive = false;
            coin.setVisible(false);
            if (score > gHighScore) gHighScore = score;
        }
    }

    if (gameOver) {
        endGameDelay = 0;
        Unicode::snprintf(txtFinalScoreBuffer, TXTFINALSCORE_SIZE, "%d", score);
        txtFinalScore.setVisible(true);
        txtFinalScore.invalidate();
        imgYouLose.setVisible(true);
        imgYouLose.invalidate();
        if(score >= gHighScore) {
            gHighScore = score;
        }
    }

    Unicode::snprintf(txtScoreBuffer, TXTSCORE_SIZE, "%d", score);
    txtScore.invalidate();

    image1.invalidate();
    pipeUpper.invalidate();
    pipeLower.invalidate();
    background1.invalidate();
    background2.invalidate();
    imgCloud.invalidate();
    dashBar.invalidate();
    imgDashing.invalidate();
    if (isCoinActive) {
        coin.invalidate();
    }
}
