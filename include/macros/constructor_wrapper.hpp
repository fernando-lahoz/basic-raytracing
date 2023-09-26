#pragma once

#define CONSTRUCTOR_WRAPPER(Cc, Ty)     \
class Ty                                \
{                                       \
private:                                \
    const Cc& original;                 \
    friend class Cc;                    \
public:                                 \
    Ty(const Ty& c) = delete;           \
    Ty(Ty&& c) = delete;                \
    Ty(const Cc&& c) = delete;          \
    Ty(Cc&& c) = delete;                \
    Ty(const Cc& c) : original{c} {}    \
};                                      \
Cc(Ty&& original);
