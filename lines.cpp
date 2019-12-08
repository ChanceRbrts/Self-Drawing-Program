#include "lines.h"

Line::Line(std::vector<Point> p, SDL_Surface* img, int maxPoints){
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
    visiblePoints = 0;
    maxVisPoint = p.size();
    time = 0;
    maxTime = maxPoints*timeForPoint+1;
    timeForPoint = 0.0025;
    toggle = true;
    float randomMult = rand()/(1.0*RAND_MAX);
    if (p.size() < 30){
        // If we have less than 30 points, let's just draw it after the max points.
        offsetTime = maxPoints*timeForPoint-1+randomMult*2.0;
    } else{
        // If we have the max amount of points, we want to start immediately.
        // If we have a mid amount of points, we want to delay stuff a bit.
        // At the same time, we want this to finish as the final line is finishing with it's drawing.
        offsetTime = randomMult*timeForPoint*(maxPoints-p.size());
    }
}

Line::~Line(){
    delete pos;
    delete colors;
}

void Line::update(double deltaTime, bool spacePress){
    // Space changes whether the lines and drawn or erased.
    if (toggle){
        time += deltaTime;
        if (time > maxTime) time = maxTime; 
    } else {
        time -= deltaTime;
        if (time < 0) time = 0;
    }
    visiblePoints = (time/timeForPoint)-offsetTime;
    if (visiblePoints > maxVisPoint){
        visiblePoints = maxVisPoint;
    } else if (visiblePoints < 0){
        visiblePoints = 0;
    }
    if (spacePress) toggle = !toggle;
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
                if (otherCol < thresh2 && partOfLine[y+j][x+i] == -1){
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

void Lines::followLine(std::vector<std::vector<cols>> pixels, int startX, int startY){
    int curX = startX;
    int curY = startY;
    int dir = 0;
    int startDir = 0;
    bool contin = true;
    std::vector<Point> pts;
    while (contin){
        partOfLine[curY][curX] = -2;
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
            partOfLine[curY][curX] = -2;
            reversePts.push_back({curX, curY, pixels[curY][curX]});
        }
    } 
    // Reverse the list again.
    for (int i = reversePts.size()-1; i >= 0; i--){
        pts.push_back(reversePts[i]);
    }
    reversePts.clear();
    int newID = ps.size();
    // It turns out that there are many "lines" that are only 1-30 pixels long. Let's add those into our real lines.
    if (ps.size() != 0 && pts.size() < 30){
        // Find a line that is a pixel away from one of the points.
        for (int i = 0; i < pts.size(); i++){
            int pX = pts[i].x;
            int pY = pts[i].y;
            // Check adjacent points and see what lines they belong to.
            for (int x = -1; x < 2; x++){
                for (int y = -1; y < 2; y++){
                    if (x != 0 || y != 0){
                        if (pY+y > 0 && pY+y < partOfLine.size() && pX+x > 0 && pX+x < partOfLine[0].size() 
                            && partOfLine[pY+y][pX+x] > -1 && newID == ps.size()){
                            // This point belongs to another line.
                            newID = partOfLine[pY+y][pX+x];
                        }
                    }
                }
            }
            if (newID != ps.size()) break;
        }   
    }
    if (newID == ps.size()){
        std::vector<Point> p;
        ps.push_back(p);
    }
    for (int i = 0; i < pts.size(); i++){
        ps[newID].push_back(pts[i]);
        partOfLine[pts[i].y][pts[i].x] = newID;
    }
    //l.push_back(new Line(pts));
}

Lines::Lines(std::vector<std::vector<cols>> pixels, SDL_Surface* img){
    for (int i = 0; i < pixels.size(); i++){
        std::vector<int> pLine;
        for (int j = 0; j < pixels[i].size(); j++){
            pLine.push_back(-1);
        }
        partOfLine.push_back(pLine);
    }
    // Now, we actually have to separate the pixels into lines... :|
    for (int y = 0; y < pixels.size(); y++){
        for (int x = 0; x < pixels[y].size(); x++){
            if (partOfLine[y][x] == -1 && pixels[y][x].r < thresh2){
                // Make a line.
                followLine(pixels, x, y);
            }
        }
    }
    // Let's get the max amount of points in a line.
    int maxPoints = 0;
    for (int i = 0; i < ps.size(); i++){
        if (ps[i].size() > maxPoints){
            maxPoints = ps[i].size();
        }
    }
    // Now, with our points, we make the lines.
    for (int i = 0; i < ps.size(); i++){
        l.push_back(new Line(ps[i], img, maxPoints));
    }
}

void Lines::update(double deltaTime, bool spacePress){
    for (int i = 0; i < l.size(); i++){
        l[i]->update(deltaTime, spacePress);
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
