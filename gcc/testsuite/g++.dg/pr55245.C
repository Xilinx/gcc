/* { dg-do compile } */
template <typename VType> class Vector2 { };
typedef Vector2<float> Vector2_f;

void GetR( const Vector2_f mosaic_position[3]);

class Test1 {
 private: virtual void TestBody();
};

void Test1::TestBody() {
  Vector2_f mosaic_position[2][1];   // (1)
}

class Test2 {
 private: virtual void TestBody();
};

int tri;
void Test2::TestBody() {
  Vector2_f mosaic_position[2][3] = { };
  GetR(mosaic_position[tri]);
}
