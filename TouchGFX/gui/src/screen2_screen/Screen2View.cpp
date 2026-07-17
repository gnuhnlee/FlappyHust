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

	bgX = 0;
	speed = 3;
	score = 0;
	highScore = 0;
	gameOver = false;
	endGameDelay = 0;

}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();

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
	coin.setVisible(true);

	bgX = 0;
	speed = 3;
	score = 0;
	gameOver = false;

	if (xorshift32() % 2 == 0) {
		image1.setBitmap(touchgfx::Bitmap(BITMAP_CHIM_MY_ID));
	} else {
		image1.setBitmap(touchgfx::Bitmap(BITMAP_CHIM_NHUNG_ID));
	}
	image1.invalidate();

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

	    image1.invalidate();
		pipeUpper.invalidate();
		pipeLower.invalidate();
		background1.invalidate();
		background2.invalidate();
		if (isCoinActive) {
			coin.invalidate();
		}

	    uint16_t control;
	        if (osMessageQueueGet(myQueue01Handle, &control, NULL, 0) == osOK){
	            // Bất kỳ nút nào (control 1 hoặc 2) đều làm chim bay lên
	            birdVelocity = -2.0f; // Vận tốc âm để bay lên (thay đổi số này để chỉnh độ cao khi nhảy)
	            //buzzer_state = BEEP_FLAP;
	        }


	        // 2. VẬT LÝ FLAPPY BIRD (TRỌNG LỰC)
	        birdVelocity += 0.4f; // Trọng lực kéo xuống mỗi frame
	        carY += (int)birdVelocity;

	        // Giới hạn trần nhà và mặt đất
	        if (carY < 0) {
	            carY = 0;
	            birdVelocity = 0;
	        }
	        // Giả sử màn hình cao 320. Chạm đất là Game Over
	        if (carY > 280) {
	            gameOver = true;
	            buzzer_state = BEEP_DEAD;
	        }

	        image1.setXY(carX, carY);

	        // 3. CUỘN HÌNH NỀN THEO CHIỀU NGANG
	        bgX -= speed;
	        if (bgX <= -240) bgX = 0; // Thay đổi tùy theo kích thước ảnh nền của bạn

	        // Cần đổi setY thành setX cho các ảnh nền nếu muốn cuộn ngang
	        background1.setX(bgX);
	        background2.setX(bgX + 240);
	        // (Tuỳ chỉnh khoảng cách cộng thêm tương ứng với độ rộng ảnh track)

	        pipeX -= speed;
			coinX -= speed;


	        if (pipeX < -30){
				pipeX = 320;

				gapY = 80 + (xorshift32() % 160);
				gapSize = 90 + (xorshift32() % 30);

				score++;

				if (score % 5 == 0 && speed < 8) speed++;
				if (score > gHighScore) gHighScore = score;
			}

	        if (coinX < -32) {
				// Đặt coin vào vị trí mới (giữa khoảng trống tiếp theo)
				coinX = pipeX + 175;

				// Random độ cao của coin để người chơi phải bay lên/xuống (từ Y=60 đến Y=240)
				coinY = 60 + (xorshift32() % 180);

				isCoinActive = true;
				coin.setVisible(true);
			}

	            // Tính toán trục Y cho 2 ống dựa vào chiều cao 200px của ống
	            int upperY = gapY - (gapSize / 2) - 200; // Ống trên bị đẩy lên trên
	            int lowerY = gapY + (gapSize / 2);       // Ống dưới bị đẩy xuống dưới

	            pipeUpper.setXY(pipeX, upperY);
	            pipeLower.setXY(pipeX, lowerY);

	            if (isCoinActive) {
					coin.setXY(coinX, coinY);
				}

	            // 5. XÉT VA CHẠM (COLLISION)
	            // Giả định chim (image1) kích thước 30x50, Ống kích thước 30x200
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

	            if(hitTopPipe || hitBottomPipe)
	            {
	            	buzzer_state = BEEP_DEAD;
	                gameOver = true;
	                //buzzer_state = BEEP_DEAD;
	            }

	            if (isCoinActive) {
					bool hitCoin = (image1.getX() < coin.getX() + coinWidth &&
									image1.getX() + birdWidth > coin.getX() &&
									image1.getY() < coin.getY() + coinHeight &&
									image1.getY() + birdHeight > coin.getY());

					if (hitCoin) {
						score += 4;               // Cộng 4 điểm khi ăn coin
						isCoinActive = false;     // Tắt trạng thái hoạt động
						coin.setVisible(false);   // Ẩn coin đi
						if (score > gHighScore) gHighScore = score;
					}
				}

	            // 6. XỬ LÝ GAME OVER UI
	            if (gameOver) {
	                endGameDelay = 0;
	                Unicode::snprintf(txtFinalScoreBuffer, TXTFINALSCORE_SIZE, "%d", score);
	                txtFinalScore.setVisible(true);
	                txtFinalScore.invalidate();
	                if(score >= gHighScore) {
	                    gHighScore = score;
	                    //boxRecord.setVisible(true);
	                    //boxRecord.invalidate();
	                } else {
	                    //boxNormal.setVisible(true);
	                    //boxNormal.invalidate();
	                }
	            }

	            // 7. CẬP NHẬT GIAO DIỆN
	            Unicode::snprintf(txtScoreBuffer, TXTSCORE_SIZE, "%d", score);
	            txtScore.invalidate();

	            image1.invalidate();
	            pipeUpper.invalidate();
	            pipeLower.invalidate();
	            background1.invalidate();
	            background2.invalidate();
	            if (isCoinActive) {
					coin.invalidate();
				}
}
