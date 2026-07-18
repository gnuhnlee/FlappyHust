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
    birdX = 50;
    birdY = 160;
    birdVelocityY = 0;

    pipeX = 320;
    pipeGapY = 160;
    pipeGapSize = 100;
    hasPassedPipe = false;

    coinX = pipeX + 175;
    coinY = 160;
    isCoinActive = true;

    cloudX = 240;
    cloudY = 20;
    backgroundX = 0;
    scrollSpeed = 3;

    score = 0;
    gHighScore = 0;
    isGameOver = false;
    gameOverTimer = 0;
    countdownTimer = 240;

    isDashing = false;
    dashEnergy = 0;
    dashTimer = 0;
    graceTimer = 0;

    soundCooldown = 0;
    buzzer_state = 0;
}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();

    birdImg.setVisible(false);
    pipeUpper.setVisible(false);
    pipeLower.setVisible(false);
    coinImg.setVisible(false);
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
        birdImg.setBitmap(touchgfx::Bitmap(BITMAP_CHIM_MY_ID));
    } else {
        birdImg.setBitmap(touchgfx::Bitmap(BITMAP_CHIM_NHUNG_ID));
    }
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

void Screen2View::ExitFromScreen2(){
    if(score > gHighScore) gHighScore = score;

    birdX = 50;
    birdY = 160;
    birdVelocityY = 0;

    pipeX = 320;
    pipeGapY = 160;
    pipeGapSize = 80;
    hasPassedPipe = false;

    coinX = pipeX + 175;
    coinY = 160;
    isCoinActive = true;

    cloudX = 240;
    cloudY = 20 + (xorshift32() % 60);
    backgroundX = 0;
    scrollSpeed = 3;

    score = 0;
    isGameOver = false;
    gameOverTimer = 0;
    countdownTimer = 240;

    isDashing = false;
    dashEnergy = 0;
    dashTimer = 0;
    graceTimer = 0;

    soundCooldown = 0;
    buzzer_state = 0;

    imgCountdown.setVisible(true);
    imgDashing.setVisible(false);
    imgYouLose.setVisible(false);
    pipeUpper.setVisible(false);
    pipeLower.setVisible(false);
    birdImg.setVisible(false);
    coinImg.setVisible(false);
    dashBar.setVisible(false);
    txtScore.setVisible(false);
    imgCloud.setVisible(false);

    if (xorshift32() % 2 == 0) {
        birdImg.setBitmap(touchgfx::Bitmap(BITMAP_CHIM_MY_ID));
    } else {
        birdImg.setBitmap(touchgfx::Bitmap(BITMAP_CHIM_NHUNG_ID));
    }

    invalidate();

    uint16_t control;
    while(osMessageQueueGet(myQueue01Handle, &control, NULL, 0) == osOK){}
}

