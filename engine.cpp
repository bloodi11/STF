#include "engine.h"
#include <sstream>
#include <cmath>
#include <fstream>

void JobReader::readData(Image& img, QString pathToRead){
       std::ifstream file;
       std::string line;

       file.open(pathToRead.toStdString());
       if (!file.is_open()){
           std::cerr << "cannot open file cause it not exist" << std::endl;
           exit(404);
       } else
           while (file.good()){
               std::getline(file, line);
               std::istringstream in(line);

               auto pxTemp = std::make_unique<Pixel>();

               in >> pxTemp->x >> pxTemp->y >> pxTemp->r >> pxTemp->g >> pxTemp->b;
               img.pixelData.push_back(*pxTemp);

           } //WHILE
       img.pixelData.pop_back();
       file.close();

}

int JobFindAxis::findXAxisLength(WorkSpace &workspace){
    float y = 0;
    int i = 1;
    for(auto&& e : workspace.img1.pixelData){
        if(e.y > y){
            break;
        }
        i++;
    }
    return i-1;
}

void JobGradient::findCorners(WorkSpace& workspace){
    int xAxisLength = findXAxisLength(workspace);
    Pixel p0;
    p0.x = 0;

    Pixel pinf;
    pinf.x = xAxisLength;

    workspace.img1.corner.push_back(p0);
    workspace.img2.corner.push_back(p0);

    //MOST OUTER RIGHT CORNER
    int i = 0;
    for(auto&& e : workspace.img1.pixelData){
       if(e.r != 255 || e.g != 255 || e.b != 255){
              if(e.x > workspace.img1.corner.at(0).x){
                   if(workspace.img1.pixelData.at(i+xAxisLength).r == 255 && workspace.img1.pixelData.at(i+xAxisLength).g == 255 && workspace.img1.pixelData.at(i+xAxisLength).b == 255
                      && workspace.img1.pixelData.at(i+1).r == 255 && workspace.img1.pixelData.at(i+1).g == 255 && workspace.img1.pixelData.at(i+1).b == 255){
               workspace.img1.corner.pop_back();
               workspace.img1.corner.push_back(e);
                   }
               }         
       }
       i++;
    }

    i = 0;
    for(auto&& e : workspace.img2.pixelData){
       if(e.r != 255 || e.g != 255 || e.b != 255){
           if(e.x > workspace.img2.corner.at(0).x){
               if(workspace.img2.pixelData.at(i+xAxisLength).r == 255  && workspace.img2.pixelData.at(i+xAxisLength).g == 255 && workspace.img2.pixelData.at(i+xAxisLength).b == 255
                       && workspace.img2.pixelData.at(i+1).r == 255 && workspace.img2.pixelData.at(i+1).g == 255 && workspace.img2.pixelData.at(i+1).b == 255){
           workspace.img2.corner.pop_back();
           workspace.img2.corner.push_back(e);
               }
           }
       }
       i++;
    }

    //MOST OUTER LEFT CORNER
    i = 0;

    workspace.img1.corner.push_back(pinf);
    workspace.img2.corner.push_back(pinf);
    for(auto&& e : workspace.img1.pixelData){
       if(e.r != 255 || e.g != 255 || e.b != 255){
              if(e.x < workspace.img1.corner.at(1).x){
                   if(workspace.img1.pixelData.at(i-xAxisLength).r == 255 && workspace.img1.pixelData.at(i-xAxisLength).g == 255 && workspace.img1.pixelData.at(i-xAxisLength).b == 255
                           && workspace.img1.pixelData.at(i-1).r == 255 && workspace.img1.pixelData.at(i-1).g == 255 && workspace.img1.pixelData.at(i-1).b == 255){
               workspace.img1.corner.pop_back();
               workspace.img1.corner.push_back(e);
                   }
               }
       }
       i++;
    }

    i = 0;
    for(auto&& e : workspace.img2.pixelData){
       if(e.r != 255 || e.g != 255 || e.b != 255){
           if(e.x < workspace.img2.corner.at(1).x){
               if(workspace.img2.pixelData.at(i+xAxisLength).r == 255  && workspace.img2.pixelData.at(i+xAxisLength).g == 255 && workspace.img2.pixelData.at(i+xAxisLength).b == 255
                       && workspace.img2.pixelData.at(i-1).r == 255 && workspace.img2.pixelData.at(i-1).g == 255 && workspace.img2.pixelData.at(i-1).b == 255){
           workspace.img2.corner.pop_back();
           workspace.img2.corner.push_back(e);
               }
           }
       }
       i++;
    }

    workspace.peak1 = workspace.img1.corner.at(0);
    workspace.peak2 = workspace.img2.corner.at(0);
    workspace.peak3 = workspace.img1.corner.at(1);
    workspace.peak4 = workspace.img2.corner.at(1);

    std::cout << workspace.peak1.x << " " <<workspace.peak1.y << std::endl;
    std::cout <<workspace.peak2.x << " " << workspace.peak2.y <<std::endl;
    std::cout << workspace.peak3.x << " " <<workspace.peak3.y << std::endl;
    std::cout <<workspace.peak4.x << " " << workspace.peak4.y <<std::endl;
}


