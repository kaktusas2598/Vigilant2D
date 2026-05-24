#pragma once

class GameClock {
    public:
        void update(float dt) {
            if (!running) {
                return;
            }

            accumulatedGameMinutes += dt * minutesPerRealSecond;

            while (accumulatedGameMinutes >= 1.0f) {
                accumulatedGameMinutes -= 1.0f;
                advanceMinute();
            }
        }

        void setMinutesPerRealSecond(float value) {
            minutesPerRealSecond = value > 0.0f ? value : 1.0f;
        }

        void setTime(int newDay, int newHour, int newMinute) {
            day = newDay;
            hour = newHour;
            minute = newMinute;
        }

        void setRunning(bool value) { running = value; }

        int getDay() const { return day; }
        int getHour() const { return hour; }
        int getMinute() const { return minute; }

        float getTimeOfDay01() const {
            return static_cast<float>(hour * 60 + minute) / 1440.0f;
        }

    private:
        void advanceMinute() {
            minute++;
            if (minute >= 60) {
                minute = 0;
                hour++;
            }

            if (hour >= 24) {
                hour = 0;
                day++;
            }
        }

        int day = 1;
        int hour = 6;
        int minute = 0;

        bool running = true;
        float minutesPerRealSecond = 1.0f;
        float accumulatedGameMinutes = 0.0f;
};