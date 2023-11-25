#pragma once

#include "geometry.hpp"
#include "ray_tracing.hpp"
#include "shapes.hpp"
#include "light.hpp"
#include "object_set.hpp"

#include "kdtree/kdtree.hpp"
#include "queue/concurrent_bounded_queue.hpp"
#include "progress_bar/text_progress_bar.hpp"

#include <thread>
#include <vector>

namespace PhotonMapping {

struct Photon
{
    Point position;
    Color flux;
    Real lat, az;

    Photon() = default;
    inline Photon(const Point& p, const Color& f, Real theta, Real phi)
        : position{p}, flux{f}, lat{theta}, az{phi}
    {}

    struct KDTreeAccessor
    {
        Real operator()(const Photon& photon, Index i) const
        {
            return photon.position[i];
        }
    };
};

using PhotonMap = nn::KDTree<Photon, 3, Photon::KDTreeAccessor>;

//void castPhotonsToScene(const ObjectSet& objSet, const Ray& ray,
//        PhotonMap& map, Randomizer& random);
//Color evalSceneLight(...);

class Renderer
{
private:

public:
    static constexpr Index totalConcurrency = 0;

    Renderer() {}

    void render(const Camera& cam, Image& img, const ObjectSet& objects,
            Index ppp, Index totalPhotons);
};

} //namespace PhotonMapping
