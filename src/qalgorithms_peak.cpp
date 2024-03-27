// qalgorithms_Peak.cpp

#include "../include/qalgorithms_Peak.h"

namespace q
{
  // Constructor
  Peak::Peak() : B({0, 0, 0, 0}) {}

  Peak::Peak(int index, int scale) : index(index), scale(scale), B({0, 0, 0, 0}) {}

  // Destructor
  Peak::~Peak() {}

  // methods
  // getters
  int Peak::getIndex() const { return index; }
  int Peak::getScale() const { return scale; }
  const std::array<double, 4> &Peak::getB() const { return B; }
  double Peak::getPositionNorm() const { return position_norm; }
  double Peak::getPosition() const { return position; }
  double Peak::getHeight() const { return height; }
  double Peak::getWidth() const { return width; }
  double Peak::getArea() const { return area; }
  int Peak::getDf() const { return df; }
  double Peak::getPositionUncertainty() const { return position_uncertainty; }
  double Peak::getHeightUncertainty() const { return height_uncertainty; }
  double Peak::getWidthUncertainty() const { return width_uncertainty; }
  double Peak::getAreaUncertainty() const { return area_uncertainty; }
  double Peak::getMse() const { return mse; }
  // setters
  void Peak::setIndex(int index) { this->index = index; }
  void Peak::setScale(int scale) { this->scale = scale; }
  void Peak::setB(const std::array<double, 4> &B) { this->B = B; }
  void Peak::setPositionNorm(double position_norm) { this->position_norm = position_norm; }
  void Peak::setPosition(double position) { this->position = position; }
  void Peak::setHeight(double height) { this->height = height; }
  void Peak::setWidth(double width) { this->width = width; }
  void Peak::setArea(double area) { this->area = area; }
  void Peak::setDf(int df) { this->df = df; }
  void Peak::setPositionUncertainty(double position_uncertainty) { this->position_uncertainty = position_uncertainty; }
  void Peak::setHeightUncertainty(double height_uncertainty) { this->height_uncertainty = height_uncertainty; }
  void Peak::setWidthUncertainty(double width_uncertainty) { this->width_uncertainty = width_uncertainty; }
  void Peak::setAreaUncertainty(double area_uncertainty) { this->area_uncertainty = area_uncertainty; }
  void Peak::setMse(double mse) { this->mse = mse; }
  // debugging
  void Peak::print() const
  {
    std::cout << "index: " << index << std::endl;
    std::cout << "scale: " << scale << std::endl;
    std::cout << "B: " << B[0] << ", " << B[1] << ", " << B[2] << ", " << B[3] << std::endl;
    std::cout << "position_norm: " << position_norm << std::endl;
    std::cout << "position: " << position << std::endl;
    std::cout << "height: " << height << std::endl;
    std::cout << "width: " << width << std::endl;
    std::cout << "area: " << area << std::endl;
    std::cout << "df: " << df << std::endl;
    std::cout << "position_uncertainty: " << position_uncertainty << std::endl;
    std::cout << "height_uncertainty: " << height_uncertainty << std::endl;
    std::cout << "width_uncertainty: " << width_uncertainty << std::endl;
    std::cout << "area_uncertainty: " << area_uncertainty << std::endl;
    std::cout << "mse: " << mse << std::endl;
  }
}