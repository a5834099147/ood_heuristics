///< 实现Bridget模式

class Drawing {
public: 
    virtual void drawLine(double x1, double y1, double x2, double y2) = 0;
};

class Shape{
public:
    Shape(Drawing *dp);
    virtual void draw() = 0;
private:
    Drawing *_dp;
};

Shape::Shape(Drawing *dp) 
{
    _dp = dp;
}





void main() 
{
    Shape *s1;
    Shape *s2;
    Drawing *dp1, *dp2;

    dp1 = new V1Drawing;
    s1 = new Rectangle(dp, 1, 1, 2, 2);

    dp2 = new V2Drawing;
    s2 = new Circle(dp, 2, 2, 4);

    s1->draw();
    s2->draw();

    delete s1;
    delete s2;
    delete dp1;
    delete dp2;
}