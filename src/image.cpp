#include "image.hpp"
#include "tone_mapping.hpp"

void Image::toneMap(const ToneMappingStrategy& func)
{
    Image& img = *this;
    for (Index i = 0; i < img.pixels(); ++i)
    {
        auto [h, s, v] = HSVPixel::fromRGB(img(i));
        img(i) = HSVPixel::toRGB({h, s, func(img, v)});
    }
    maxLuminance = 1;
}