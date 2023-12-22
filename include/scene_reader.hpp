#pragma once

#include "object_set.hpp"

#include <optional>
#include <string>
#include <sstream>
#include <fstream>

struct Scene
{
    Point focus;
    Direction front, up;
    ObjectSet objects; 
};

std::optional<Scene> makeSceneFromFile(std::string_view file_name)
{
    std::ifstream is{std::string{file_name}};

    if (!is.is_open())
        return std::nullopt;

    // Aux functions
    auto getline = [](std::istream& is, std::string& buff)
    {
        buff.clear();
        for(;;)
        {
            int c = is.get();
            if (c == '\r')
            {
                if (is.peek() == '\n')
                    is.get();
                return;
            }   
            else if (c == '\n') { return; }
            else if (c == std::istream::traits_type::eof()) { return; }
            else { buff.push_back(char(c)); } 
        }
    };

    auto checkEOL = [](std::string_view sv, std::size_t pos)
    {
        auto remaining = sv.find_first_not_of(" \t", pos);
        return (remaining == std::string_view::npos);
    };

    auto getNextNoEmptyLine = [&](std::stringstream& buffer)
    {   
        std::string line;
        do {
            do {
                getline(is, line);
                if (line.length() == 0 && !is)
                    return false;
            }
            while (line.length() == 0);
        }
        while (std::string{" \t"}.find(line[0]) != std::string::npos && checkEOL(line, 1));
        buffer = std::stringstream{line};
        return true;
    };

    auto parseVec = [&]<typename Ty>(Ty& value) -> bool
    {
        Vec3 aux;
        is >> aux[0] >> aux[1] >> aux[2];
        if (is)
        {
            value = Ty{aux[0], aux[1], aux[2]};
            return true;
        }
        return false;
    };

    auto parseReal = [&]<typename Ty>(Ty& value) -> bool
    {
        Real aux;
        is >> aux;
        if (is)
        {
            value = Ty{aux};
            return true;
        }
        return false;
    };

    bool foundCamera = false;
    std::vector<std::pair<const std::string, std::shared_ptr<const Material>>> materials;

    auto findMaterial = [&materials](const std::string& id) -> std::shared_ptr<const Material>
    {
        for (auto& [mId, ptr] : materials)
            if (id == mId)
                return ptr;
        return nullptr;
    };

    auto parseMaterial = [&](const std::string& type) -> std::shared_ptr<const Material>
    {
        std::string word;
        if (type == "Shade") {
            Color k;
            Real index;
            Material material;
            bool ok = false;
            do {
                if (!(is >> word)) return nullptr;
                if (word == "diffuse:")
                {
                    if (!parseVec(k)) return nullptr;
                    material = material + diffuse(k);
                }
                else if (word == "specular:")
                {
                    if (!parseVec(k)) return nullptr;
                    material = material + specular(k);
                }
                else if (word == "refractive:")
                {
                    if (!parseVec(k) || !(is >> word)) return nullptr;
                    if (word == "ior:" || word == "index:")
                    {
                        if (!parseReal(index)) return nullptr;
                        material = material + refractive(k, index);
                    }
                    else return nullptr;
                }
                else if (word == "emission:")
                {
                    if (!parseVec(k)) return nullptr;
                    material = material + emitter(k);
                }
                else ok = (word == "}");
            } while (!ok);
            getline(is, word);
            return std::make_shared<const Material>(material);
        }

        return nullptr;
    };

    Scene scene;
    std::stringstream buffer;
    while (getNextNoEmptyLine(buffer)) {
        std::string word;

        if (!(buffer >> word) || word[0] == '#' || word[0] == '}')
            continue;

        if (word == "Camera")
        {
            foundCamera = true;
            bool gotfocus = false, gotfront = false, gotup = false;
            do {
                if (!(is >> word)) return std::nullopt;
                if (word == "focus:")
                {
                    gotfocus = true;
                    if (!parseVec(scene.focus))
                        return std::nullopt;
                }
                else if (word == "front:")
                {
                    gotfront = true;
                    if (!parseVec(scene.front))
                        return std::nullopt;
                }
                else if (word == "up:")
                {
                    gotup = true;
                    if (!parseVec(scene.up))
                        return std::nullopt;
                }
            } while (!gotfocus || !gotfront || !gotup);

            getline(is, word);
        }
        else if (word == "Light")
        {
            Point point;
            Color emission;
            bool gotPoint = false, gotEmission = false;
            do {
                if (!(is >> word)) return std::nullopt;
                if (word == "point:")
                {
                    gotPoint = true;
                    if (!parseVec(point))
                        return std::nullopt;
                }
                else if (word == "emission:")
                {
                    gotEmission = true;
                    if (!parseVec(emission))
                        return std::nullopt;
                }
            } while (!gotPoint || !gotEmission);
            scene.objects.pointLights.emplace_back(point, emission);
            getline(is, word);
        }
        else if (word == "Sphere")
        {
            Point center;
            Real radius;
            std::shared_ptr<const Material> material;
            bool readNextWord = true;
            bool gotCenter = false, gotRadius = false, gotMaterial = false;
            do {
                if (readNextWord && !(is >> word))
                    return std::nullopt;
                readNextWord = true;
                if (word == "center:")
                {
                    gotCenter = true;
                    if (!parseVec(center))
                        return std::nullopt;
                }
                else if (word == "radius:")
                {
                    gotRadius = true;
                    if (!parseReal(radius))
                        return std::nullopt;
                }
                else if (word == "material:")
                {
                    gotMaterial = true;
                    std::string type;
                    if (!(is >> type) || !(is >> word))
                        return std::nullopt;
                    if (word == "{") {
                        material = parseMaterial(type);
                    }
                    else {
                        material = findMaterial(type);
                        readNextWord = false;
                    }
                }
            } while (!gotCenter || !gotRadius || !gotMaterial);
            if (!material) return std::nullopt;
            scene.objects.objects.emplace_back(
                std::make_shared<Sphere>(center, radius), material
            );
            getline(is, word);
        }
        else if (word == "Plane")
        {
            Point point;
            Direction normal;
            std::shared_ptr<const Material> material;
            bool readNextWord = true;
            bool gotPoint = false, gotNormal = false, gotMaterial = false;
            do {
                if (readNextWord && !(is >> word))
                    return std::nullopt;
                readNextWord = true;
                if (word == "point:")
                {
                    gotPoint = true;
                    if (!parseVec(point))
                        return std::nullopt;
                }
                else if (word == "normal:")
                {
                    gotNormal = true;
                    if (!parseVec(normal))
                        return std::nullopt;
                }
                else if (word == "material:")
                {
                    gotMaterial = true;
                    std::string type;
                    if (!(is >> type) || !(is >> word))
                        return std::nullopt;
                    if (word == "{") {
                        material = parseMaterial(type);
                    }
                    else {
                        material = findMaterial(type);
                        readNextWord = false;
                    }
                }
            } while (!gotPoint || !gotNormal || !gotMaterial);
            if (!material) return std::nullopt;
            scene.objects.objects.emplace_back(
                std::make_shared<Plane>(point, normal), material
            );
            getline(is, word);
        }
        else { // Check material
            std::string id = word;
            if (!(buffer >> word) || word != "=")
                return std::nullopt;

            if (!(buffer >> word))
                return std::nullopt;

            auto ptr = parseMaterial(word);
            if (!ptr) return std::nullopt;

            materials.push_back({id, ptr});
        }        
    }

    if (!foundCamera)
        return std::nullopt;

    return scene;
}

