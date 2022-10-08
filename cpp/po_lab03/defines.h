#pragma once

enum class FileError {
    Success,
    FileClosed,
    OperationNotPermitted
};

struct Point {
    double x, y, z;

    Point() : x(0.0), y(0.0), z(0.0) {}
    Point(double x, double y, double z) : x(x), y(y), z(z) {}
};
