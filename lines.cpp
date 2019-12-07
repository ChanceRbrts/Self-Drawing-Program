#include "lines.h"

Line::Line(std::vector<Point> p, SDL_Surface* img){
    pos = new float[p.size()*3];
    colors = new float[p.size()*3];
    for (int i = 0; i < p.size(); i++){
        pos[i*3] = p[i].x*2.0/(img->w*1.0)-1;
        pos[i*3+1] = 1-p[i].y*2.0/(img->h*1.0);
        pos[i*3+2] = 0;
        colors[i*3] = p[i].c.r;
        colors[i*3+1] = p[i].c.g;
        colors[i*3+2] = p[i].c.b;
    }
    visiblePoints = p.size();
}

Line::~Line(){
    delete pos;
    delete colors;
}

void Line::draw(float scale){
    glPointSize(scale);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, pos);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glDrawArrays(GL_POINTS, 0, visiblePoints);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

xy Lines::nextPoint(std::vector<std::vector<cols>> pixels, int x, int y, int lastDir){
    // This is grayscale, so r, g, and b are the same.
    float curCol = pixels[y][x].r;
    std::vector<Point> possiblePoints;
    // Let's get our neighboring 8 as possible candidates.
    for (int i = -1; i < 2; i++){
        for (int j = -1; j < 2; j++){
            if ((i != 0 || j != 0) && x+i >= 0 && x+i < pixels[y].size() && y+j >= 0 && y+j < pixels.size()){
                float otherCol = pixels[y+j][x+i].r;
                if (otherCol < thresh2 && !partOfLine[y+j][x+i]){
                    possiblePoints.push_back({i, j, pixels[y+j][x+i]});
                }
            }
        }
    }

    xy curPoint = {-1,-1};
    float maxLikelyPoints = 0;
    // Now, let's analyze them.
    for (int i = 0; i < possiblePoints.size(); i++){
        Point point = possiblePoints[i];
        float likelyPoints = 0;
        // A line likely has roughly the same thickness throughout.
        likelyPoints += 1-abs(point.c.r-curCol);
        // Find the direction compared to the point.
        int dir = 1+(point.y+1)*3+(point.x+1);
        // A line also likely goes the same direction as long as it can.
        if (dir == lastDir) likelyPoints += 0.2;
        // Update our most likely line continuation.
        if (likelyPoints > maxLikelyPoints){
            curPoint.x = x+point.x;
            curPoint.y = y+point.y;
            curPoint.lastDir = dir;
            maxLikelyPoints = likelyPoints;
        }
    }
    possiblePoints.clear();
    return curPoint;
}

void Lines::followLine(std::vector<std::vector<cols>> pixels, int startX, int startY, SDL_Surface* img){
    int curX = startX;
    int curY = startY;
    int dir = 0;
    int startDir = 0;
    bool contin = true;
    std::vector<Point> pts;
    while (contin){
        partOfLine[curY][curX] = true;
        pts.push_back({curX, curY, pixels[curY][curX]});
        // Find the next pixel to add to the line.
        xy nP = nextPoint(pixels, curX, curY, dir);
        if (nP.x == -1){
            // We didn't find another point.
            contin = false;
        }
        dir = nP.lastDir;
        if (startDir == 0) startDir = dir;
        curX = nP.x;
        curY = nP.y;
    }
    // It's possible that we landed in the middle of the line in some cases.
    // In that case, let's run it again, but put our pixels at the start of that line.
    contin = true;
    curX = startX;
    curY = startY;
    // Set the line in the opposite direction.
    dir = 8-(startDir-1)+1;
    std::vector<Point> reversePts;
    // Reverse our list.
    for (int i = pts.size()-1; i >= 0; i--){
        reversePts.push_back(pts[i]);
    }
    pts.clear();
    while (contin){
        xy nP = nextPoint(pixels, curX, curY, dir);
        if (nP.x == -1){
            // We didn't find another point.
            contin = false;
        } else {
            dir = nP.lastDir;
            curX = nP.x;
            curY = nP.y;
            partOfLine[curY][curX] = true;
            reversePts.push_back({curX, curY, pixels[curY][curX]});
        }
    } 
    // Reverse the list again.
    for (int i = reversePts.size()-1; i >= 0; i--){
        pts.push_back(reversePts[i]);
    }
    reversePts.clear();
    // Make our line!
    l.push_back(new Line(pts, img));
}

Lines::Lines(std::vector<std::vector<cols>> pixels, SDL_Surface* img){
    for (int i = 0; i < pixels.size(); i++){
        std::vector<bool> pLine;
        for (int j = 0; j < pixels[i].size(); j++){
            pLine.push_back(false);
        }
        partOfLine.push_back(pLine);
    }
    // Now, we actually have to separate the pixels into lines... :|
    for (int y = 0; y < pixels.size(); y++){
        for (int x = 0; x < pixels[y].size(); x++){
            if (!partOfLine[y][x] && pixels[y][x].r < thresh2){
                // Make a line.
                followLine(pixels, x, y, img);
            }
        }
    }
}

void Lines::draw(float scale){
    for (int i = 0; i < l.size(); i++){
        l[i]->draw(scale);
    }
}

Lines::~Lines(){
    for (int i = 0; i < l.size(); i++){
        delete l[i];
    }
    l.clear();
}
