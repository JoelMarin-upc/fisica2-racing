#pragma once
#include "Module.h"
#include "Globals.h"

#include <vector>
#include <string>
#include <unordered_map>

struct AnimFrame {
    Rectangle rect{};
    int durationMs = 50;
};

class Animation {
public:
    Animation();
    void AddFrame(const Rectangle& r, int durationMs);
    void SetLoop(bool v);
    void Reset();
    bool HasFinishedOnce() const;
    void Update(float dt);
    const Rectangle& GetCurrentFrame() const;
    int GetFrameCount() const;

private:
    std::vector<AnimFrame> frames_;
    int currentIndex_ = 0;
    int timeInFrameMs_ = 0;
    bool loop_ = true;
    bool finishedOnce_ = false;

    static Rectangle kEmpty_;
};

class AnimationSet {
public:
    AnimationSet();

    // load from TSX with aliases {baseTileId -> name}
    bool LoadFromTSX(const char* tsxPath,
        const std::unordered_map<int, std::string>& aliases);

    // manage animations
    void SetCurrent(const std::string& name);
    void PlayOnce(const std::string& name);
    void Update(float dtSeconds);
    const Rectangle& GetCurrentFrame() const;
    const std::string& GetCurrentName() const;

    bool Has(const std::string& name) const;

private:
    int tileW_ = 0, tileH_ = 0, columns_ = 0;
    std::unordered_map<std::string, Animation> clips_;
    std::string currentName_;
    bool playingOnce = false;
    std::string previousName_;

    static Rectangle TileIdToRect(int tileid, int columns, int tileW, int tileH);
};
