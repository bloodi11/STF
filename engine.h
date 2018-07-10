#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <string>
#include <memory>
#include <QString>
#include <iostream>



class Image;
class WorkSpace;


class JobReader{
public:
    void readData(Image& img, QString pathToRead);

};

class JobRotate{
public:
    float sinfi;
    float cosfi;
    float degrees;

    std::vector<float> rotationMatrix;

    void calculateRotationMatrix(WorkSpace& workspace);
};

class JobFindAxis{
public:
    int findXAxisLength(WorkSpace& workspace);
};


class JobScale{
public:
    float m;

    std::vector<float> scaleMatrix;

    void calculateScaleMatrix(WorkSpace& workspace);

};

class JobTranslate{
public:
    float tx;
    float ty;

    std::vector<float> translateMatrix;

    void calculateTranslateMatrix(WorkSpace& workspace);
};

class JobTransformation{
public:
    void calculateTransformation(WorkSpace& workspace);
};

class Pixel{
public:
    float x;
    float y;

    float r;
    float g;
    float b;
};

class JobGradient : public JobFindAxis{
public:
    void findCorners(WorkSpace& workspace);
};

class Image{
public:
    std::vector<Pixel> pixelData;
    std::vector<Pixel> corner;
};

class JobMatchingError{
public:
    float returnMatchingError(WorkSpace& workspace);
};

class WorkSpace : public JobReader, public JobGradient, public JobScale, public JobRotate, public JobTranslate, public JobTransformation, public JobMatchingError{
public:
    Image img1;
    Image img2;


    Pixel peak1;
    Pixel peak2;
    Pixel peak3;
    Pixel peak4;

    std::vector<float> transformationMatrix;

    WorkSpace();
    WorkSpace(QString path1, QString path2);
};




#endif // ENGINE_H