void JobTransformation::calculateTransformation(WorkSpace& workspace){                                          /*ADD THIS COLUMN*/
    std::vector<float> transformationMatrix = {workspace.m * workspace.cosfi, -(workspace.m * workspace.sinfi), workspace.tx,
                                               workspace.m * workspace.sinfi, workspace.m * workspace.cosfi,    workspace.ty};


    workspace.transformationMatrix = transformationMatrix;
}

void JobTranslate::calculateTranslateMatrix(WorkSpace& workspace){  
    tx = (workspace.peak2.x - workspace.m * workspace.peak1.x * workspace.cosfi + workspace.m * workspace.peak1.y * workspace.sinfi + workspace.peak4.x - workspace.m * workspace.peak3.x * workspace.cosfi + workspace.m * workspace.peak3.y * workspace.sinfi) / 2;
    ty = (workspace.peak2.y - workspace.m * workspace.peak1.x * workspace.sinfi - workspace.m * workspace.peak1.y * workspace.cosfi + workspace.peak4.y - workspace.m * workspace.peak3.x * workspace.sinfi - workspace.m * workspace.peak3.y * workspace.cosfi) / 2;

    std::vector<float> translateMatrix = { tx,
                                           ty };

    workspace.translateMatrix = translateMatrix;
}


void JobScale::calculateScaleMatrix(WorkSpace& workspace){
    float dist1x = workspace.peak1.x - workspace.peak3.x;
    float dist1y = workspace.peak1.y - workspace.peak3.y;
    float dist2x = workspace.peak2.x - workspace.peak4.x;
    float dist2y = workspace.peak2.y - workspace.peak4.y;
    float dist1 = sqrt(pow(dist1x, 2) + pow(dist1y, 2));
    float dist2 = sqrt(pow(dist2x, 2) + pow(dist2y, 2));

    workspace.m = dist2/  dist1;

    std::vector<float> scaleMatrix = {m, 0,
                                      0, m};


     workspace.scaleMatrix = scaleMatrix;
}

void JobRotate::calculateRotationMatrix(WorkSpace &workspace){
    float dist1x = workspace.peak1.x - workspace.peak3.x;
    float dist1y = workspace.peak1.y - workspace.peak3.y;
    float dist2x = workspace.peak2.x - workspace.peak4.x;
    float dist2y = workspace.peak2.y - workspace.peak4.y;

    float radians = atan2(dist1x, dist1y) - atan2(dist2x, dist2y);

    cosfi = cos(radians);
    sinfi = sin(radians);
    degrees = radians*180/3.14159265;

    std::vector<float> rotationMatrix = {cosfi, -sinfi,
                                         sinfi, cosfi};

    workspace.rotationMatrix = rotationMatrix;
}

float JobMatchingError::returnMatchingError(WorkSpace& workspace){  
    float vx1, vy1, vx2, vy2;

    vx1 = workspace.tx + workspace.m * workspace.peak1.x * workspace.cosfi - workspace.m * workspace.peak1.y * workspace.sinfi - 0.458;
    vy1 = workspace.ty + workspace.m * workspace.peak1.x * workspace.sinfi + workspace.m * workspace.peak1.y * workspace.cosfi - 0.165;
    vx2 = workspace.tx + workspace.m * workspace.peak3.x * workspace.cosfi - workspace.m * workspace.peak3.y * workspace.sinfi - 0.032;
    vy2 = workspace.ty + workspace.m * workspace.peak3.x * workspace.sinfi + workspace.m * workspace.peak3.y * workspace.cosfi - 0.255;

    for(auto&& e : workspace.img1.pixelData){
        if(e.r != 255 && e.g != 255 && e.b != 255){
       float qx= workspace.tx + workspace.m * e.x * workspace.cosfi - workspace.m * e.y * workspace.sinfi;
       float qy = workspace.ty + workspace.m * e.x * workspace.sinfi + workspace.m * e.y * workspace.cosfi;
       std:: cout << qx << " " << qy <<std::endl;
        }
    }

   // std::cout << vx1 + 0.458 << " " << vy1 + 0.165 << std::endl;
   // std::cout << vx2 + 0.032 << " " << vy2 + 0.255 << std::endl;
    float mE = (sqrt(vx1*vx1 + vy1*vy1) + sqrt(vx2*vx2 + vy2*vy2)) / 2;

    return mE;
}

WorkSpace::WorkSpace(QString path1, QString path2){
    readData(img1, path1);
    readData(img2, path2);
}

WorkSpace::WorkSpace(){

}
