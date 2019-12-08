#ifndef sdp_lines
#define sdp_lines

#include "utils.h"
#include <random>
#define SDP_LU 1
#define SDP_U 2
#define SDP_RU 3
#define SDP_L 4
#define SDL_C 5
#define SDP_R 6
#define SDP_LD 7
#define SDP_D 8
#define SDP_RD 9

struct Point{
    int x;
    int y;
    cols c;
};

struct xy{
    int x;
    int y;
    int lastDir;
};

class Line{
    private:
        int visiblePoints;
        int maxVisPoint;
        float* pos;
        float* colors;
        double time;
        double offsetTime;
        double maxTime;
        double timeForPoint;
        bool toggle;
    public:
        Line(std::vector<Point> p, SDL_Surface* img, int maxPoints);
        ~Line();
        void update(double deltaTime, bool spacePress);
        void draw(float scale);
};

class Lines{
    private:
        std::vector<Line*> l;
        std::vector<std::vector<Point>> ps; 
        std::vector<std::vector<int>> partOfLine;
        xy nextPoint(std::vector<std::vector<cols>> pixels, int x, int y, int lastDir);
        void followLine(std::vector<std::vector<cols>> pixels, int startX, int startY);
    public:
        Lines(std::vector<std::vector<cols>> pixels, SDL_Surface* img);
        ~Lines();
        void update(double deltaTime, bool spacePress);
        void draw(float scale);
};

#endif