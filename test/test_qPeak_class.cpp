 #include "../include/qalgorithms_peak.h"

int main() {
  q::Peak peak;
  peak.setProperty(q::Property::CoeffB1,-.1);
  peak.setProperty(q::Property::CoeffB2,.1);
  peak.setProperty(q::Property::CoeffB3,.1);
  peak.print();
  peak.validate();
}