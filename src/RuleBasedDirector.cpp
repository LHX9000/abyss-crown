#include "RuleBasedDirector.h"

std::string RuleBasedDirector::buildHint(const GameSnapshot& snapshot) {
    switch (snapshot.lastEvent) {
        case EventType::Start:
            return "地牢已经苏醒。收集 3 个记忆碎片，然后找到出口。";
        case EventType::HitWall:
            return "冰冷的墙体挡住了道路，换一条路线试试。";
        case EventType::PickShard:
            return "记忆碎片在手中轻鸣，出口似乎更近了。";
        case EventType::NeedMoreShards:
            return "出口拒绝开启，你还没有集齐全部记忆碎片。";
        case EventType::ReachExit:
            return "大门缓缓开启，遗失的记忆在光芒中归来。";
        case EventType::Hurt:
            if (snapshot.health <= 1) {
                return "生命值已经很低，再次踏入危险区域可能会失败。";
            }
            return "危险区域对你造成了伤害，接下来要谨慎移动。";
        case EventType::Defeat:
            return "你的光芒已经熄灭，但迷宫仍记得走过的道路。";
        case EventType::Move:
            if (snapshot.shardCount == snapshot.requiredShards) {
                return "记忆碎片已经集齐，现在前往绿色出口。";
            }
            if (snapshot.health <= 1) {
                return "生命值较低，请避开紫色危险区域。";
            }
            return "继续探索迷宫，蓝色记忆碎片是逃离这里的关键。";
        case EventType::None:
        default:
            return "选择“开始游戏”，进入回声地牢。";
    }
}
