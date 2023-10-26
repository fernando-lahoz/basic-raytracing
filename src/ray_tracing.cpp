#include "ray_tracing.hpp"
#include "shapes.hpp"
#include "geometry.hpp"

Camera::Camera(Point pinhole, Direction front, Direction up, Dimensions dim)
    : f { front },
      l { normalize(cross(up, front)) * ((norm(up) * dim.width) / dim.height) },
      u { up },
      o { pinhole },
      pixelWidth{ 2.0 / dim.width }, pixelHeight{ 2.0 / dim.height },
      randomX{pixelWidth}, randomY{pixelHeight},
      rd{}
{
    gen.seed(rd());
}

Camera::Camera(const Camera& cam)
    : f{cam.f}, l{cam.l}, u{cam.u}, o{cam.o},
      pixelWidth{cam.pixelWidth}, pixelHeight{cam.pixelHeight},
      randomX{0, pixelWidth}, randomY{0, pixelHeight},
      rd{}
{
    gen.seed(rd());
}

Ray Camera::randomRay(Index i, Index j)
{
    Real y = static_cast<Real>(1 - pixelHeight * i - randomY(gen));
    Real x = static_cast<Real>(1 - pixelWidth * j - randomX(gen));

    return {o, normalize((x * l) + (y * u) + f)};
}
