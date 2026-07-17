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
    virtual void handleTickEvent() override;
    virtual void ExitFromScreen2() override;
protected:
    int16_t localImageX;
    uint32_t tickCount;

    int carX;
    int carY;

    int lambX;
    int lambY;

    int lamb_1X;
    int lamb_1Y;
    int trackY;

    int speed;
    int score;
    int highScore;
    bool gameOver;
    int endGameDelay;

    float birdVelocity; // Quản lý vận tốc rơi/nhảy của chim
    int pipeX;        // Trục X chung cho cả 2 ống
	int gapY;         // Vị trí tâm của khoảng trống
	int gapSize;      // Độ rộng của khoảng trống (có thể thay đổi để tăng độ khó)
	int bgX;

	int coinX;
	int coinY;
	bool isCoinActive;

};

#endif // SCREEN2VIEW_HPP