void Screen2View::handleTickEvent()
{
    Screen2ViewBase::handleTickEvent();

    if (soundCooldown > 0) {
        soundCooldown--;
    }

    if (isGameOver){
        gameOverTimer++;
        if (gameOverTimer >= 180) {
            application().gotoScreen1ScreenNoTransition();
        }
        return;
    }

    if (countdownTimer > 0) {
        countdownTimer--;

        if (countdownTimer == 239 || countdownTimer == 179 || countdownTimer == 119) {
            if (soundCooldown == 0) {
                buzzer_state = BEEP_COUNTDOWN;
                soundCooldown = 15;
            }
        }
        else if (countdownTimer == 59) {
            if (soundCooldown == 0) {
                buzzer_state = BEEP_START;
                soundCooldown = 15;
            }
        }

        uint16_t control;
        while(osMessageQueueGet(myQueue01Handle, &control, NULL, 0) == osOK){}

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

            birdImg.setVisible(true);
            pipeUpper.setVisible(true);
            pipeLower.setVisible(true);
            if(isCoinActive) coinImg.setVisible(true);
            dashBar.setVisible(true);
            txtScore.setVisible(true);
            imgCloud.setVisible(true);

            invalidate();
        }
        return;
    }

    birdImg.invalidate();
    pipeUpper.invalidate();
    pipeLower.invalidate();
    background1.invalidate();
    background2.invalidate();
    imgCloud.invalidate();
    dashBar.invalidate();
    if (isCoinActive) {
        coinImg.invalidate();
    }
    imgDashing.invalidate();

    uint16_t control;
    if (osMessageQueueGet(myQueue01Handle, &control, NULL, 0) == osOK){
        if (control == 1) {
            if (!isDashing) {
                birdVelocityY = -2.0f;
            }
        }
        else if (control == 2) {
            if (dashEnergy >= 8 && !isDashing && graceTimer <= 0) {
                isDashing = true;
                dashTimer = MAX_DASH_TIME;
                dashEnergy = 0;

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
        birdY = 160;
        birdVelocityY = 0;
    } else {
        birdVelocityY += 0.1f;
        birdY += (int)birdVelocityY;
    }

    if (birdY < 0) {
        birdY = 0;
        birdVelocityY = 0;
    }

    if (birdY > 280) {
        if (graceTimer <= 0) {
            isGameOver = true;
            buzzer_state = BEEP_DEAD;
            soundCooldown = 180;
        } else {
            birdY = 280;
        }
    }
    birdImg.setXY(birdX, birdY);

    int currentScrollSpeed = scrollSpeed;
    if (isDashing) {
        currentScrollSpeed = 12;
        dashTimer--;

        int percent = (dashTimer * 100) / MAX_DASH_TIME;
        dashBar.setValue(percent);

        bool isBlink = (dashTimer % 10 < 5);
        pipeUpper.setVisible(isBlink);
        pipeLower.setVisible(isBlink);
        coinImg.setVisible(isBlink);

        if (dashTimer <= 0) {
            isDashing = false;
            graceTimer = 30;

            imgDashing.setVisible(false);
            pipeUpper.setVisible(true);
            pipeLower.setVisible(true);
            coinImg.setVisible(isCoinActive);
        }
    }
    else if (graceTimer > 0) {
        graceTimer--;
        birdImg.setVisible(graceTimer % 10 < 5);

        if (graceTimer <= 0) {
            birdImg.setVisible(true);
        }
        dashBar.setValue(0);
    }
    else {
        int percent = (dashEnergy * 100) / 8;
        if (percent > 100) percent = 100;
        dashBar.setValue(percent);
    }

    backgroundX -= currentScrollSpeed;
    if (backgroundX <= -240) backgroundX = 0;
    background1.setX(backgroundX);
    background2.setX(backgroundX + 240);

    cloudX -= (currentScrollSpeed * 0.5f);
    if (cloudX < -60) {
        cloudX = 240;
        cloudY = 10 + (xorshift32() % 100);

        int cloudType = xorshift32() % 3;
        if (cloudType == 0) imgCloud.setBitmap(touchgfx::Bitmap(BITMAP_MAY_1_ID));
        else if (cloudType == 1) imgCloud.setBitmap(touchgfx::Bitmap(BITMAP_MAY_2_ID));
        else imgCloud.setBitmap(touchgfx::Bitmap(BITMAP_MAY_3_ID));
    }
    imgCloud.setXY((int)cloudX, cloudY);

    pipeX -= currentScrollSpeed;
    coinX -= currentScrollSpeed;

    if (pipeX < -30){
        pipeX = 320;
        pipeGapY = 80 + (xorshift32() % 160);
        pipeGapSize = 90 + (xorshift32() % 30);
        hasPassedPipe = false;
    }

    if (!hasPassedPipe && pipeX < 20) {
        hasPassedPipe = true;

        if (isDashing) {
            score += 2;
        } else {
            score += 1;
            if (soundCooldown == 0) {
                buzzer_state = BEEP_POINT;
                soundCooldown = 15;
            }
            if (dashEnergy < 8) dashEnergy += 1;
        }

        scrollSpeed = 3 + (score / 5);
        if (scrollSpeed > 8) scrollSpeed = 8;
        if (score > gHighScore) gHighScore = score;
    }

    if (coinX < -32) {
        coinX = pipeX + 175;
        coinY = 60 + (xorshift32() % 180);
        isCoinActive = true;
        coinImg.setVisible(true);
    }

    int upperY = pipeGapY - (pipeGapSize / 2) - 200;
    int lowerY = pipeGapY + (pipeGapSize / 2);

    pipeUpper.setXY(pipeX, upperY);
    pipeLower.setXY(pipeX, lowerY);

    if (isCoinActive) {
        coinImg.setXY(coinX, coinY);
    }

    int birdWidth = 35;
    int birdHeight = 32;
    int pipeWidth = 30;
    int pipeHeight = 200;
    int coinWidth = 32;
    int coinHeight = 32;

    bool hitTopPipe = (birdImg.getX() < pipeUpper.getX() + pipeWidth &&
                       birdImg.getX() + birdWidth > pipeUpper.getX() &&
                       birdImg.getY() < pipeUpper.getY() + pipeHeight &&
                       birdImg.getY() + birdHeight > pipeUpper.getY());

    bool hitBottomPipe = (birdImg.getX() < pipeLower.getX() + pipeWidth &&
                          birdImg.getX() + birdWidth > pipeLower.getX() &&
                          birdImg.getY() < pipeLower.getY() + pipeHeight &&
                          birdImg.getY() + birdHeight > pipeLower.getY());

    if(!isDashing && graceTimer <= 0 && (hitTopPipe || hitBottomPipe)) {
        isGameOver = true;
        buzzer_state = BEEP_DEAD;
        soundCooldown = 180;
    }

    if (isCoinActive) {
        bool hitCoin = (birdImg.getX() < coinImg.getX() + coinWidth &&
                        birdImg.getX() + birdWidth > coinImg.getX() &&
                        birdImg.getY() < coinImg.getY() + coinHeight &&
                        birdImg.getY() + birdHeight > coinImg.getY());

        if (hitCoin) {
            score += 4;

            if (soundCooldown == 0) {
                buzzer_state = BEEP_COIN;
                soundCooldown = 15;
            }

            if (!isDashing) {
                dashEnergy += 4;
                if (dashEnergy > 8) dashEnergy = 8;
            }

            isCoinActive = false;
            coinImg.setVisible(false);

            scrollSpeed = 3 + (score / 5);
            if (scrollSpeed > 8) scrollSpeed = 8;
            if (score > gHighScore) gHighScore = score;
        }
    }

    if (isGameOver) {
        gameOverTimer = 0;
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

    birdImg.invalidate();
    pipeUpper.invalidate();
    pipeLower.invalidate();
    background1.invalidate();
    background2.invalidate();
    imgCloud.invalidate();
    dashBar.invalidate();
    imgDashing.invalidate();

    if (isCoinActive) {
        coinImg.invalidate();
    }
}
