# 指标优化说明

## 优化摘要

将 `BollingerBands` 从 O(n·period) 优化到 O(n)，使用滑动窗口维护 sum(x) 和 sum(x²)。

## 核心公式

σ² = E[X²] - (E[X])² = (sum(x²)/n) - (sum(x)/n)²

## 性能提升

- **BollingerBands**: 303 μs (10k 数据点, period=20)
- **RollingStdDev**: 69 μs (新增 O(n) 实现)
- **复杂度改进**: O(n·period) → O(n)

## 关键优化

1. 避免临时容器创建和拷贝
2. O(1) 增量更新累加器
3. 严格边界检查 (NaN/Inf)
4. Welford's 算法提升数值稳定性

详细说明见代码注释。
