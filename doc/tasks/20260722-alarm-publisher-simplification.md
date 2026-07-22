# 报警发布器简化重构

## 目标

降低业务模块触发报警的样板代码量。业务调用侧优先提供以下信息：

- 报警信息
- 报警区域
- 报警时间
- 报警等级

报警码、来源模块、来源键、标题、去重键、确认要求和自动清除策略属于固定报警定义，由发布器统一补全。首个迁移对象为 `TubeTrack` 的称重工位。

## 范围与决策

- 迁移 `TubeTrack/src/WeightPosition.cpp`，并在 `AlarmPublisher` 提供可复用接口。
- 保留现有 `AlarmRaiseRequest`、`AlarmClearRequest`、`Raise`、`Clear`，避免破坏其他模块未来或历史调用。
- 保留显式清除事件：有效重量和关闭称重功能仍产生 `clear`。
- 保留称重诊断数据：`rawWeight`、`weightEnable`、`tubeNo`、`flowNo`、`orderNo`、`itemNo` 继续写入 `detailJson`。
- 不把报警接口加到 `CPositionBase`。报警来源和去重策略属于具体报警，而非全部工位的公共行为。
- 不调整无效重量分支中现有的调试赋值和步进梁放行逻辑；那是独立的生产行为问题。

## 目标 API

### 固定报警定义

```cpp
const AlarmDefinition weightAlarm{
    "WEIGHT_DATA_INVALID",
    "AREA-A",
    "TubeTrack",
    "weight.position.measurement",
    "称重工位无效重量数据",
    "tubetrack:weight-position:invalid-data",
    true,
    false,
};
```

`AlarmDefinition` 只包含同一类报警固定不变的信息：报警码、默认区域、来源、标题、去重键、确认要求和自动清除策略。

### 业务事件

```cpp
AlarmEvent event;
event.message = "称重超时，未获得有效的重量数据，请尝试人工称重";
event.detailJson = BuildWeightAlarmDetailJson(tube, rawWeight, true);

publisher.Raise(weightAlarm, event);
```

`AlarmEvent` 提供可变字段：`message`、`areaCode`、`severity`、`occurredAt`、`detailJson`。

- 空 `areaCode` 使用定义中的默认区域。
- 空 `severity` 使用 `major`。
- 空 `occurredAt` 在发布时生成 UTC ISO-8601 时间。
- `detailJson` 默认 `{}`，业务模块可附加诊断上下文。

## 实现路径

| 步骤 | 文件 | 状态 | 内容 |
| --- | --- | --- | --- |
| 1 | `AlarmPublisher/include/AlarmPublisher.h` | 已完成 | 新增 `AlarmDefinition`、`AlarmEvent` 和轻量 `Raise` / `Clear` 重载。 |
| 2 | `AlarmPublisher/src/AlarmPublisher.cpp` | 已完成 | 校验固定定义，将轻量模型展开为既有完整请求，并复用原 Redis 发布路径。 |
| 3 | `TubeTrack/src/WeightPosition.cpp` | 已完成 | 集中称重报警定义，保留自动诊断明细，移除完整请求逐字段构造和本地 UTC 时间生成。 |
| 4 | Linux CMake 构建 | 待执行 | 构建 `AlarmPublisher` 和 `TubeTrack`。 |
| 5 | Redis 与 Web 集成回归 | 待执行 | 验证 Redis 事件结构和 `pnpm verify:alarms`。 |

## 协议兼容性

以下 Redis 契约必须保持不变：

- 事件键：`alarm:event:<dedupeKey>`
- 事件频道：`AlarmChanged`
- 事件类型：`raise`、`clear`
- 载荷字段：`alarmCode`、`areaCode`、`severity`、`sourceModule`、`sourceKey`、`title`、`message`、`detailJson`、`dedupeKey`、`occurredAt`、`eventType`

轻量接口在调用既有完整接口前补全字段，因此 JSON 序列化、Redis `SET`、Redis `PUBLISH`、Web 后端订阅和落库路径保持不变。

## 验证清单

1. 在 Linux 环境运行：

   ```bash
   cmake --preset linux-debug
   cmake --build build -j --target AlarmPublisher TubeTrack
   ```

2. 对称重工位检查以下路径：

   - `weight == -1`：发布乱码报警。
   - `weight == -2`：发布超时报警。
   - `weight == 0`：发布零重量报警。
   - `weight >= 1`：发布恢复正常的清除事件。
   - `weight_enable_ == 0`：发布功能未使能的清除事件。

3. 每条事件确认写入 `alarm:event:tubetrack:weight-position:invalid-data`、发布到 `AlarmChanged`，并保留全部协议字段和原有消息。

4. 在具备 Redis 与 PostgreSQL 的 `gt4_web` 环境运行：

   ```bash
   pnpm verify:alarms
   ```

5. 确认旧的完整请求 API 对缺失协议字段仍执行原有校验；轻量 API 仅为区域、等级和时间提供默认值。

## 回滚方案

如试点出现问题，恢复 `WeightPosition.cpp` 中原有 `AlarmRaiseRequest` 与 `AlarmClearRequest` 的逐字段构造即可。完整 API 未移除，回滚不涉及 Redis、数据库或 Web 服务变更。

## 后续迁移原则

其他工位只有在实际增加报警时才迁移到 `AlarmDefinition + AlarmEvent` 模式。每类报警创建一个固定定义，业务事件只填写变化的信息和必要诊断上下文，不预先修改没有报警调用的工位。
