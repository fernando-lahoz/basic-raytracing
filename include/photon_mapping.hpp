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

struct Task
{
    struct Indices { Index i, j; };
    Indices start;
    Indices end;
};

using TaskQueue = ConcurrentBoundedQueue<Task>;

class TaskDivider
{
private:
    Index i, j;
public:
    const Index width, height;
    const Index regionWidth, regionHeight;

    TaskDivider(Dimensions imageDim, Dimensions regionDim)
        : i{0}, j{0}, width{imageDim.width}, height{imageDim.height},
          regionWidth{regionDim.width}, regionHeight{regionDim.height} {}

    bool getNextTask(Task& task);
};

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

struct SPhoton
{
    Point position;
    Color flux;
    Real lat, az;
    const Shape* shape;

    SPhoton() = default;
    inline SPhoton(const Point& p, const Color& f,
            Real theta, Real phi, const Shape* ptr)
        : position{p}, flux{f}, lat{theta}, az{phi}, shape{ptr}
    {}

    struct KDTreeAccessor
    {
        Real operator()(const SPhoton& photon, Index i) const
        {
            return photon.position[i];
        }
    };
};

template <typename PhotonTy>
using PhotonMap = nn::KDTree<PhotonTy, 3, typename PhotonTy::KDTreeAccessor>;

class Renderer
{
private:
    std::vector<std::thread> threadPool;
    std::thread leader;
    TaskQueue tasks;
    TaskDivider taskDivider;

    template<typename PhotonTy>
    void renderSpecialized(const Camera& cam, Image& img, const ObjectSet& objects,
            Index ppp, Index totalPhotons, Real evalRadius, Index evalNumPhotons,
            bool nextEventEstimation);
public:
    static constexpr Index totalConcurrency = 0;

    Renderer(const Index numWorkers, const Index queueSize,
            const TaskDivider& divider);

    void render(const Camera& cam, Image& img, const ObjectSet& objects,
            Index ppp, Index totalPhotons, Real evalRadius,
            Index evalNumPhotons, bool nextEventEstimation,
            bool onlyCountSameShapePhotons);
    
    Index numThreads();
};

} //namespace PhotonMapping
