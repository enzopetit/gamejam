#pragma once

struct UpgradeOption {
    enum class Type { Damage, FireRate, MoveSpeed, PickupRadius };
    Type type;
    float value;
    int level;
};

