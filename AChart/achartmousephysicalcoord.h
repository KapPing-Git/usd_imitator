#ifndef ACHARTMOUSEPHYSICALCOORD_H
#define ACHARTMOUSEPHYSICALCOORD_H

class AChartMousePhysicalCoord
{
public:
  AChartMousePhysicalCoord();

  void setXY_visible(bool visible);
  void setX_Visible(bool visible);
  void setY_Visible(bool visible);
  bool xyVisible();
  bool xVisible();
  bool yVisible();

  enum ValueOrientashion{ValueOrientashion_top,
                         ValueOrientashion_bottom,
                         ValueOrientashion_left,
                         ValueOrientashion_right};
  void setX_Orientashion(ValueOrientashion orientashion);
  void setY_Orientashion(ValueOrientashion orientashion);
  ValueOrientashion xOrientashion();
  ValueOrientashion yOrientashion();

  void setX_CoordDigitFormat(char f='f',int precision=1);
  void setY_CoordDigitFormat(char f='f',int precision=1);
  int xPrecision();
  int yPrecision();
  char xFormat();
  char yFormat();

private:
  bool m_xVisible;
  bool m_yVisible;
  ValueOrientashion m_xValueOrientashion;
  ValueOrientashion m_yYalueOrientashion;
  char m_xFormat;
  char m_yFormat;
  int  m_xPrecision;
  int  m_yPrecision;
};

#endif // ACHARTMOUSEPHYSICALCOORD_H
